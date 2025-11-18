// LargeVerticalCaveFinder.cpp
//
// Paste-ready single file.
// Scans 1,000 seeds, prints top 20 vertical mega-cave candidates.
//
// Assumes your project provides:
// - MU / ND / c_* typedefs/macros, i64
// - Pos2D, Pos3D, DoublePos2D, DoublePos3D, AreaRange, BoundingBox
// - RNG with setSeed(), nextInt<T>(), nextFloat(), nextLongI()
// - MathHelper::{sin,cos} and constants: PI_FLOAT, HALF_PI_FLOAT
// - BoundingBox::makeChunkBox(), ::isVecInside()
// - lce/processor.hpp wiring

#include "common/AreaRange.hpp"
#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "common/rng.hpp"
#include "common/timer.hpp"
#include "components/BoundingBox.hpp"
#include "lce/processor.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <queue>
#include <vector>

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
    double      coverY{};          // vertical coverage inside [12,64] while a 16×16 square fits fully in the cave
    double      maxWidth{};        // max radius (blocks) seen on this branch
    double      marginMax{};       // max(adjustedRadius - NEED_R) where NEED_R is radius to contain a 16×16

    // Extras (debug/printing)
    double      avgVertical{};     // mean |sin(pitch)| over counted steps
    double      spanY{};           // y_max - y_min
    Pos3D       range{};           // last step's (max - min) voxel span (debug)

    MU inline double score() const {
        // Main: how tall a column [64..12] we actually clear at any offset.
        // Bonus: extra radius and margin (robustness vs. voxel discretization).
        static constexpr double NEED_R = 11.313708498984760390413; // sqrt(8^2 + 8^2)
        return 1.00 * coverY
             + 0.35 * std::max(0.0, maxWidth - NEED_R)
             + 0.75 * std::max(0.0, marginMax);
    }
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
        // World window (the only area where we care about mining savings)
        Pos2D lower = range.getLower();
        Pos2D upper = range.getUpper();

        BoundingBox bounds = BoundingBox::makeChunkBox(lower.x, lower.z);
        bounds.encompass(BoundingBox::makeChunkBox(upper.x, upper.z));

        std::vector<VerticalCaveHit> out;

        // Generation area extends by ±8 chunks because each chunk's cave pass looks that far
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

    // Back-compat alias
    static std::vector<VerticalCaveHit> findAllLargeCaves(c_i64 worldSeed, AreaRange& range) {
        return findAllLargeVerticalCaves(worldSeed, range);
    }

private:
    static inline double clampd(double v, double lo, double hi) {
        return (v < lo) ? lo : (v > hi ? hi : v);
    }

    static constexpr double Y_TOP  = 64.0;
    static constexpr double Y_BOT  = 12.0;
    static constexpr double NEED_R = 11.313708498984760390413; // sqrt(8^2 + 8^2)
    static constexpr double EPS    = 0.25;                     // small safety margin

    void addFeature(const Pos2D baseChunk, std::vector<VerticalCaveHit>& out) {
        const int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt<40>() + 1) + 1);
        if EXPECT_TRUE (rng.nextInt<15>() != 0) { return; } // match vanilla spawn chance

        for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
            DoublePos3D start;
            start.x = static_cast<double>(baseChunk.x * 16 + rng.nextInt<16>());
            start.y = static_cast<double>(rng.nextInt(rng.nextInt<120>() + 8));
            start.z = static_cast<double>(baseChunk.z * 16 + rng.nextInt<16>());

            int segmentCount = 1;

            if (rng.nextInt<4>() == 0) {
                const i64 seedMod = rng.nextLongI();

                // don't bother exploring shitty caves
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

                // if (baseChunk.x == 6 && baseChunk.z == -9) {
                //     std::cout << "(" << baseChunk.x << "," << baseChunk.z << "): w=" << width << " | yaw=" << yaw << " | pitch=" << pitch << "\n";
                // }

                // EARLY BREAK FOR SHITTY SMALL CAVES
                if (1.5 + static_cast<double>(width) < NEED_R - EPS) {
                    rng.nextLongI();
                    continue; // can't ever fully cover a 16x16 ⇒ not worth scoring
                }

                addTunnel(out, rng.nextLongI(), baseChunk, start, width, yaw, pitch, 0, 0, 1.0);
            }
        }
    }

    void addTunnel(std::vector<VerticalCaveHit>& out,
                   c_i64 theSeedModifier, const Pos2D theCurrentChunk, DoublePos3D theStart,
                   c_float theWidth, float theTunnelYaw, float theTunnelPitch, int theCurrentSegment,
                   int theMaxSegment, c_double theHeightMultiplier) {

        if (1.5 + static_cast<double>(theWidth) < NEED_R - EPS) {
            return;
        }

        float yawModifier   = 0.0F;
        float pitchModifier = 0.0F;
        RNG localRng; localRng.setSeed(static_cast<u64>(theSeedModifier));

        const double minX8 = static_cast<double>(genBounds.m_minX) + 8.0;
        const double maxX8 = static_cast<double>(genBounds.m_maxX) - 8.0;
        const double minZ8 = static_cast<double>(genBounds.m_minZ) + 8.0;
        const double maxZ8 = static_cast<double>(genBounds.m_maxZ) - 8.0;

        if (theMaxSegment <= 0) {
            constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
            constexpr int RANDOM_RANGE   = RANGE_BOUNDARY / 4;
            theMaxSegment = RANGE_BOUNDARY - localRng.nextInt(RANDOM_RANGE);
        }

        bool isMain = false;
        if (theCurrentSegment == -1) {
            theCurrentSegment = theMaxSegment / 2;
            isMain = true; // we do NOT special-return on main; we score the whole branch
        }

        const int   splitPoint  = localRng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
        const float segFDivPI   = PI_FLOAT / static_cast<float>(theMaxSegment);
        const bool  widePattern = localRng.nextInt<6>() == 0;

        // --- FULL-branch accumulators ---
        DoublePos3D prev = theStart;
        double coverY    = 0.0;   // total |Δy| inside [12,64] while a centered 16×16 square fits inside the cave
        double maxWidth  = 0.0;
        double marginMax = 0.0;
        double sumAbsSin = 0.0;
        int    countVert = 0;
        double yMin      =  1e9, yMax = -1e9;
        Pos3D  lastRange{};

        auto finalize_emit = [&](const DoublePos3D& pos) {
            VerticalCaveHit hit;
            hit.worldSeed   = worldSeed;
            hit.chunk       = theCurrentChunk;
            hit.approxStart = pos;
            hit.coverY      = coverY;
            hit.maxWidth    = maxWidth;
            hit.marginMax   = marginMax;
            hit.avgVertical = (countVert > 0) ? (sumAbsSin / static_cast<double>(countVert)) : 0.0;
            hit.spanY       = (yMax > yMin) ? (yMax - yMin) : 0.0;
            hit.range       = lastRange;
            out.emplace_back(hit);
        };

        for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {

            // width/height at this step
            const double adjustedWidth  = 1.5 + static_cast<double>(
                MathHelper::sin(static_cast<float>(theCurrentSegment) * segFDivPI) * theWidth);

            const double adjustedHeight = adjustedWidth * theHeightMultiplier;

            const float sPitch = MathHelper::sin(theTunnelPitch);
            const float cPitch = MathHelper::cos(theTunnelPitch);
            const float sYaw   = MathHelper::sin(theTunnelYaw);
            const float cYaw   = MathHelper::cos(theTunnelYaw);

            // advance position
            theStart.x += static_cast<double>(cYaw * cPitch);
            theStart.y += static_cast<double>(sPitch);
            theStart.z += static_cast<double>(sYaw * cPitch);

            // slope integration (order preserved)
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

            // yaw integration (order preserved)
            theTunnelYaw += yawModifier * 0.1F;
            yawModifier   = yawModifier * 0.75F;
            {
                const float f2_1 = localRng.nextFloat();
                const float f2_2 = localRng.nextFloat();
                const float f2_3 = localRng.nextFloat();
                yawModifier = yawModifier + (f2_1 - f2_2) * f2_3 * 4.0F;
            }

            // split (exact platform order): close current branch, recurse both children, then return
            if (!isMain && theCurrentSegment == splitPoint && theWidth > 1.0F && theMaxSegment > 0) {
                finalize_emit(theStart);

                float w1, w2; i64 s1, s2;
                if constexpr (IsXbox) {
                    w1 = localRng.nextFloat(); s1 = localRng.nextLongI();
                    w2 = localRng.nextFloat(); s2 = localRng.nextLongI();
                } else {
                    s1 = localRng.nextLongI(); w1 = localRng.nextFloat();
                    s2 = localRng.nextLongI(); w2 = localRng.nextFloat();
                }
                addTunnel(out, s1, theCurrentChunk, theStart, w1 * 0.5F + 0.5F,
                          theTunnelYaw - HALF_PI_FLOAT, theTunnelPitch / 3.0F, theCurrentSegment, theMaxSegment, 1.0);
                addTunnel(out, s2, theCurrentChunk, theStart, w2 * 0.5F + 0.5F,
                          theTunnelYaw + HALF_PI_FLOAT, theTunnelPitch / 3.0F, theCurrentSegment, theMaxSegment, 1.0);
                return;
            }

            // probabilistic skip as in vanilla; skipped steps don't count toward metrics
            if (!isMain && localRng.nextInt<4>() == 0) {
                prev = theStart;
                continue;
            }

            // accumulate verticality for counted steps
            sumAbsSin += std::abs(static_cast<double>(sPitch));
            ++countVert;

            // debug spans
            if (theStart.y < yMin) yMin = theStart.y;
            if (theStart.y > yMax) yMax = theStart.y;
            if (adjustedWidth > maxWidth) maxWidth = adjustedWidth;

            // simple voxel span snapshot (debug only; independent of chunk grid)
            {
                Pos3D mn, mx;
                mn.x = static_cast<int>(std::floor(theStart.x - adjustedWidth)) - 1;
                mx.x = static_cast<int>(std::floor(theStart.x + adjustedWidth)) + 1;
                mn.y = static_cast<int>(std::floor(theStart.y - adjustedHeight)) - 1;
                mx.y = static_cast<int>(std::floor(theStart.y + adjustedHeight)) + 1;
                mn.z = static_cast<int>(std::floor(theStart.z - adjustedWidth)) - 1;
                mx.z = static_cast<int>(std::floor(theStart.z + adjustedWidth)) + 1;
                if (mn.y < 1)   mn.y = 1;
                if (mx.y > 120) mx.y = 120;
                lastRange = (mx - mn);
            }

            // Best-case 16×16 square centered at current cave center (offset-agnostic).
            // Only count it if that whole square lies within the original world window.
            const bool squareInside =
                    (theStart.x >= minX8) & (theStart.x <= maxX8) &
                    (theStart.z >= minZ8) & (theStart.z <= maxZ8);

            // Margin vs. the radius required to cover the 16×16 square this step
            const double margin = adjustedWidth - NEED_R;
            if (margin > marginMax) marginMax = margin;

            // If our centered square is inside the world window and inside the cave radius,
            // accrue vertical coverage between [12,64] for this step.
            if (squareInside && margin >= 0.0) {
                const double y0 = clampd(prev.y, Y_BOT, Y_TOP);
                const double y1 = clampd(theStart.y, Y_BOT, Y_TOP);
                coverY += std::abs(y1 - y0);
            }

            prev = theStart;
        } // for segments

        // normal loop end: finalize once
        finalize_emit(theStart);
    }
};

// ----------------------- Seed scanning helper ---------------------------
template<bool IsXbox>
MU inline std::vector<VerticalCaveHit>
scanSeedsTopN(AreaRange& range, i64 seedStart, i64 seedCount, int topN)
{
    struct Node {
        double score;
        VerticalCaveHit hit;
        bool operator<(const Node& o) const { return score > o.score; } // min-heap by score
    };

    std::priority_queue<Node> heap;
    int lastPct = -1;

    for (i64 s = 0; s < seedCount; ++s) {
        const int curPct = static_cast<int>( ((s + 1) * 100.0) / static_cast<double>(seedCount) );
        if (curPct > lastPct) {
            lastPct = curPct;
            std::printf("[scan] %3d%% (%lld/%lld)\n", curPct, s + 1, seedCount);
            std::fflush(stdout);
        }

        const i64 worldSeed = seedStart + s;
        auto hits = LargeVerticalCaveFinder<IsXbox>::findAllLargeVerticalCaves(worldSeed, range);
        for (const auto& h : hits) {
            const double sc = h.score();
            if ((int)heap.size() < topN) {
                heap.push(Node{sc, h});
            } else if (sc > heap.top().score) {
                heap.pop();
                heap.push(Node{sc, h});
            }
        }
    }

    std::vector<VerticalCaveHit> best;
    best.reserve(heap.size());
    while (!heap.empty()) { best.push_back(heap.top().hit); heap.pop(); }
    std::reverse(best.begin(), best.end());
    return best;
}

} // namespace finders


// ---------------------------- Demo main ---------------------------------
int main() {
    using namespace finders;

    // Scan a window centered at (0,0) with radius 16 chunks (512×512 blocks).
    AreaRange range(Pos2D(0, 0), 16);

    // -6651998285536156346
    constexpr i64 seedStart = 1'000'000'000'000;
    constexpr i64 seedCount = 1'000'000;   // per request: 1,000 seeds
    constexpr int topN      = 10'000;       // show top 20

    Timer timer;
    auto best = scanSeedsTopN</*IsXbox=*/false>(range, seedStart, seedCount, topN);
    auto end = timer.getSeconds();
    std::cout << "Time: " << end << "\n";
    std::cout << "Seed/sec: " << static_cast<float>(seedCount) / end << "\n\n\n";

    std::printf("Found %zu vertical mega-cave candidates (top %d):\n\n", best.size(), topN);
    for (const auto& h : best) {
        std::printf(
            "score=%6.3f seed=%lld cnk=(%d,%d) maxR=%5.2f cvrY=%4.1f mMax=%3.2f "
                "avgVert=%5.3f spanY=%4.1f s=(%d,%d,%d)\n", // r=(%d,%d,%d)
            h.score(),
            h.worldSeed,
            h.chunk.x, h.chunk.z,
            h.maxWidth, h.coverY, h.marginMax,
            h.avgVertical, h.spanY,
            (int)h.approxStart.x, (int)h.approxStart.y, (int)h.approxStart.z
            // h.range.x, h.range.y, h.range.z
        );
    }
    return 0;
}
