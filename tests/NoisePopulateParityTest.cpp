// Compile with the same FP mode you ship (no fast-math / no FMA):
// MSVC: /std:c++20 /O2 /fp:precise /fp:except- /EHsc
// Clang/GCC: -std=c++20 -O2 -fno-fast-math -ffp-contract=off

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <bit>
#include <limits>
#include <memory>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "terrain/noise/NoiseGenerator.hpp"   // NoiseGeneratorImproved, RNG, perlinInit
#include "lce/enums.hpp"                      // lce::CONSOLE
#include "terrain/generator.hpp"              // Generator

// ---------- Config (no CLI) ----------
static constexpr double   TOTAL_BUDGET_MS   = 10000.0;    // ~10s total
static constexpr double   PER_SUITE_BUDGET  = TOTAL_BUDGET_MS / 2.0; // ~5s per console
static constexpr double   EDGE_FRACTION     = 0.35;       // ~35% of per-suite time
static constexpr double   RANDOM_FRACTION   = 0.55;       // ~55% of per-suite time (rest is fixed)
static constexpr uint64_t GLOBAL_SEED       = 0xBADC0FFEE0DDF00Dull;
static constexpr int      PRINT_FIRST_K     = 4;          // per failing case
static constexpr int      PRINT_MAX_CASES   = 24;         // per suite

// ---------- Helpers ----------
static inline const char* consoleName(lce::CONSOLE c) {
    switch (c) { case lce::CONSOLE::WIIU: return "WIIU"; default: return "NON_WIIU"; }
}

// PLEASE EDIT if your Generator has a different ctor.
static std::unique_ptr<Generator> createTestGenerator(lce::CONSOLE console, std::int64_t worldSeed) {
    return std::make_unique<Generator>(console, LCEVERSION::ELYTRA, worldSeed,
                                       lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
}

template <typename T>
static inline std::uint64_t bits_of(T x) {
    static_assert(sizeof(T) == sizeof(std::uint64_t));
    return std::bit_cast<std::uint64_t>(x);
}

static inline void print_mismatch(std::size_t idx, double a, double b) {
    std::printf("  mismatch at index %zu:\n"
                "    A = %.17g  (0x%016llX)\n"
                "    B = %.17g  (0x%016llX)\n",
                idx,
                a, (unsigned long long)bits_of(a),
                b, (unsigned long long)bits_of(b));
}

static inline void init_noise(NoiseGeneratorImproved& n, std::uint64_t permSeed) {
    RNG rng; rng.setSeed(permSeed); perlinInit(&n, rng);
}

using PopulateFn = void (NoiseGeneratorImproved::*)(
        const Generator*,
        std::vector<double>&,
        double,double,double,
        int,int,int,
        double,double,double,
        double
);

enum class Prefill { Zeros, Constant, PRandPerElement };

static inline void apply_prefill(std::vector<double>& v, Prefill mode, std::uint64_t seed, double c=0.0) {
    switch (mode) {
        case Prefill::Zeros:    std::fill(v.begin(), v.end(), 0.0); break;
        case Prefill::Constant: std::fill(v.begin(), v.end(), c);   break;
        case Prefill::PRandPerElement: {
            std::mt19937_64 r(seed);
            for (auto& x : v) {
                std::uint64_t u = r();
                u = (u & ((1ull<<52)-1ull)) | (1022ull<<52); // ~0.5..1.0 magnitude
                double d = std::bit_cast<double>(u);
                if (r() & 1) d = -d;
                x = d;
            }
            break;
        }
    }
}

using Clock = std::chrono::steady_clock;
static inline double ms_since(const Clock::time_point& t0) {
    return std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
}

// ---------- Core runner ----------
static int g_printed_fail_cases = 0;

static bool run_case_prepared(const char* tag,
                              const Generator* g,
                              NoiseGeneratorImproved& A,
                              NoiseGeneratorImproved& B,
                              PopulateFn fnA,
                              PopulateFn fnB,
                              // args
                              double xOff, double yOff, double zOff,
                              int xSize, int ySize, int zSize,
                              double xScale, double yScale, double zScale,
                              double noiseScale,
                              Prefill prefillMode, std::uint64_t preSeed, double preConst,
                              int print_first_k)
{
    const std::size_t N = (std::size_t)xSize * (std::size_t)ySize * (std::size_t)zSize;
    std::vector<double> outA(N), outB(N);
    apply_prefill(outA, prefillMode, preSeed, preConst);
    apply_prefill(outB, prefillMode, preSeed, preConst);

    (A.*fnA)(g, outA, xOff, yOff, zOff, xSize, ySize, zSize, xScale, yScale, zScale, noiseScale);
    (B.*fnB)(g, outB, xOff, yOff, zOff, xSize, ySize, zSize, xScale, yScale, zScale, noiseScale);

    if (std::memcmp(outA.data(), outB.data(), N * sizeof(double)) != 0) {
        if (g_printed_fail_cases < PRINT_MAX_CASES) {
            std::printf("FAIL [%s]: sizes=(%d,%d,%d) scales=(%.17g,%.17g,%.17g) ns=%.17g offs=(%.17g,%.17g,%.17g)\n",
                        tag, xSize,ySize,zSize, xScale,yScale,zScale, noiseScale, xOff,yOff,zOff);
            int shown = 0;
            for (std::size_t i=0;i<N && shown<print_first_k;++i) {
                if (bits_of(outA[i]) != bits_of(outB[i])) { print_mismatch(i, outA[i], outB[i]); ++shown; }
            }
            ++g_printed_fail_cases;
        }
        return false;
    }
    return true;
}

static bool run_case_3prefills(const char* tag,
                               const Generator* g,
                               std::uint64_t permSeed,
                               PopulateFn fnA,
                               PopulateFn fnB,
                               // args
                               double xOff, double yOff, double zOff,
                               int xSize, int ySize, int zSize,
                               double xScale, double yScale, double zScale,
                               double noiseScale,
                               int print_first_k)
{
    NoiseGeneratorImproved A{}, B{};
    init_noise(A, permSeed);
    init_noise(B, permSeed);

    bool ok = true;
    ok &= run_case_prepared(tag, g, A, B, fnA, fnB,
                            xOff,yOff,zOff,xSize,ySize,zSize,xScale,yScale,zScale,noiseScale,
                            Prefill::Zeros, 0x123456789ABCDEF0ull, 0.0, print_first_k);
    ok &= run_case_prepared(tag, g, A, B, fnA, fnB,
                            xOff,yOff,zOff,xSize,ySize,zSize,xScale,yScale,zScale,noiseScale,
                            Prefill::Constant, 0, 0.25, print_first_k);
    ok &= run_case_prepared(tag, g, A, B, fnA, fnB,
                            xOff,yOff,zOff,xSize,ySize,zSize,xScale,yScale,zScale,noiseScale,
                            Prefill::PRandPerElement, 0xBADC0FFEE0DDF00Dull, 0.0, print_first_k);
    return ok;
}

// ---------- Fixed shapes (always run) ----------
static int run_fixed_game_shapes(const Generator* g, PopulateFn fnA, PopulateFn fnB)
{
    int fails = 0;
    const std::uint64_t seeds[] = {
            0ull, 1ull, 12345ull, 0xDEADBEEFCAFEBABEull, 0xFFFFFFFFFFFFFFFFull, 0x0000000100000001ull
    };

    for (auto s: seeds) {
        if (g) {
            fails += !run_case_3prefills("2D-200", g, s, fnA, fnB,
                                         13.0, 10.0, -7.0,
                                         5, 1, 5,
                                         200.0, 1.0, 200.0,
                                         1.0, PRINT_FIRST_K);
            fails += !run_case_3prefills("2D-200-ns0.5", g, s, fnA, fnB,
                                         -101.25, 10.0, 99.75,
                                         5, 1, 5,
                                         200.0, 1.0, 200.0,
                                         0.5, PRINT_FIRST_K);
            fails += !run_case_3prefills("2D-mixed-scales", g, s, fnA, fnB,
                                         1.0, 10.0, -1.0,
                                         9, 1, 7,
                                         1e-3, 1.0, 5000.0,
                                         3.0, PRINT_FIRST_K);
        }

        fails += !run_case_3prefills("3D-main", g, s, fnA, fnB,
                                     17.0, 64.0, -29.0,
                                     5, 33, 5,
                                     8.55515, 4.277575, 8.55515,
                                     1.0, PRINT_FIRST_K);
        fails += !run_case_3prefills("3D-main-25", g, s, fnA, fnB,
                                     17.0, 64.0, -29.0,
                                     5, 25, 5,
                                     8.55515, 4.277575, 8.55515,
                                     1.0, PRINT_FIRST_K);
        fails += !run_case_3prefills("3D-main-41", g, s, fnA, fnB,
                                     17.0, 64.0, -29.0,
                                     5, 41, 5,
                                     8.55515, 4.277575, 8.55515,
                                     1.0, PRINT_FIRST_K);

        fails += !run_case_3prefills("3D-limits", g, s, fnA, fnB,
                                     -256.0, 32.0, 512.0,
                                     5, 33, 5,
                                     684.412, 684.412, 684.412,
                                     1.0, PRINT_FIRST_K);
        fails += !run_case_3prefills("3D-limits-ns0.125", g, s, fnA, fnB,
                                     0.0, 0.0, 0.0,
                                     5, 33, 5,
                                     684.412, 684.412, 684.412,
                                     0.125, PRINT_FIRST_K);
        fails += !run_case_3prefills("3D-limits-big", g, s, fnA, fnB,
                                     1e6, 1e6, -1e6,
                                     3, 33, 3,
                                     1e6, 1e6, 1e6,
                                     1.0, PRINT_FIRST_K);
        fails += !run_case_3prefills("3D-limits-small", g, s, fnA, fnB,
                                     -100.0, 0.0, 100.0,
                                     7, 17, 9,
                                     1e-6, 1e-6, 1e-6,
                                     1.0, PRINT_FIRST_K);
    }
    return fails;
}

// ---------- Edge sampler (budgeted) ----------
static double heavy_tail(std::mt19937_64& r) {
    std::uniform_real_distribution<double> U01(0.0,1.0);
    double u = U01(r);
    if (u < 0.33) return std::ldexp(1.0, -(int)(r()%40 + 10)); // tiny
    if (u < 0.66) return std::ldexp(1.0,  (int)(r()%26));      // large
    return std::uniform_real_distribution<double>(1e-6, 1e6)(r);
}

static int run_edge_samples_budgeted(const Clock::time_point& t0, double per_suite_budget_ms,
                                     const Generator* g, PopulateFn fnA, PopulateFn fnB)
{
    int fails = 0;
    std::mt19937_64 rng(GLOBAL_SEED ^ 0xEDDFEC);
    const double eps = std::nextafter(1.0, 2.0) - 1.0;
    const int ints[] = {-2,-1,0,1,2,1024,-2048};
    const double bases[] = {0.0,-0.0,0.5,-0.5,1000.0,-1000.0};
    const int sizes2[][3] = {{5,1,5},{9,1,9}};
    const int sizes3[][3] = {{5,33,5},{9,17,9},{9,41,9}};
    const std::uint64_t permSeeds[] = {0ull,1ull,0xDEADBEEFCAFEBABEull};

    const double end_ms = per_suite_budget_ms * (1.0 - RANDOM_FRACTION); // run edges until this point
    while (ms_since(t0) < end_ms) {
        const bool is2D = (rng() & 3ull) == 0ull; // ~25% 2D
        int xS,yS,zS;
        if (is2D && g) {
            auto s = sizes2[rng()% (int)(sizeof(sizes2)/sizeof(*sizes2))];
            xS=s[0]; yS=1; zS=s[2];
        } else {
            auto s = sizes3[rng()% (int)(sizeof(sizes3)/sizeof(*sizes3))];
            xS=s[0]; yS=s[1]; zS=s[2];
        }

        double scX = heavy_tail(rng);
        double scY = is2D ? 1.0 : heavy_tail(rng);
        double scZ = heavy_tail(rng);

        double baseX = bases[rng()% (int)(sizeof(bases)/sizeof(*bases))];
        double baseZ = bases[rng()% (int)(sizeof(bases)/sizeof(*bases))];
        int k = ints[rng()% (int)(sizeof(ints)/sizeof(*ints))];

        const double xOff_eq = (double)k - baseX;
        const double zOff_eq = (double)k - baseZ;
        std::uint64_t perm = permSeeds[rng()% (int)(sizeof(permSeeds)/sizeof(*permSeeds))];

        fails += !run_case_3prefills(is2D ? "EDGE-2D" : "EDGE-3D", g, perm, fnA, fnB,
                                     xOff_eq, 10.0, zOff_eq,
                                     xS,yS,zS, scX,scY,scZ, 1.0, PRINT_FIRST_K);
        if (ms_since(t0) >= end_ms) break;

        fails += !run_case_3prefills("EDGE-ulp-", g, perm, fnA, fnB,
                                     xOff_eq - eps, 10.0, zOff_eq - eps,
                                     xS,yS,zS, scX,scY,scZ, 1.0, PRINT_FIRST_K);
        if (ms_since(t0) >= end_ms) break;

        fails += !run_case_3prefills("EDGE-ulp+", g, perm, fnA, fnB,
                                     xOff_eq + eps, 10.0, zOff_eq + eps,
                                     xS,yS,zS, scX,scY,scZ, 1.0, PRINT_FIRST_K);
    }
    return fails;
}

// ---------- Random sweep (fills remaining time) ----------
static int run_random_sweep_budgeted(const Clock::time_point& t0, double per_suite_budget_ms,
                                     const Generator* g, PopulateFn fnA, PopulateFn fnB)
{
    int fails = 0;
    std::mt19937_64 rng(GLOBAL_SEED);
    std::uniform_int_distribution<int> uOdd5(0,5);
    std::uniform_int_distribution<int> uY(0,4);
    std::uniform_real_distribution<double> uOff(-1e9, 1e9);
    std::uniform_real_distribution<double> uNS(1e-6, 8.0);

    const double end_ms = per_suite_budget_ms; // fill remaining time
    while (ms_since(t0) < end_ms) {
        const std::uint64_t permSeed = rng();
        const bool do2D = ((rng() & 7u) == 0u); // ~12.5% 2D
        const bool negNS = (rng() & 1u) && ((permSeed & 0x11u) == 0);

        int xS = 1 + uOdd5(rng)*4 + 1;
        int yS = do2D ? 1 : (1 + uY(rng)*8 + 1);
        int zS = 1 + uOdd5(rng)*4 + 1;

        double xScale = heavy_tail(rng);
        double yScale = do2D ? 1.0 : heavy_tail(rng);
        double zScale = heavy_tail(rng);

        double xOff = uOff(rng);
        double yOff = do2D ? 10.0 : uOff(rng);
        double zOff = uOff(rng);

        double noiseScale = uNS(rng);
        if (negNS) noiseScale = -noiseScale;

        fails += !run_case_3prefills("RND", g, permSeed, fnA, fnB,
                                     xOff,yOff,zOff,
                                     xS,yS,zS,
                                     xScale,yScale,zScale,
                                     noiseScale,
                                     PRINT_FIRST_K);
    }
    return fails;
}

// ---------- main ----------
int main() {
    std::puts("Noise populate parity test (old vs new, ~10s budget)");
    PopulateFn fnOld = &NoiseGeneratorImproved::populateNoiseArray;
    PopulateFn fnNew = &NoiseGeneratorImproved::populateNoiseArrayNew;

    int totalFails = 0;

    auto run_suite = [&](const char* suiteName, const Generator* g) {
        g_printed_fail_cases = 0;
        auto t0 = Clock::now();
        int fails = 0;

        // Always run canonical game-like shapes first
        fails += run_fixed_game_shapes(g, fnOld, fnNew);

        // Budgeted phases
        const double fixed_ms = ms_since(t0);
        const double edge_target_ms   = std::max(0.0, PER_SUITE_BUDGET * EDGE_FRACTION);
        const double random_target_ms = std::max(0.0, PER_SUITE_BUDGET); // fill to end

        // Edges until ~35% of per-suite budget
        if (fixed_ms < PER_SUITE_BUDGET)
            fails += run_edge_samples_budgeted(t0, std::min(PER_SUITE_BUDGET * (EDGE_FRACTION + 0.0001), PER_SUITE_BUDGET), g, fnOld, fnNew);

        // Random until per-suite end
        if (ms_since(t0) < PER_SUITE_BUDGET)
            fails += run_random_sweep_budgeted(t0, PER_SUITE_BUDGET, g, fnOld, fnNew);

        double ms = ms_since(t0);
        std::printf("[%s] fails=%d  time=%.1f ms (budget=%.0f ms)\n",
                    suiteName, fails, ms, PER_SUITE_BUDGET);
        totalFails += fails;
    };

    auto g_nonwiiu = createTestGenerator(lce::CONSOLE::XBOX360, 123456789ull);
    run_suite(consoleName(lce::CONSOLE::XBOX360), g_nonwiiu.get());

    auto g_wiiu = createTestGenerator(lce::CONSOLE::WIIU, 987654321ull);
    run_suite(consoleName(lce::CONSOLE::WIIU), g_wiiu.get());

    if (totalFails == 0) {
        std::puts("ALL GOOD: no mismatches detected within the ~10s budget.");
        return 0;
    } else {
        std::puts("FAIL: mismatches detected (see logs above).");
        return 1;
    }
}
