// Stronghold mining tool: builds the datasets behind tools/stronghold_mining/analysis.
//
//   EXE_StrongholdMining selftest --scale small
//   EXE_StrongholdMining dataset  --scale small --seeds 10000000 --out data
//   EXE_StrongholdMining oracle   --scale small --seeds 20000 --out data     (after dataset: needs its kernels)
//
// See tools/stronghold_mining/README.md for what every output file contains.

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "include/json.hpp"
#include "sh_core.hpp"
#include "sh_io.hpp"
#include "sh_raster.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;
using namespace shm;

namespace {

    constexpr int FORMAT_VERSION = 1;
    constexpr u64 ORACLE_SALT = 0x4F5241434C45ULL; // "ORACLE"

    // ------------------------------------------------------------------------------------------------- CLI

    struct Args {
        std::string mode;
        std::string scale;
        std::string console = "xbox1";
        std::string version = "elytra";
        u64 seeds = 0; ///< 0 = mode default
        u64 firstSample = 0;
        u64 runSeed = 1;
        int threads = 0; ///< 0 = all hardware threads
        std::string out = ".";
        u64 kernelSeeds = 250000;
        bool balanced = true;
        bool balancedOnly = false;
        int eyeY = 62;
        u64 streams = 4096;
        std::string kernels;   ///< oracle: directory with the dataset outputs (default: --out)
        std::string kernelTag; ///< oracle: tag of the dataset run whose kernels to use (default: this run's tag)
        std::string targets = "any_h:0,any_h:30,any_3d:30";
    };

    const char* USAGE = R"(usage: EXE_StrongholdMining <selftest|dataset|oracle> --scale <small|medium|large> [options]

common options
  --console NAME       xbox1 (default), xbox360, ps4, ...
  --version NAME       elytra (default), aquatic, bountiful, horse, potions, adventure, pillage
  --seeds N            worlds to sample (defaults: selftest 2000, dataset 1000000, oracle 10000)
  --first-sample I     index of the first sample (default 0); samples I .. I+N-1 are generated
  --run-seed S         base value for sampling worlds (default 1); same value + same index = same world
  --threads T          worker threads (default: all hardware threads)
  --out DIR            output directory (default .)
  --no-balanced        skip the "Find Balanced Seed" test (faster, balanced column becomes -1)
  --balanced-only      sample like "Find Balanced Seed" is ON: every world is balanced (slow, ~1 in 500
                       candidates passes); outputs get the tag <scale>_balanced
  --eye-y Y            eye height for the 3D xray kernel (default 62)

dataset options
  --kernel-seeds K     worlds that contribute to the kernels (default 250000)

oracle options
  --streams M          RNG streams per world (default 4096)
  --kernels DIR        directory holding the dataset run for the same scale (default: --out)
  --kernel-tag TAG     use the kernels of that dataset run (e.g. small for a --balanced-only oracle;
                       layouts never depend on biomes, so any run's kernels are valid)
  --targets LIST       kernel:radius pairs (default any_h:0,any_h:30,any_3d:30)
)";

    [[noreturn]] void usage(const std::string& error) {
        std::cerr << "error: " << error << "\n\n" << USAGE;
        std::exit(2);
    }

    std::string lower(std::string s) {
        std::ranges::transform(s, s.begin(), [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    u64 parseU64(const std::string& key, const std::string& v) {
        try {
            size_t used = 0;
            const unsigned long long x = std::stoull(v, &used, 0);
            if (used != v.size()) throw std::invalid_argument(v);
            return x;
        } catch (const std::exception&) { usage("invalid number for " + key + ": " + v); }
    }

    Args parseArgs(const int argc, char** argv) {
        if (argc < 2) usage("missing mode");
        Args a;
        a.mode = argv[1];
        for (int i = 2; i < argc; ++i) {
            const std::string k = argv[i];
            auto value = [&]() -> std::string {
                if (i + 1 >= argc) usage("missing value for " + k);
                return argv[++i];
            };
            if (k == "--scale") a.scale = lower(value());
            else if (k == "--console")
                a.console = lower(value());
            else if (k == "--version")
                a.version = lower(value());
            else if (k == "--seeds")
                a.seeds = parseU64(k, value());
            else if (k == "--first-sample")
                a.firstSample = parseU64(k, value());
            else if (k == "--run-seed")
                a.runSeed = parseU64(k, value());
            else if (k == "--threads")
                a.threads = static_cast<int>(parseU64(k, value()));
            else if (k == "--out")
                a.out = value();
            else if (k == "--kernel-seeds")
                a.kernelSeeds = parseU64(k, value());
            else if (k == "--no-balanced")
                a.balanced = false;
            else if (k == "--balanced-only")
                a.balancedOnly = true;
            else if (k == "--eye-y")
                a.eyeY = static_cast<int>(parseU64(k, value()));
            else if (k == "--streams")
                a.streams = parseU64(k, value());
            else if (k == "--kernels")
                a.kernels = value();
            else if (k == "--kernel-tag")
                a.kernelTag = lower(value());
            else if (k == "--targets")
                a.targets = value();
            else if (k == "--help" || k == "-h")
                usage("help requested");
            else
                usage("unknown option " + k);
        }
        if (a.mode != "selftest" && a.mode != "dataset" && a.mode != "oracle") usage("unknown mode " + a.mode);
        if (a.scale != "small" && a.scale != "medium" && a.scale != "large")
            usage("--scale must be small, medium or large");
        if (a.threads <= 0) a.threads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
        if (a.kernels.empty()) a.kernels = a.out;
        if (a.balancedOnly && !a.balanced) usage("--balanced-only and --no-balanced exclude each other");
        return a;
    }

    /// File tag of a run: the biome scale, plus "_balanced" for --balanced-only runs.
    std::string tagOf(const Args& a) { return a.scale + (a.balancedOnly ? "_balanced" : ""); }

    lce::BIOMESCALE parseScale(const std::string& s) {
        if (s == "small") return lce::BIOMESCALE::SMALL;
        if (s == "medium") return lce::BIOMESCALE::MEDIUM;
        return lce::BIOMESCALE::LARGE;
    }

    LCEVERSION parseVersion(const std::string& s) {
        static const std::vector<std::pair<std::string, LCEVERSION>> names = {
                {"aquatic", LCEVERSION::AQUATIC},     {"elytra", LCEVERSION::ELYTRA},
                {"bountiful", LCEVERSION::BOUNTIFUL}, {"horse", LCEVERSION::HORSE},
                {"potions", LCEVERSION::POTIONS},     {"adventure", LCEVERSION::ADVENTURE},
                {"pillage", LCEVERSION::PILLAGE}};
        for (const auto& [name, v]: names)
            if (name == s) return v;
        usage("unknown --version " + s);
    }

    Settings makeSettings(const Args& a) {
        Settings s;
        s.console = lce::strToConsole(a.console);
        if (s.console == lce::CONSOLE::NONE) usage("unknown --console " + a.console);
        s.version = parseVersion(a.version);
        s.scale = parseScale(a.scale);
        s.runSeed = a.runSeed;
        s.balancedCheck = a.balanced;
        s.balancedOnly = a.balancedOnly;
        return s;
    }

    json settingsJson(const Args& a, const u64 worlds) {
        return json{{"tool", "EXE_StrongholdMining"},
                    {"format_version", FORMAT_VERSION},
                    {"mode", a.mode},
                    {"console", a.console},
                    {"version", a.version},
                    {"world_size", "classic"},
                    {"biome_scale", a.scale},
                    {"run_seed", a.runSeed},
                    {"first_sample", a.firstSample},
                    {"worlds", worlds},
                    {"balanced_check", a.balanced},
                    {"balanced_only", a.balancedOnly},
                    {"tag", tagOf(a)},
                    {"threads", a.threads},
                    {"eye_y", a.eyeY},
                    {"world_half", WORLD_HALF},
                    {"player_spawn_radius", PLAYER_SPAWN_RADIUS},
                    {"grid_step", GRID_STEP},
                    {"grid_half", GRID_HALF}};
    }

    void writeJson(const std::string& path, const json& j) {
        std::ofstream f(path);
        if (!f) throw std::runtime_error("cannot write " + path);
        f << j.dump(2) << "\n";
    }

    // --------------------------------------------------------------------------------------------- threading

    using Clock = std::chrono::steady_clock;

    double secondsSince(const Clock::time_point t0) { return std::chrono::duration<double>(Clock::now() - t0).count(); }

    /// Runs body(thread) on `threads` threads while printing progress; rethrows the first worker exception.
    struct Pool {
        std::atomic<u64> next{0};
        std::atomic<u64> done{0};
        std::atomic<bool> failed{false};
        std::mutex mutex;
        std::string error;

        void run(const int threads, const u64 total, const std::string& label, const std::function<void(int)>& body) {
            std::atomic<int> running{threads};
            std::vector<std::thread> pool;
            const auto t0 = Clock::now();
            for (int t = 0; t < threads; ++t) {
                pool.emplace_back([&, t] {
                    try {
                        body(t);
                    } catch (const std::exception& e) {
                        std::lock_guard lock(mutex);
                        if (!failed.exchange(true)) error = e.what();
                    }
                    --running;
                });
            }
            auto lastPrint = Clock::now();
            while (running.load() > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                if (std::chrono::duration<double>(Clock::now() - lastPrint).count() < 15.0) continue;
                lastPrint = Clock::now();
                const double s = secondsSince(t0);
                const double d = static_cast<double>(done.load());
                const double rate = d / std::max(s, 1e-9);
                std::fprintf(stderr, "[%s] %llu / %llu worlds  %.0f/s  eta %.1f min\n", label.c_str(),
                             static_cast<unsigned long long>(done.load()), static_cast<unsigned long long>(total), rate,
                             rate > 0 ? (static_cast<double>(total) - d) / rate / 60.0 : 0.0);
            }
            for (auto& th: pool) th.join();
            if (failed) throw std::runtime_error(error);
        }

        /// Next [begin, end) batch, or false when everything is taken (or a worker failed).
        bool take(const u64 total, const u64 batch, u64& begin, u64& end) {
            if (failed) return false;
            begin = next.fetch_add(batch);
            if (begin >= total) return false;
            end = std::min(total, begin + batch);
            return true;
        }
    };

    // --------------------------------------------------------------------------------------------- selftest

    struct Checks {
        u64 run = 0, failed = 0;
        std::vector<std::string> messages;

        void check(const bool ok, const std::string& what) {
            ++run;
            if (ok) return;
            ++failed;
            if (messages.size() < 30) messages.push_back(what);
        }
    };

    std::string posStr(const Pos2D p) { return "(" + std::to_string(p.x) + ", " + std::to_string(p.z) + ")"; }

    int runSelftest(const Args& a) {
        const Settings s = makeSettings(a);
        const u64 n = a.seeds ? a.seeds : 2000;
        Checks c;
        const auto t0 = Clock::now();

        // 1) reference positions from unit_tests/test_structures.cpp (placement must match with and without cache)
        struct Ref {
            lce::CONSOLE console;
            LCEVERSION version;
            lce::BIOMESCALE scale;
            i64 seed;
            Pos2D expected;
        };
        const Ref refs[] = {{lce::CONSOLE::WIIU, LCEVERSION::AQUATIC, lce::BIOMESCALE::SMALL, 1, {84, 4}},
                            {lce::CONSOLE::XBOX1, LCEVERSION::ELYTRA, lce::BIOMESCALE::SMALL, 1, {20, -28}},
                            {lce::CONSOLE::XBOX1, LCEVERSION::ELYTRA, lce::BIOMESCALE::LARGE, 10557, {-44, 100}}};
        for (const Ref& r: refs) {
            Generator g(r.console, r.version, r.seed, lce::WORLDSIZE::CLASSIC, r.scale);
            const Pos2D uncached = Placement::Stronghold::getWorldPositions(g)[0];
            g.generateCache(4);
            const Pos2D cached = Placement::Stronghold::getWorldPositions(g)[0];
            RNG rng;
            rng.setSeed(static_cast<u64>(r.seed));
            const Pos2D copy = placeStronghold(g, rng, nullptr);
            const std::string tag = "reference seed " + std::to_string(r.seed) + ": expected " + posStr(r.expected);
            c.check(uncached == r.expected, tag + ", library uncached " + posStr(uncached));
            c.check(cached == r.expected, tag + ", library cached " + posStr(cached));
            c.check(copy == r.expected, tag + ", placeStronghold " + posStr(copy));
        }

        // 2) random worlds: every fast path against the library's own uncached implementation
        WorldSampler sampler(s);
        Generator uncached(s.console, s.version, lce::WORLDSIZE::CLASSIC, s.scale);
        Generator smallUncached(s.console, s.version, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        Generator smallCached(s.console, s.version, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        smallCached.generateCache(4);
        KernelAccumulator kernel(a.eyeY, nullptr);
        std::mt19937_64 pick(12345);
        u64 balancedWorlds = 0, prefilterPassed = 0, retries = 0, transformChecks = 0;

        for (u64 i = 0; i < n; ++i) {
            const u64 sampleIndex = a.firstSample + i;
            WorldSample w = sampler.sample(sampleIndex); // throws if placeStronghold != library (cached)
            const i64 seed = w.seed;
            const std::string tag = "seed " + std::to_string(seed) + ": ";
            if (!s.balancedOnly)
                c.check(seed == sampleWorldSeed(s.runSeed, sampleIndex), tag + "seedFor != sampleWorldSeed");
            uncached.applyWorldSeed(seed);

            const Pos2D libUncached = Placement::Stronghold::getWorldPositions(uncached)[0];
            c.check(libUncached == w.stronghold,
                    tag + "placement cached " + posStr(w.stronghold) + " vs uncached " + posStr(libUncached));
            RNG rng;
            rng.setSeed(static_cast<u64>(seed));
            const Pos2D copyUncached = placeStronghold(uncached, rng, nullptr);
            c.check(copyUncached == libUncached, tag + "placeStronghold uncached " + posStr(copyUncached));
            if (w.trace.attempts > 1) ++retries;

            const Pos2D spawnUncached = uncached.getSpawnBlock();
            c.check(spawnUncached == w.spawn,
                    tag + "spawn cached " + posStr(w.spawn) + " vs uncached " + posStr(spawnUncached));

            const Pos2D off = w.player - w.spawn;
            c.check(std::abs(off.x) <= PLAYER_SPAWN_RADIUS && std::abs(off.z) <= PLAYER_SPAWN_RADIUS,
                    tag + "player offset out of range");

            if (s.balancedCheck) {
                smallUncached.applyWorldSeed(seed);
                smallCached.applyWorldSeed(seed);
                const bool reference = isBalancedFromGenerator(smallUncached);
                const bool fromCache = isBalancedFromCache(Biomes4(smallCached));
                c.check(reference == fromCache, tag + "balanced test: cache vs getBiomeRange differ");
                c.check((w.balanced == 1) == reference, tag + "balanced flag differs from reference");
                c.check(!reference || mayBeBalanced(smallUncached),
                        tag + "mushroom pre-filter rejected a balanced seed");
                if (reference) ++balancedWorlds;
                if (mayBeBalanced(smallUncached)) ++prefilterPassed;
            }

            // biome view against single-cell generation on the uncached generator
            const Biomes4& view = sampler.biomes();
            for (int k = 0; k < 16; ++k) {
                const int x4 = static_cast<int>(pick() % 216) - 108;
                const int z4 = static_cast<int>(pick() % 216) - 108;
                c.check(view.at(x4, z4) == uncached.getBiomeIdAt(4, x4, z4),
                        tag + "biome cache mismatch at 1:4 (" + std::to_string(x4) + ", " + std::to_string(z4) + ")");
            }
            c.check(view.at(108, 0) == biome_t::none && view.at(0, -109) == biome_t::none,
                    "biome view must return none outside the world");

            // distance transform against brute force
            if (i < 50) {
                const gen::Stronghold& sh = sampler.stronghold();
                const Pos2D chunk = w.stronghold.toChunkPos();
                for (int kind = 0; kind < K_COUNT; ++kind) {
                    int lo = 0, hi = -1;
                    if (!kernel.transform(sh, chunk, static_cast<KernelKind>(kind), lo, hi)) continue;
                    for (int k = 0; k < 400; ++k) {
                        const int cx = lo + static_cast<int>(pick() % static_cast<u64>(hi - lo + 1));
                        const int cz = lo + static_cast<int>(pick() % static_cast<u64>(hi - lo + 1));
                        const double fast =
                                kernel.distances()[static_cast<size_t>(cz) * KERNEL_SIDE + static_cast<size_t>(cx)];
                        const long long brute =
                                distanceToStronghold(sh, static_cast<KernelKind>(kind), chunk.x * 16 - KERNEL_HALF + cx,
                                                     chunk.z * 16 - KERNEL_HALF + cz, a.eyeY);
                        const bool none = brute == std::numeric_limits<long long>::max();
                        const bool same = none ? fast >= DT_INF : static_cast<long long>(fast + 0.5) == brute;
                        c.check(same, tag + "distance transform " + KERNEL_NAMES[static_cast<size_t>(kind)] + " fast " +
                                              std::to_string(fast) + " vs brute " + std::to_string(brute));
                        ++transformChecks;
                    }
                }
            }
        }

        std::printf("selftest %s/%s/%s: %llu worlds, %llu checks, %llu failed (%.1fs)\n", a.console.c_str(),
                    a.version.c_str(), a.scale.c_str(), static_cast<unsigned long long>(n),
                    static_cast<unsigned long long>(c.run), static_cast<unsigned long long>(c.failed),
                    secondsSince(t0));
        std::printf("  balanced worlds: %llu (mushroom pre-filter passed %llu), placement retries: %llu, "
                    "transform checks: %llu\n",
                    static_cast<unsigned long long>(balancedWorlds), static_cast<unsigned long long>(prefilterPassed),
                    static_cast<unsigned long long>(retries), static_cast<unsigned long long>(transformChecks));
        for (const auto& m: c.messages) std::printf("  FAIL %s\n", m.c_str());
        return c.failed == 0 ? 0 : 1;
    }

    // ----------------------------------------------------------------------------------------------- dataset

    int runDataset(const Args& a) {
        const Settings s = makeSettings(a);
        const u64 total = a.seeds ? a.seeds : 1000000;
        const u64 kernelWorlds = std::min(a.kernelSeeds, total);
        fs::create_directories(a.out);
        const std::string tag = tagOf(a);
        const std::string csvPath = a.out + "/seeds_" + tag + ".csv";

        std::FILE* csv = std::fopen(csvPath.c_str(), "wb");
        if (csv == nullptr) throw std::runtime_error("cannot write " + csvPath);
        {
            std::string header;
            for (const auto& col: seedCsvColumns()) header += (header.empty() ? "" : ",") + col;
            header += '\n';
            std::fwrite(header.data(), 1, header.size(), csv);
        }

        Pool pool;
        std::mutex csvMutex;
        std::vector<std::unique_ptr<CoverageAccumulator>> coverage(static_cast<size_t>(a.threads));
        auto kernelTotals = std::make_unique<KernelTotals>();
        std::atomic<u64> balancedWorlds{0}, retries{0}, fallbacks{0};
        const u64 BATCH = a.balancedOnly ? 4 : 256; // balanced-only worlds take ~0.1 s each

        std::fprintf(stderr, "dataset %s/%s/%s: %llu worlds (kernels from the first %llu), %d threads -> %s\n",
                     a.console.c_str(), a.version.c_str(), a.scale.c_str(), static_cast<unsigned long long>(total),
                     static_cast<unsigned long long>(kernelWorlds), a.threads, a.out.c_str());
        const auto t0 = Clock::now();

        pool.run(a.threads, total, "dataset " + tag, [&](const int t) {
            WorldSampler sampler(s);
            auto cov = std::make_unique<CoverageAccumulator>();
            std::unique_ptr<KernelAccumulator> ker;
            std::string buffer;
            buffer.reserve(1 << 21);
            auto flush = [&] {
                std::lock_guard lock(csvMutex);
                if (std::fwrite(buffer.data(), 1, buffer.size(), csv) != buffer.size())
                    throw std::runtime_error("write failed: " + csvPath);
                buffer.clear();
            };
            u64 begin = 0, end = 0;
            while (pool.take(total, BATCH, begin, end)) {
                for (u64 i = begin; i < end; ++i) {
                    WorldSample w = sampler.sample(a.firstSample + i);
                    w.footprint = cov->add(sampler.stronghold(), w.stronghold, w.balanced == 1);
                    if (i < kernelWorlds) {
                        if (!ker) ker = std::make_unique<KernelAccumulator>(a.eyeY, kernelTotals.get());
                        ker->add(sampler.stronghold(), w.stronghold.toChunkPos());
                    }
                    if (w.balanced == 1) ++balancedWorlds;
                    if (w.trace.attempts > 1) ++retries;
                    if (w.trace.found == 0) ++fallbacks;
                    appendSeedCsvRow(buffer, w);
                    ++pool.done;
                }
                if (buffer.size() > (1u << 20)) flush();
            }
            if (!buffer.empty()) flush();
            if (ker) ker->flush();
            coverage[static_cast<size_t>(t)] = std::move(cov);
        });
        std::fclose(csv);
        const double seconds = secondsSince(t0);

        // merge
        auto cov = std::make_unique<CoverageAccumulator>();
        for (const auto& c: coverage)
            if (c) cov->merge(*c);
        const KernelTotals* ker = kernelTotals.get();

        const std::vector<size_t> worldShape = {WORLD_SIDE, WORLD_SIDE};
        writeNpy(a.out + "/heatmap_any_" + tag + ".npy", cov->any.data(), worldShape);
        writeNpy(a.out + "/heatmap_any_balanced_" + tag + ".npy", cov->anyBalanced.data(), worldShape);
        writeNpy(a.out + "/heatmap_portal_" + tag + ".npy", cov->portal.data(), worldShape);
        writeNpy(a.out + "/heatmap_start_" + tag + ".npy", cov->start.data(), worldShape);
        const std::vector<u32> cumulative = ker->cumulative();
        json kernelFiles = json::object();
        for (int kind = 0; kind < K_COUNT; ++kind) {
            const std::string name = KERNEL_NAMES[static_cast<size_t>(kind)];
            const std::string file = "kernel_" + name + "_" + tag + ".npy";
            writeNpy(a.out + "/" + file, &cumulative[static_cast<size_t>(kind) * NUM_RADII * KERNEL_CELLS],
                     {static_cast<size_t>(NUM_RADII), KERNEL_SIDE, KERNEL_SIDE});
            kernelFiles[name] = file;
        }

        json info = settingsJson(a, total);
        info["seconds"] = seconds;
        info["worlds_per_second"] = static_cast<double>(total) / std::max(seconds, 1e-9);
        info["balanced_worlds"] = a.balanced ? json(balancedWorlds.load()) : json(nullptr);
        info["placement_retries"] = retries.load();
        info["placement_fallbacks"] = fallbacks.load();
        info["kernel_worlds"] = ker->worlds;
        info["kernel_radii"] = std::vector<int>(KERNEL_RADII.begin(), KERNEL_RADII.end());
        info["kernel_half"] = KERNEL_HALF;
        info["kernel_kinds"] = std::vector<std::string>(KERNEL_NAMES.begin(), KERNEL_NAMES.end());
        info["pieces_outside_kernel_window"] = ker->piecesOutsideWindow;
        info["pieces_clipped_by_world"] = cov->clipped;
        info["files"] = {{"seeds", "seeds_" + tag + ".csv"},
                         {"heatmap_any", "heatmap_any_" + tag + ".npy"},
                         {"heatmap_any_balanced", "heatmap_any_balanced_" + tag + ".npy"},
                         {"heatmap_portal", "heatmap_portal_" + tag + ".npy"},
                         {"heatmap_start", "heatmap_start_" + tag + ".npy"},
                         {"kernels", kernelFiles}};
        writeJson(a.out + "/run_" + tag + ".json", info);

        std::fprintf(stderr, "done: %llu worlds in %.1f min (%.0f/s), balanced %llu, retries %llu, fallbacks %llu\n",
                     static_cast<unsigned long long>(total), seconds / 60.0,
                     static_cast<double>(total) / std::max(seconds, 1e-9),
                     static_cast<unsigned long long>(balancedWorlds.load()),
                     static_cast<unsigned long long>(retries.load()),
                     static_cast<unsigned long long>(fallbacks.load()));
        if (ker->piecesOutsideWindow != 0)
            std::fprintf(stderr, "WARNING: %llu pieces outside the kernel window, kernels near R=100 are truncated\n",
                         static_cast<unsigned long long>(ker->piecesOutsideWindow));
        return 0;
    }

    // ------------------------------------------------------------------------------------------------ oracle

    /// Minimal .npy reader for the kernels written by dataset mode (<u4, C order).
    std::vector<u32> readNpyU32(const std::string& path, std::vector<size_t>& shape) {
        std::ifstream f(path, std::ios::binary);
        if (!f) throw std::runtime_error("cannot read " + path + " (run dataset mode for this scale first)");
        char magic[8];
        f.read(magic, 8);
        unsigned char len[2];
        f.read(reinterpret_cast<char*>(len), 2);
        std::string header(static_cast<size_t>(len[0] | (len[1] << 8)), '\0');
        f.read(header.data(), static_cast<std::streamsize>(header.size()));
        if (header.find("'<u4'") == std::string::npos || header.find("False") == std::string::npos)
            throw std::runtime_error(path + ": expected a C-ordered <u4 array");
        const size_t open = header.find('(', header.find("'shape'"));
        const size_t close = header.find(')', open);
        std::stringstream ss(header.substr(open + 1, close - open - 1));
        shape.clear();
        size_t count = 1;
        for (std::string part; std::getline(ss, part, ',');) {
            if (part.find_first_not_of(' ') == std::string::npos) continue;
            shape.push_back(std::stoull(part));
            count *= shape.back();
        }
        std::vector<u32> data(count);
        f.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(count * sizeof(u32)));
        if (!f) throw std::runtime_error(path + ": truncated");
        return data;
    }

    /// One oracle target: P(the xray/dig at a column sees the stronghold) as a kernel around the chunk origin.
    struct OracleTarget {
        std::string name;
        KernelKind kind = K_ANY_H;
        int radius = 0;
        std::vector<double> k;                    ///< KERNEL_SIDE^2 probabilities
        int bx0 = 0, bx1 = -1, bz0 = 0, bz1 = -1; ///< non-zero box, offsets from the chunk origin
        std::vector<double> sub;                  ///< kernel at offsets that are multiples of 4, inside the box
        int sx0 = 0, sx1 = -1, sz0 = 0, sz1 = -1; ///< sub box in units of 4 blocks

        ND double at(c_int dx, c_int dz) const {
            if (dx < bx0 || dx > bx1 || dz < bz0 || dz > bz1) return 0.0;
            return k[static_cast<size_t>(dz + KERNEL_HALF) * KERNEL_SIDE + static_cast<size_t>(dx + KERNEL_HALF)];
        }
    };

    int floorDiv4(const int v) { return v >= 0 ? v / 4 : -((-v + 3) / 4); }
    int ceilDiv4(const int v) { return -floorDiv4(-v); }

    std::vector<OracleTarget> loadTargets(const Args& a) {
        const std::string kernelTag = a.kernelTag.empty() ? tagOf(a) : a.kernelTag;
        const std::string runJson = a.kernels + "/run_" + kernelTag + ".json";
        std::ifstream jf(runJson);
        if (!jf) throw std::runtime_error("cannot read " + runJson + " (run dataset mode with the same options first)");
        const json info = json::parse(jf);
        const double worlds = info.at("kernel_worlds").get<double>();
        if (worlds <= 0)
            throw std::runtime_error(runJson + " has no kernel worlds; pass --kernel-tag of a run that has them");

        std::vector<OracleTarget> targets;
        std::stringstream list(a.targets);
        for (std::string item; std::getline(list, item, ',');) {
            const size_t colon = item.find(':');
            if (colon == std::string::npos) usage("bad --targets entry " + item);
            OracleTarget t;
            const std::string kindName = item.substr(0, colon);
            t.radius = static_cast<int>(parseU64("--targets", item.substr(colon + 1)));
            int kindIndex = -1;
            for (int kk = 0; kk < K_COUNT; ++kk)
                if (kindName == KERNEL_NAMES[static_cast<size_t>(kk)]) kindIndex = kk;
            if (kindIndex < 0) usage("unknown kernel in --targets: " + kindName);
            t.kind = static_cast<KernelKind>(kindIndex);
            const auto rIt = std::ranges::find(KERNEL_RADII, t.radius);
            if (rIt == KERNEL_RADII.end()) usage("radius " + std::to_string(t.radius) + " is not tabulated");
            const auto r = static_cast<size_t>(rIt - KERNEL_RADII.begin());
            t.name = kindName + "_r" + std::to_string(t.radius);

            std::vector<size_t> shape;
            const auto counts = readNpyU32(a.kernels + "/kernel_" + kindName + "_" + kernelTag + ".npy", shape);
            if (shape != std::vector<size_t>{static_cast<size_t>(NUM_RADII), KERNEL_SIDE, KERNEL_SIDE})
                throw std::runtime_error("kernel file has an unexpected shape");
            t.k.resize(KERNEL_CELLS);
            for (size_t cell = 0; cell < KERNEL_CELLS; ++cell) {
                t.k[cell] = counts[r * KERNEL_CELLS + cell] / worlds;
                if (t.k[cell] <= 0.0) continue;
                c_int dx = static_cast<int>(cell % KERNEL_SIDE) - KERNEL_HALF;
                c_int dz = static_cast<int>(cell / KERNEL_SIDE) - KERNEL_HALF;
                if (t.bx1 < t.bx0) t.bx0 = t.bx1 = dx, t.bz0 = t.bz1 = dz;
                t.bx0 = std::min(t.bx0, dx), t.bx1 = std::max(t.bx1, dx);
                t.bz0 = std::min(t.bz0, dz), t.bz1 = std::max(t.bz1, dz);
            }
            if (t.bx1 < t.bx0) throw std::runtime_error("kernel " + t.name + " is empty");
            t.sx0 = ceilDiv4(t.bx0), t.sx1 = floorDiv4(t.bx1), t.sz0 = ceilDiv4(t.bz0), t.sz1 = floorDiv4(t.bz1);
            for (int sz = t.sz0; sz <= t.sz1; ++sz)
                for (int sx = t.sx0; sx <= t.sx1; ++sx) t.sub.push_back(t.at(4 * sx, 4 * sz));
            targets.push_back(std::move(t));
        }
        return targets;
    }

    /// Start-chunk histogram of one half of the RNG streams.
    struct ChunkHistogram {
        static constexpr int HALF = WORLD_HALF / 16; // chunks [-27, 26]
        static constexpr int SIDE = 2 * HALF;
        std::vector<u32> count = std::vector<u32>(static_cast<size_t>(SIDE) * SIDE, 0);
        std::vector<int> used;
        u64 total = 0;

        void add(const Pos2D chunk) {
            if (chunk.x < -HALF || chunk.x >= HALF || chunk.z < -HALF || chunk.z >= HALF)
                throw std::runtime_error("stronghold chunk outside the world");
            const int i = (chunk.z + HALF) * SIDE + (chunk.x + HALF);
            if (count[static_cast<size_t>(i)]++ == 0) used.push_back(i);
            ++total;
        }
        void clear() {
            for (const int i: used) count[static_cast<size_t>(i)] = 0;
            used.clear();
            total = 0;
        }
        ND Pos2D chunkOf(const int i) const { return {i % SIDE - HALF, i / SIDE - HALF}; }

        /// Expected number of streams whose stronghold the target sees from column (x, z).
        ND double eval(const OracleTarget& t, c_int x, c_int z) const {
            double s = 0.0;
            for (const int i: used) {
                const Pos2D c = chunkOf(i);
                s += count[static_cast<size_t>(i)] * t.at(x - 16 * c.x, z - 16 * c.z);
            }
            return s;
        }
    };

    /// Heat map of a histogram on the 4-block lattice of the world (216 x 216), for the coarse argmax.
    struct CoarseMap {
        static constexpr int SIDE = WORLD_SIDE / 4;
        std::vector<double> v = std::vector<double>(static_cast<size_t>(SIDE) * SIDE, 0.0);

        void build(const ChunkHistogram& h, const OracleTarget& t) {
            std::ranges::fill(v, 0.0);
            c_int w = t.sx1 - t.sx0 + 1;
            for (const int i: h.used) {
                const Pos2D c = h.chunkOf(i);
                const double n = h.count[static_cast<size_t>(i)];
                c_int cx = SIDE / 2 + 4 * c.x, cz = SIDE / 2 + 4 * c.z; // lattice index of the chunk origin
                for (int sz = t.sz0; sz <= t.sz1; ++sz) {
                    c_int z = cz + sz;
                    if (z < 0 || z >= SIDE) continue;
                    const double* row = &t.sub[static_cast<size_t>(sz - t.sz0) * static_cast<size_t>(w)];
                    double* dst = &v[static_cast<size_t>(z) * SIDE];
                    for (int sx = std::max(t.sx0, -cx); sx <= std::min(t.sx1, SIDE - 1 - cx); ++sx)
                        dst[cx + sx] += n * row[sx - t.sx0];
                }
            }
        }
    };

    /// Best column for histogram(s) `hs`: argmax on the 4-block lattice, then refined at 1 block within +-2.
    Pos2D bestColumn(const CoarseMap& coarse, const std::vector<const ChunkHistogram*>& hs, const OracleTarget& t) {
        size_t best = 0;
        for (size_t i = 1; i < coarse.v.size(); ++i)
            if (coarse.v[i] > coarse.v[best]) best = i;
        c_int bx = static_cast<int>(best % CoarseMap::SIDE) * 4 - WORLD_HALF;
        c_int bz = static_cast<int>(best / CoarseMap::SIDE) * 4 - WORLD_HALF;
        Pos2D arg{bx, bz};
        double top = -1.0;
        for (int dz = -2; dz <= 2; ++dz) {
            for (int dx = -2; dx <= 2; ++dx) {
                c_int x = bx + dx, z = bz + dz;
                if (x < -WORLD_HALF || x >= WORLD_HALF || z < -WORLD_HALF || z >= WORLD_HALF) continue;
                double s = 0.0;
                for (const ChunkHistogram* h: hs) s += h->eval(t, x, z);
                if (s > top) top = s, arg = {x, z};
            }
        }
        return arg;
    }

    int runOracle(const Args& a) {
        const Settings s = makeSettings(a);
        const u64 total = a.seeds ? a.seeds : 10000;
        const u64 streams = std::max<u64>(2, a.streams + (a.streams & 1));
        const std::vector<OracleTarget> targets = loadTargets(a);
        fs::create_directories(a.out);
        const std::string tag = tagOf(a);
        const std::string csvPath = a.out + "/oracle_" + tag + ".csv";
        std::FILE* csv = std::fopen(csvPath.c_str(), "wb");
        if (csv == nullptr) throw std::runtime_error("cannot write " + csvPath);
        {
            std::string header = "sample,seed,balanced,sh_x,sh_z,streams,distinct_chunks";
            for (const auto& t: targets)
                header += "," + t.name + "_best_x," + t.name + "_best_z," + t.name + "_p_cross," + t.name +
                          "_p_insample," + t.name + "_hit";
            header += '\n';
            std::fwrite(header.data(), 1, header.size(), csv);
        }

        std::fprintf(stderr, "oracle %s/%s/%s: %llu worlds x %llu streams, %d threads -> %s\n", a.console.c_str(),
                     a.version.c_str(), a.scale.c_str(), static_cast<unsigned long long>(total),
                     static_cast<unsigned long long>(streams), a.threads, csvPath.c_str());
        Pool pool;
        std::mutex csvMutex;
        const auto t0 = Clock::now();
        pool.run(a.threads, total, "oracle " + tag, [&](int) {
            WorldSampler sampler(s);
            ChunkHistogram half[2];
            CoarseMap mapA, mapB, mapAll;
            std::string buffer;
            u64 begin = 0, end = 0;
            while (pool.take(total, 4, begin, end)) {
                for (u64 i = begin; i < end; ++i) {
                    const u64 sampleIndex = a.firstSample + i;
                    const i64 seed = sampler.seedFor(sampleIndex);
                    sampler.loadSeed(seed);
                    const int balanced =
                            s.balancedOnly ? 1 : (s.balancedCheck ? (sampler.isBalanced(seed) ? 1 : 0) : -1);
                    const Generator& g = sampler.generator();
                    const Pos2D actual = Placement::Stronghold::getWorldPositions(g)[0];
                    sampler.generateLayout(seed, actual);

                    half[0].clear();
                    half[1].clear();
                    const u64 streamBase = hash2(s.runSeed ^ ORACLE_SALT, sampleIndex);
                    for (u64 j = 0; j < streams; ++j) {
                        RNG rng;
                        rng.setSeed(hash2(streamBase, j));
                        half[j & 1].add(placeStronghold(g, rng, nullptr).toChunkPos());
                    }
                    size_t distinct = half[0].used.size();
                    for (const int idx: half[1].used)
                        if (half[0].count[static_cast<size_t>(idx)] == 0) ++distinct;

                    char head[160];
                    std::snprintf(head, sizeof(head), "%llu,%lld,%d,%d,%d,%llu,%zu",
                                  static_cast<unsigned long long>(sampleIndex), static_cast<long long>(seed), balanced,
                                  actual.x, actual.z, static_cast<unsigned long long>(streams), distinct);
                    buffer += head;
                    for (const OracleTarget& t: targets) {
                        mapA.build(half[0], t);
                        mapB.build(half[1], t);
                        for (size_t c = 0; c < mapAll.v.size(); ++c) mapAll.v[c] = mapA.v[c] + mapB.v[c];
                        const Pos2D bestA = bestColumn(mapA, {&half[0]}, t);
                        const Pos2D bestB = bestColumn(mapB, {&half[1]}, t);
                        const Pos2D bestAll = bestColumn(mapAll, {&half[0], &half[1]}, t);
                        const double nA = static_cast<double>(half[0].total), nB = static_cast<double>(half[1].total);
                        const double cross =
                                0.5 * (half[1].eval(t, bestA.x, bestA.z) / nB + half[0].eval(t, bestB.x, bestB.z) / nA);
                        const double inSample =
                                (half[0].eval(t, bestAll.x, bestAll.z) + half[1].eval(t, bestAll.x, bestAll.z)) /
                                (nA + nB);
                        const long long d =
                                distanceToStronghold(sampler.stronghold(), t.kind, bestAll.x, bestAll.z, a.eyeY);
                        const int hit = withinRadius(t.kind, d, t.radius) ? 1 : 0;
                        char cell[160];
                        std::snprintf(cell, sizeof(cell), ",%d,%d,%.6f,%.6f,%d", bestAll.x, bestAll.z, cross, inSample,
                                      hit);
                        buffer += cell;
                    }
                    buffer += '\n';
                }
                if (buffer.size() > (1u << 16)) {
                    std::lock_guard lock(csvMutex);
                    std::fwrite(buffer.data(), 1, buffer.size(), csv);
                    buffer.clear();
                }
                pool.done += end - begin;
            }
            std::lock_guard lock(csvMutex);
            std::fwrite(buffer.data(), 1, buffer.size(), csv);
        });
        std::fclose(csv);
        const double seconds = secondsSince(t0);

        json info = settingsJson(a, total);
        info["streams"] = streams;
        info["kernels_from"] = a.kernels + "/run_" + (a.kernelTag.empty() ? tag : a.kernelTag) + ".json";
        info["seconds"] = seconds;
        std::vector<std::string> names;
        for (const auto& t: targets) names.push_back(t.name);
        info["targets"] = names;
        info["files"] = {{"oracle", "oracle_" + tag + ".csv"}};
        writeJson(a.out + "/oracle_run_" + tag + ".json", info);
        std::fprintf(stderr, "done: %llu worlds in %.1f min\n", static_cast<unsigned long long>(total), seconds / 60.0);
        return 0;
    }

} // namespace

int main(const int argc, char** argv) {
    const Args a = parseArgs(argc, argv);
    Placement::Stronghold::setWorldSize(lce::WORLDSIZE::CLASSIC);
    try {
        if (a.mode == "selftest") return runSelftest(a);
        if (a.mode == "dataset") return runDataset(a);
        return runOracle(a);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}
