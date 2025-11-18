// LargeVerticalCaveFinder.cpp
//
// Multithreaded + JSON-config/state + periodic atomic checkpointing (nlohmann::json).
// - Reads config from scan.json (if present), otherwise warm-starts from state.json/backup.
// - Spawns N worker threads, time-sliced (~slice_seconds) work chunks.
// - Maintains global Top-N across threads; writes best.json periodically.
// - Writes state.json atomically via tmp+rotate (Windows-safe if same dir).

#include "include/json.hpp"

#include "common/AreaRange.hpp"
#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "common/rng.hpp"
#include "common/timer.hpp"
#include "components/BoundingBox.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
static void enable_vt_mode() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(h, &mode)) return;
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using nlohmann::json;

inline constexpr int64_t STARTING_SEED = 2'200'000'000;
inline constexpr int STARTING_THREADS  = 4;

namespace finders {

    // -------------------- Shared small helpers/constants --------------------
    inline constexpr int   CHUNK_RANGE            = 8;
    MU inline constexpr float RESERVE_MULTIPLIER  = 1.4F;

    struct SeedMul { i64 x, z; };

    inline SeedMul getSeedMultiplier(c_i64 worldSeed) {
        RNG r; r.setSeed(static_cast<u64>(worldSeed));
        return { r.nextLongI(), r.nextLongI() };
    }

    // -------------------- Vertical-cave specific result ---------------------
    struct VerticalCaveHit {
        // Identity / location
        i64         worldSeed{};
        Pos2D       chunk{};           // chunk the tunnel started from
        DoublePos3D approxStart{};     // snapshot position at branch end

        // Core metrics
        double      maxWidth{};        // max radius (blocks) seen on this branch
        double      marginMax{};       // max(adjustedRadius - NEED_R) where NEED_R is radius to contain a 16×16
        double      avgVertical{};     // mean |sin(pitch)| over counted steps

        int         minYBlock = 256;
        int         maxYBlock = 0;

        MU ND inline double score() const {
            static constexpr double NEED_R       = 11.313708498984760390413;
            static constexpr int    HARD_FLOOR   = 10;
            static constexpr int    HARD_CEILING = 20;
            static constexpr int    MIN_Y        = 12;
            static constexpr int    MAX_Y        = 70;

            // tie-breaker strength: must be < 1 so span always dominates
            static constexpr double START_BIAS = 0.20;  // tweak: 0.02–0.10

            if (minYBlock <= HARD_FLOOR) return std::numeric_limits<double>::lowest();
            if (minYBlock >= HARD_CEILING) return std::numeric_limits<double>::lowest();

            const int y0   = std::max(minYBlock, MIN_Y);
            const int y1   = std::min(maxYBlock, MAX_Y);
            const int span = std::max(0, y1 - y0);

            double base =
                    1.00 * span +
                    0.35 * std::max(0.0, maxWidth - NEED_R) +
                    0.75 * std::max(0.0, marginMax);

            // Lower y0 ⇒ larger bonus. For equal span: 12–52 > 22–62 > 32–72
            const double start_bonus = START_BIAS * (MAX_Y - y0);

            return base + start_bonus;
        }
    };

    struct BranchAgg {
        int minYAny = 256;  // lowest carved Y reachable anywhere along this branch or its children
        int maxYAny = 0;    // not used by scoring now, but handy to keep
    };

    // --------------- Finder that scores the WHOLE branch (no CUT_OFF gates) -
    template<bool IsXbox>
    class LargeVerticalCaveFinder final {
    public:
        i64 worldSeed;
        RNG rng;
        BoundingBox genBounds = BoundingBox::EMPTY;

        explicit LargeVerticalCaveFinder(c_i64 _worldSeed) : worldSeed(_worldSeed) {}

        static std::vector<VerticalCaveHit> findAllLargeVerticalCaves(c_i64 worldSeed, AreaRange& range) {
            Pos2D lower = range.getLower();
            Pos2D upper = range.getUpper();

            BoundingBox bounds = BoundingBox::makeChunkBox(lower.x, lower.z);
            bounds.encompass(BoundingBox::makeChunkBox(upper.x, upper.z));

            std::vector<VerticalCaveHit> out;

            const SeedMul mul = getSeedMultiplier(worldSeed);
            lower = lower - CHUNK_RANGE;
            upper = upper + CHUNK_RANGE;

            LargeVerticalCaveFinder self(worldSeed);
            for (int x = lower.x; x <= upper.x; ++x) {
                const i64 ax = static_cast<i64>(x) * mul.x;
                for (int z = lower.z; z <= upper.z; ++z) {
                    const i64 az = static_cast<i64>(z) * mul.z;
                    self.rng.setSeed(static_cast<u64>(ax ^ az ^ worldSeed));
                    self.genBounds = bounds;
                    self.addFeature(Pos2D(x, z), out);
                }
            }
            return out;
        }

        static std::vector<VerticalCaveHit> findAllLargeCaves(c_i64 worldSeed, AreaRange& range) {
            return findAllLargeVerticalCaves(worldSeed, range);
        }

    private:
        static inline double clampd(double v, double lo, double hi) {
            return (v < lo) ? lo : (v > hi ? hi : v);
        }

        static constexpr double Y_TOP  = 64.0;
        static constexpr double Y_BOT  = 12.0;
        static constexpr double NEED_R = 11.313708498984760390413;
        static constexpr double EPS    = 0.25;

        void addFeature(const Pos2D baseChunk, std::vector<VerticalCaveHit>& out) {
            const int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt<40>() + 1) + 1);
            if EXPECT_TRUE (rng.nextInt<15>() != 0) { return; }

            for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
                DoublePos3D start;
                start.x = static_cast<double>(baseChunk.x * 16 + rng.nextInt<16>());
                start.y = static_cast<double>(rng.nextInt(rng.nextInt<120>() + 8));
                start.z = static_cast<double>(baseChunk.z * 16 + rng.nextInt<16>());

                int segmentCount = 1;

                if (rng.nextInt<4>() == 0) {
                    const i64 seedMod = rng.nextLongI();
                    float w0 = 1.0F + rng.nextFloat() * 6.0F;
                    if (1.5 + static_cast<double>(w0) >= NEED_R - EPS) {
                        addTunnel(out, seedMod, baseChunk, start, w0, 0.0F, 0.0F, -1, -1, 0.5);
                    }
                    segmentCount = rng.nextInt<4>() + 1;
                }

                for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
                    const float yaw   = rng.nextFloat() * (PI_FLOAT * 2.0F);
                    const float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
                    float width       = rng.nextFloat() * 2.0F + rng.nextFloat();

                    if (rng.nextInt(10) == 0) { width *= rng.nextFloat() * rng.nextFloat() * 3.0F + 1.0F; }

                    if (1.5 + static_cast<double>(width) < NEED_R - EPS) {
                        rng.nextLongI();
                        continue;
                    }

                    addTunnel(out, rng.nextLongI(), baseChunk, start, width, yaw, pitch, 0, 0, 1.0);
                }
            }
        }

        BranchAgg addTunnel(std::vector<VerticalCaveHit>& out,
                            c_i64 theSeedModifier, const Pos2D theCurrentChunk, DoublePos3D theStart,
                            c_float theWidth, float theTunnelYaw, float theTunnelPitch, int theCurrentSegment,
                            int theMaxSegment, c_double theHeightMultiplier) {

            BranchAgg agg{}; // we'll always return the "any-width" reach, even if we don't emit a hit

            if (1.5 + static_cast<double>(theWidth) < NEED_R - EPS) {
                return agg;
            }

            float yawModifier   = 0.0F;
            float pitchModifier = 0.0F;
            RNG localRng; localRng.setSeed(static_cast<u64>(theSeedModifier));

            if (theMaxSegment <= 0) {
                constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
                constexpr int RANDOM_RANGE   = RANGE_BOUNDARY / 4;
                theMaxSegment = RANGE_BOUNDARY - localRng.nextInt(RANDOM_RANGE);
            }

            bool isMain = false;
            if (theCurrentSegment == -1) {
                theCurrentSegment = theMaxSegment / 2;
                isMain = true;
            }

            const int   splitPoint  = localRng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
            const float segFDivPI   = PI_FLOAT / static_cast<float>(theMaxSegment);
            const bool  widePattern = localRng.nextInt<6>() == 0;

            // ---------- (1) PRE-CHECK: first-step carve window ----------
            {
                const int   s0      = theCurrentSegment;                   // already set to mid if main
                const float sinArg  = static_cast<float>(s0) * segFDivPI;
                const double adjW0  = 1.5 + static_cast<double>(MathHelper::sin(sinArg) * theWidth);
                const double H0     = adjW0 * theHeightMultiplier;
                const int carveMin0 = std::max(1, (int)std::floor(theStart.y - 0.7 * H0 + 0.5) + 1);
                if (carveMin0 < (int)Y_BOT) {
                    // definitely intersects lava band; prune this branch entirely
                    agg.minYAny = carveMin0;
                    agg.maxYAny = carveMin0;
                    return agg;
                }
            }

            // --- Big-section carve-consistent tracking (for score Y-range only) ---
            bool hadBig = false;
            int  minYCarveBig = 256;
            int  maxYCarveBig = 0;

            // --- "Any-width" reach across this branch (used to veto lava leaks) ---
            int  minYAny = 256;
            int  maxYAny = 0;

            double maxWidth  = 0.0;
            double marginMax = 0.0;
            double sumAbsSin = 0.0;
            int    countVert = 0;

            // Precompute interior (big-section) XZ bounds once
            const double minX8 = static_cast<double>(genBounds.m_minX) + 8.0;
            const double maxX8 = static_cast<double>(genBounds.m_maxX) - 8.0;
            const double minZ8 = static_cast<double>(genBounds.m_minZ) + 8.0;
            const double maxZ8 = static_cast<double>(genBounds.m_maxZ) - 8.0;

            auto emit_if_ok = [&](const DoublePos3D& pos, int minYAnyCombined) {
                // If the connected branch (this + children) reaches below Y_BOT, drop the hit.
                if (!hadBig || minYCarveBig > maxYCarveBig) return;    // not a “big section” branch at all
                if (minYAnyCombined < static_cast<int>(Y_BOT)) return; // leaks to lava depth -> reject

                VerticalCaveHit hit;
                hit.worldSeed   = worldSeed;
                hit.chunk       = theCurrentChunk;
                hit.approxStart = pos;
                hit.maxWidth    = maxWidth;
                hit.marginMax   = marginMax;
                hit.avgVertical = (countVert > 0) ? (sumAbsSin / static_cast<double>(countVert)) : 0.0;
                hit.minYBlock   = minYCarveBig;
                hit.maxYBlock   = maxYCarveBig;
                out.emplace_back(hit);
            };

            for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {
                const double adjustedWidth  = 1.5 + static_cast<double>(
                                                           MathHelper::sin(static_cast<float>(theCurrentSegment) * segFDivPI) * theWidth);
                const double adjustedHeight = adjustedWidth * theHeightMultiplier;

                // Advance parametric position
                const float sPitch = MathHelper::sin(theTunnelPitch);
                const float cPitch = MathHelper::cos(theTunnelPitch);
                const float sYaw   = MathHelper::sin(theTunnelYaw);
                const float cYaw   = MathHelper::cos(theTunnelYaw);

                theStart.x += static_cast<double>(cYaw * cPitch);
                theStart.y += static_cast<double>(sPitch);
                theStart.z += static_cast<double>(sYaw * cPitch);

                // Evolve slope/yaw
                if (widePattern) theTunnelPitch = theTunnelPitch * 0.92F;
                else             theTunnelPitch = theTunnelPitch * 0.7F;
                theTunnelPitch = theTunnelPitch + pitchModifier * 0.1F;
                pitchModifier  = pitchModifier * 0.9F;
                {
                    const float f1_1 = localRng.nextFloat();
                    const float f1_2 = localRng.nextFloat();
                    const float f1_3 = localRng.nextFloat();
                    pitchModifier = pitchModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
                }

                theTunnelYaw += yawModifier * 0.1F;
                yawModifier   = yawModifier * 0.75F;
                {
                    const float f2_1 = localRng.nextFloat();
                    const float f2_2 = localRng.nextFloat();
                    const float f2_3 = localRng.nextFloat();
                    yawModifier = yawModifier + (f2_1 - f2_2) * f2_3 * 4.0F;
                }

                // ---- Update aggregates ----
                sumAbsSin += std::abs(static_cast<double>(sPitch));
                ++countVert;
                if (adjustedWidth > maxWidth) maxWidth = adjustedWidth;

                const double H = adjustedHeight;

                // "Any-width" carve reach (no big-section gate). Matches carve math vertically.
                const int carveMinAny = std::max(1,   (int)std::floor(theStart.y - 0.7 * H + 0.5) + 1);
                const int carveMaxAny = std::min(120, (int)std::ceil (theStart.y + 1.0 * H + 0.5) - 1);
                if (carveMinAny < minYAny) minYAny = carveMinAny;
                if (carveMaxAny > maxYAny) maxYAny = carveMaxAny;

                // ---------- (2) LOOP SHORT-CIRCUIT ----------
                if (minYAny < (int)Y_BOT) {
                    // no need to simulate/split further; this branch (or its future children)
                    // already reaches lava band
                    agg.minYAny = minYAny;
                    agg.maxYAny = maxYAny;
                    return agg;
                }

                // Big-section gate: 16×16 fits & inside the interior scan bounds
                const bool squareInside =
                        (theStart.x >= minX8) & (theStart.x <= maxX8) &
                        (theStart.z >= minZ8) & (theStart.z <= maxZ8);

                const double margin = adjustedWidth - NEED_R;
                if (margin > marginMax) marginMax = margin;

                if (squareInside && margin >= 0.0) {
                    const int carveMinBig = std::max(1,   (int)std::floor(theStart.y - 0.7 * H + 0.5) + 1);
                    const int carveMaxBig = std::min(120, (int)std::ceil (theStart.y + 1.0 * H + 0.5) - 1);
                    if (carveMinBig <= carveMaxBig) {
                        hadBig = true;
                        if (carveMinBig < minYCarveBig) minYCarveBig = carveMinBig;
                        if (carveMaxBig > maxYCarveBig) maxYCarveBig = carveMaxBig;
                    }
                }

                // ---- Split into children ----
                if (!isMain && theCurrentSegment == splitPoint && theWidth > 1.0F && theMaxSegment > 0) {
                    // Recurse to children FIRST to learn their "any-width" minima,
                    // then decide whether to emit this parent hit (veto lava leaks).
                    float w1, w2; i64 s1, s2;
                    if constexpr (IsXbox) {
                        w1 = localRng.nextFloat(); s1 = localRng.nextLongI();
                        w2 = localRng.nextFloat(); s2 = localRng.nextLongI();
                    } else {
                        s1 = localRng.nextLongI(); w1 = localRng.nextFloat();
                        s2 = localRng.nextLongI(); w2 = localRng.nextFloat();
                    }

                    BranchAgg a1 = addTunnel(out, s1, theCurrentChunk, theStart, w1 * 0.5F + 0.5F,
                                             theTunnelYaw - HALF_PI_FLOAT, theTunnelPitch / 3.0F,
                                             theCurrentSegment, theMaxSegment, 1.0);
                    BranchAgg a2 = addTunnel(out, s2, theCurrentChunk, theStart, w2 * 0.5F + 0.5F,
                                             theTunnelYaw + HALF_PI_FLOAT, theTunnelPitch / 3.0F,
                                             theCurrentSegment, theMaxSegment, 1.0);

                    const int minYAnyCombined = std::min(minYAny, std::min(a1.minYAny, a2.minYAny));
                    emit_if_ok(theStart, minYAnyCombined);

                    // Return aggregate upward
                    agg.minYAny = minYAnyCombined;
                    agg.maxYAny = std::max(maxYAny, std::max(a1.maxYAny, a2.maxYAny));
                    return agg;
                }

                // "random skip" like original; we already updated 'any-width' reach above
                if (!isMain && localRng.nextInt<4>() == 0) {
                    continue;
                }
            }

            // Normal end of branch: emit after loop, using this branch's any-width minima
            emit_if_ok(theStart, minYAny);

            agg.minYAny = minYAny;
            agg.maxYAny = maxYAny;
            return agg;
        }
    };

    struct HitNode {
        double score{};
        VerticalCaveHit hit{};
        bool operator<(const HitNode& o) const { return score > o.score; }
    };

    class GlobalTopN {
        std::priority_queue<HitNode> heap_;
        size_t cap_;
        mutable std::mutex m_;
    public:
        explicit GlobalTopN(size_t cap): cap_(cap) {}

        void push(const VerticalCaveHit& h) {
            const double sc = h.score();
            std::lock_guard<std::mutex> lk(m_);
            if (heap_.size() < cap_) heap_.push({sc,h});
            else if (sc > heap_.top().score) { heap_.pop(); heap_.push({sc,h}); }
        }

        template<class It>
        void push_many(It b, It e) {
            std::lock_guard<std::mutex> lk(m_);
            for (auto it=b; it!=e; ++it) {
                double sc = it->score();
                if (heap_.size() < cap_) heap_.push({sc,*it});
                else if (sc > heap_.top().score) { heap_.pop(); heap_.push({sc,*it}); }
            }
        }

        std::vector<VerticalCaveHit> snapshot_top_k(size_t k) const {
            std::lock_guard<std::mutex> lk(m_);
            std::priority_queue<HitNode> tmp = heap_;   // O(N) copy
            std::vector<VerticalCaveHit> v;
            v.reserve(std::min(k, tmp.size()));
            for (size_t i = 0; i < k && !tmp.empty(); ++i) {
                v.push_back(tmp.top().hit);
                tmp.pop();
            }
            std::reverse(v.begin(), v.end()); // highest score last -> first
            return v;
        }

        std::vector<VerticalCaveHit> snapshot_sorted() const {
            std::lock_guard<std::mutex> lk(m_);
            std::vector<VerticalCaveHit> v; v.reserve(heap_.size());
            auto tmp = heap_;
            while (!tmp.empty()) { v.push_back(tmp.top().hit); tmp.pop(); }
            std::reverse(v.begin(), v.end());
            return v;
        }

        size_t size() const {
            std::lock_guard<std::mutex> lk(m_);
            return heap_.size();
        }
    };

    // ---------------------- JSON helpers (nlohmann) -------------------------
    static inline std::optional<json> load_json(const fs::path& p) {
        std::ifstream f(p);
        if (!f) return std::nullopt;
        try {
            json j; f >> j; return j;
        } catch (...) { return std::nullopt; }
    }

    static inline bool save_json_atomic(const fs::path& mainPath,
                                        const json& j,
                                        const fs::path& backupPath,
                                        bool rotate_backup = true)
    {
        try {
            const fs::path tmp = mainPath.string() + ".tmp";
            {
                std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
                if (!out) return false;
                out << j.dump();
                out.flush();
            }
            if (rotate_backup) {
                if (fs::exists(backupPath)) fs::remove(backupPath);
                if (fs::exists(mainPath))   fs::rename(mainPath, backupPath);
            } else if (fs::exists(mainPath)) {
                fs::remove(mainPath);
            }
            fs::rename(tmp, mainPath);
            return true;
        } catch (...) { return false; }
    }

    // -------------------------- Config / State -------------------------------
    struct RunnerConfig {
        fs::path state_path = "state.json";
        fs::path backup_path = "state.backup.json";
        fs::path best_path   = "best.json";

        i64   center_x = 0;
        i64   center_z = 0;
        i64   radius   = 27;
        i64   current_seed = STARTING_SEED;
        i64   seed_count   = 0;  // 0 = infinite
        int   threads      = std::max(1, STARTING_THREADS); // std::thread::hardware_concurrency()
        int   top_n        = 10000;
        double slice_seconds = 30.0;
        double save_interval_seconds = 10.0;
    };

    static inline RunnerConfig load_config_or_state(const fs::path& cfgPath) {
        RunnerConfig c;

        // 1) Try explicit config file (scan.json)
        if (auto jc = load_json(cfgPath)) {
            const json& j = *jc;
            c.state_path  = j.value("state_path",  c.state_path.string());
            c.backup_path = j.value("backup_path", c.backup_path.string());
            c.best_path   = j.value("best_path",   c.best_path.string());

            c.center_x = (i64) j.value("center_x", (long long)c.center_x);
            c.center_z = (i64) j.value("center_z", (long long)c.center_z);
            c.radius   = (i64) j.value("radius",   (long long)c.radius);

            c.current_seed = (i64) j.value("current_seed", (long long)c.current_seed);
            c.seed_count   = (i64) j.value("seed_count",   (long long)c.seed_count);
            c.threads      = (int) j.value("threads",      (int)c.threads);
            c.top_n        = (int) j.value("top_n",        (int)c.top_n);
            c.slice_seconds         = j.value("slice_seconds",         c.slice_seconds);
            c.save_interval_seconds = j.value("save_interval_seconds", c.save_interval_seconds);
            return c;
        }

        // 2) No cfgPath or unreadable. Try state or backup.
        auto js = load_json(c.state_path);
        if (!js) js = load_json(c.backup_path);
        if (js) {
            const json& j = *js;
            c.state_path  = j.value("state_path",  c.state_path.string());
            c.backup_path = j.value("backup_path", c.backup_path.string());
            c.best_path   = j.value("best_path",   c.best_path.string());

            c.center_x = (i64) j.value("center_x", (long long)c.center_x);
            c.center_z = (i64) j.value("center_z", (long long)c.center_z);
            c.radius   = (i64) j.value("radius",   (long long)c.radius);

            c.current_seed = (i64) j.value("current_seed", (long long)c.current_seed);
            c.seed_count   = (i64) j.value("seed_count",   (long long)c.seed_count);
            c.threads      = (int) j.value("threads",      (int)c.threads);
            c.top_n        = (int) j.value("top_n",        (int)c.top_n);
            c.slice_seconds         = j.value("slice_seconds",         c.slice_seconds);
            c.save_interval_seconds = j.value("save_interval_seconds", c.save_interval_seconds);
        }
        return c;
    }

    // ------------------------ JSON writers (best/state) ----------------------
    static inline json make_json_best(const std::vector<VerticalCaveHit>& best,
                                      i64 from_seed, i64 next_seed, int threads) {
        json o;
        o["generated_at"] = (long long)std::time(nullptr);
        o["from_seed"]    = from_seed;
        o["next_seed"]    = next_seed;
        o["threads"]      = threads;
        auto& arr = o["top"] = json::array();

        for (const auto& h : best) {
            json item;
            item["score"]     = h.score();
            item["seed"]      = h.worldSeed;
            item["chunk"]     = { {"x", h.chunk.x}, {"z", h.chunk.z} };
            item["maxR"]      = h.maxWidth;
            item["marginMax"] = h.marginMax;
            item["avgVert"]   = h.avgVertical;
            item["minY"]      = h.minYBlock;
            item["maxY"]      = h.maxYBlock;
            item["start"]     = { {"x", (int)h.approxStart.x},
                                  {"y", (int)h.approxStart.y},
                                  {"z", (int)h.approxStart.z} };
            arr.push_back(std::move(item));
        }
        return o;
    }

    static inline json make_json_state(const RunnerConfig& c,
                                       i64 next_seed, double seeds_per_sec) {
        json o;
        o["state_path"]  = c.state_path.string();
        o["backup_path"] = c.backup_path.string();
        o["best_path"]   = c.best_path.string();

        o["current_seed"] = next_seed;
        o["seed_count"]   = c.seed_count;
        o["threads"]      = c.threads;
        o["top_n"]        = c.top_n;

        o["center_x"]     = c.center_x;
        o["center_z"]     = c.center_z;
        o["radius"]       = c.radius;

        o["slice_seconds"]          = c.slice_seconds;
        o["save_interval_seconds"]  = c.save_interval_seconds;
        o["updated_at"]             = (long long)std::time(nullptr);
        o["seeds_per_sec"]          = seeds_per_sec;
        return o;
    }

    // ---- load previous best.json into the global Top-N ----
    static inline size_t load_best_into_global(const fs::path& bestPath, GlobalTopN& globalTop) {
        auto jb = load_json(bestPath);
        if (!jb) return 0;
        const json& j = *jb;
        if (!j.contains("top") || !j["top"].is_array()) return 0;

        size_t loaded = 0;
        for (const auto& obj : j["top"]) {
            VerticalCaveHit h{};
            h.worldSeed   = (i64)  obj.value("seed", 0LL);
            if (obj.contains("chunk")) {
                const auto& ch = obj["chunk"];
                h.chunk.x = ch.value("x", 0);
                h.chunk.z = ch.value("z", 0);
            }
            h.maxWidth    =        obj.value("maxR", 0.0);
            h.marginMax   =        obj.value("marginMax", 0.0);
            h.avgVertical =        obj.value("avgVert", 0.0);
            h.minYBlock   =        obj.value("minY", 0);
            h.maxYBlock   =        obj.value("maxY", 0);
            if (obj.contains("start")) {
                const auto& st = obj["start"];
                h.approxStart.x = st.value("x", 0);
                h.approxStart.y = st.value("y", 0);
                h.approxStart.z = st.value("z", 0);
            }
            globalTop.push(h);
            ++loaded;
        }
        return loaded;
    }

    // --------------------------- Worker runner -------------------------------
    struct RunnerStats {
        std::atomic<i64> seeds_done{0};
        std::atomic<i64> started_seed{0};
        std::atomic<i64> next_seed{0};
        std::atomic<double> last_sps{0.0};
    };

    template<bool IsXbox>
    static inline void worker_thread(int /*tid*/,
                                     AreaRange range,
                                     std::atomic<i64>& nextSeed,
                                     const i64 endSeedExclusive, // if 0 => infinite
                                     double sliceSeconds,
                                     GlobalTopN& globalTop,
                                     RunnerStats& stats,
                                     std::atomic<bool>& stopFlag)
    {
        i64 block = 25000;

        while (!stopFlag.load(std::memory_order_relaxed)) {
            const i64 start = nextSeed.fetch_add(block, std::memory_order_relaxed);
            if (endSeedExclusive > 0 && start >= endSeedExclusive) break;

            const i64 localEnd = (endSeedExclusive>0) ? std::min(endSeedExclusive, start + block)
                                                        : (start + block);

            Timer t;
            std::vector<VerticalCaveHit> localBest; localBest.reserve(256);

            for (i64 s = start; s < localEnd; ++s) {
                auto hits = LargeVerticalCaveFinder<IsXbox>::findAllLargeVerticalCaves(s, range);
                for (const auto& h : hits) localBest.push_back(h);
            }

            globalTop.push_many(localBest.begin(), localBest.end());

            const double secs = t.getSeconds();
            const i64 done = (localEnd - start);
            stats.seeds_done.fetch_add(done, std::memory_order_relaxed);
            stats.next_seed.store(localEnd, std::memory_order_relaxed);
            if (secs > 0) stats.last_sps.store(done / secs, std::memory_order_relaxed);

            if (secs > 0.2) {
                double scale = sliceSeconds / secs;
                if (scale < 0.25) scale = 0.25;
                if (scale > 4.00) scale = 4.00;
                block = std::max<i64>(1000, (i64)std::llround(block * scale));
            }
        }
    }
} // namespace finders



int main(int argc, char** argv) {
#ifdef _WIN32
    enable_vt_mode();
#endif

    using namespace finders;

    // CLI: optional config path
    fs::path cfgPath = (argc >= 2) ? fs::path(argv[1]) : fs::path("state.json");
    RunnerConfig cfg = load_config_or_state(cfgPath);

    // Scan window centered at (0,0), radius 16 chunks (512×512)
    AreaRange range(Pos2D((int)cfg.center_x, (int)cfg.center_z),
                    (int)(cfg.radius > 0 ? cfg.radius : 16));

    // Progress + aggregation
    std::atomic<i64> nextSeed{cfg.current_seed};
    const i64 seedEnd = (cfg.seed_count > 0) ? (cfg.current_seed + cfg.seed_count) : 0;
    GlobalTopN globalTop(cfg.top_n);

    // >>> Warm start leaderboard from best.json <<<
    {
        size_t pre = load_best_into_global(cfg.best_path, globalTop);
        if (pre > 0) {
            std::printf("[init] preloaded %zu best hits from %s\n",
                        (size_t)pre, cfg.best_path.string().c_str());
        }
    }

    RunnerStats stats;
    stats.started_seed.store(cfg.current_seed);
    stats.next_seed.store(cfg.current_seed);

    std::atomic<bool> stopFlag{false};

    // Launch workers
    const int T = std::max(1, cfg.threads);
    std::vector<std::thread> pool;
    pool.reserve(T);
    for (int i=0;i<T;++i) {
        pool.emplace_back(worker_thread</*IsXbox=*/false>,
                          i, range, std::ref(nextSeed), seedEnd,
                          cfg.slice_seconds, std::ref(globalTop), std::ref(stats), std::ref(stopFlag));
    }

    // Saver/Logger loop
    Timer wall;
    double lastSave = 0.0;
    double lastLog  = 0.0;
    double emaSPS = 0.0; bool haveEma=false;

    while (true) {
        std::this_thread::sleep_for(250ms);
        const double now = wall.getSeconds();

        static int lastBlockLines = 0;

        if (now - lastLog >= 1.0) {
            lastLog = now;

            const double sps_inst = stats.last_sps.load();
            if (!haveEma) { emaSPS = sps_inst; haveEma = true; }
            else          { emaSPS = 0.9*emaSPS + 0.1*sps_inst; }

            // Take a snapshot of the current Top-N and show the first 5
            const auto best = globalTop.snapshot_sorted();
            const int showN = (int)std::min<size_t>(5, best.size());

            // 1) Move cursor up to the start of the old block and clear it line-by-line
            if (lastBlockLines > 0) {
                std::printf("\033[%dA", lastBlockLines);
                for (int i = 0; i < lastBlockLines; ++i) {
                    std::printf("\033[2K\r\n");
                }
                std::printf("\033[%dA", lastBlockLines);
            }

            // 2) Print the new block
            if (seedEnd > 0) {
                const double pct = 100.0 * double(nextSeed.load() - cfg.current_seed)
                                   / double(seedEnd - cfg.current_seed);
                std::printf("\033[2K\r[scan] %3.0f%% (%lld/%lld)  sps=%.0f  top=%zu\n",
                            pct,
                            (long long)(nextSeed.load() - cfg.current_seed),
                            (long long)(seedEnd - cfg.current_seed),
                            emaSPS * T, (unsigned long long)globalTop.size());
            } else {
                std::printf("\033[2K\r[scan] seeds=%lld  sps=%.0f  top=%zu\n",
                            (long long)(nextSeed.load() - cfg.current_seed),
                            emaSPS * T, (unsigned long long)globalTop.size());
            }

            // spacer line
            std::printf("\033[2K\r\n");

            // top-5 detail lines
            for (int i = 0; i < showN; ++i) {
                const auto& h = best[i];
                std::printf(
                        "\033[2K\r  #%02d  sc=%6.3f  seed=%lld  c=(%3d,%3d) mMax=%4.2f  sp=(%4d,%3d,%4d) YRange=(%d-%d)\n",
                        i+1, h.score(), h.worldSeed, h.chunk.x, h.chunk.z,
                        h.marginMax, (int)h.approxStart.x, (int)h.approxStart.y, (int)h.approxStart.z,
                        h.minYBlock, h.maxYBlock
                );
            }

            std::fflush(stdout);

            // 3) Remember how many lines we drew this time:
            lastBlockLines = 2 + showN;
        }

        // periodic save
        if (now - lastSave >= std::max(1.0, cfg.save_interval_seconds)) {
            lastSave = now;

            const auto best = globalTop.snapshot_sorted();
            const i64 curNext = nextSeed.load();

            // best.json (atomic, no rotate)
            {
                json jbest = make_json_best(best, cfg.current_seed, curNext, T);
                save_json_atomic(cfg.best_path, jbest, /*backup*/{}, /*rotate_backup=*/false);
            }

            // state.json (atomic rotate via backup)
            {
                RunnerConfig snapshot = cfg;
                json jstate = make_json_state(snapshot, curNext, emaSPS * T);
                save_json_atomic(cfg.state_path, jstate, cfg.backup_path, /*rotate_backup=*/true);
            }
        }

        // stop condition if finite
        if (seedEnd > 0 && nextSeed.load() >= seedEnd) {
            static double graceStart = -1.0;
            if (graceStart < 0) graceStart = now;
            if (now - graceStart > 2.0) { stopFlag.store(true); break; }
        }
    }


    // Join workers
    stopFlag.store(true);
    for (auto& th : pool) if (th.joinable()) th.join();

    // Final save
    const auto best = globalTop.snapshot_sorted();
    const i64 curNext = nextSeed.load();
    {
        json jbest = make_json_best(best, cfg.current_seed, curNext, T);
        save_json_atomic(cfg.best_path, jbest, /*backup*/{}, /*rotate_backup=*/false);
    }
    {
        RunnerConfig snapshot = cfg;
        json jstate = make_json_state(snapshot, curNext, 0.0);
        save_json_atomic(cfg.state_path, jstate, cfg.backup_path, /*rotate_backup=*/true);
    }

    std::printf("\nDone. Wrote %s and %s\n",
                cfg.state_path.string().c_str(),
                cfg.best_path.string().c_str());
    return 0;
}
