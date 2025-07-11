#define DOCTEST_CONFIG_IMPLEMENT
#include "include/doctest.h"
#include "common/rng.hpp"
#include <emmintrin.h>
#include <iostream>

inline uint64_t readTSC() {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

inline uint64_t readTSCLocked() {
    _mm_lfence();
    uint64_t tsc = readTSC();
    _mm_lfence();
    return tsc;
}

constexpr u64 SEED = 0x123456789ABCDEF0ULL;
constexpr int MEASURE_N = 10000000;

std::unordered_map<std::string, uint64_t> baseline_cycles = {
        {"rng.nextInt()", 37},
        {"rng.nextInt(8)", 36},
        {"rng.nextInt(10)", 41},
        {"(rng.nextInt(2'000'000'001))", 44},
        {"(rng.nextInt(2'147'483'646))", 41},
        {"rng.nextInt<8>()", 36},
        {"rng.nextInt<9>()", 39},
        {"rng.nextInt(5, 5)", 32},
        {"rng.nextInt(3, 5)", 39},
        {"(rng.nextInt<8, 8>())", 31},
        {"(rng.nextInt<8, 10>())", 39},
        {"rng.nextLong()",   42},
        {"rng.nextLongI()",  42},
        {"rng.nextFloat(5.0f, 4.0f)", 32},
        {"rng.nextFloat(1.0f, 2.0f)", 45},
        {"rng.nextDouble(2.0, 2.0)", 31},
        {"rng.nextDouble(0.1, 0.2)", 52}
};

#include <iomanip> // Required for std::setw

#define MEASURE_LOOP(expr)                                                          \
    do {                                                                            \
        const char* label = #expr;                                                  \
        uint64_t total = 0;                                                         \
        rng.setSeed(SEED);                                                          \
        for (int i = 0; i < MEASURE_N; ++i) {                                       \
            rng.advance();                                                          \
                                                                                    \
            asm volatile("" ::: "memory");  /* Compiler barrier before */           \
            _mm_lfence();                                                           \
            uint64_t start = __rdtsc();                                             \
            _mm_lfence();                                                           \
                                                                                    \
            volatile auto result = expr;                                            \
                                                                                    \
            _mm_lfence();                                                           \
            uint64_t end = __rdtsc();                                               \
            _mm_lfence();                                                           \
            asm volatile("" ::: "memory");  /* Compiler barrier after */            \
                                                                                    \
            total += (end - start);                                                 \
        }                                                                           \
        uint64_t avg = total / MEASURE_N;                                           \
        std::ostringstream out;                                                     \
        out << std::left << std::setw(40) << "[Cycle avg] " + std::string(label);   \
        out << std::right << std::setw(6) << avg << " cycles";                      \
        if (baseline_cycles.count(label)) {                                         \
            uint64_t base = baseline_cycles[label];                                 \
            int64_t delta = static_cast<int64_t>(avg) - static_cast<int64_t>(base); \
            out << "  (baseline: " << std::setw(3) << base << ")  ";                \
            if (delta == 0) {                                                       \
                out << "[=]";                                                       \
            } else if (delta < 0) {                                                 \
                out << "[FASTER ^" << std::setw(3) << -delta << "]";                \
            } else {                                                                \
                out << "[slower v" << std::setw(3) << delta << "]";                 \
            }                                                                       \
        }                                                                           \
        std::cout << out.str() << "\n";                                             \
    } while (0)


TEST_SUITE("RNG Code Path Branch Coverage") {

    /*
    TEST_CASE("yeah") {
        for (int num = 0; num < 8; num++) {
            c_u64 addend = RNG::computeAddend(num);
            c_u64 mult = RNG::computeMultiplier(num);
            std::cout << "Add (" << num << "): " << addend << "\n";
            std::cout << "Mult(" << num << "): " << mult << "\n";
        }
    }
     */




    TEST_CASE("nextInt() returns deterministic values") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt());
        }

        RNG rng(SEED);
        std::vector<i32> expected = {80969665, 651188779, -1616041088, 303921174, -869424907};

        for (i32 val : expected) {
            CHECK((rng.nextInt() == val));
        }
    }

    TEST_CASE("nextInt(n) where n is power of two") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt(8));
        }


        RNG rng(SEED);
        std::vector<i32> expected = {0, 1, 4, 0, 6};

        for (i32 val : expected) {
            CHECK((rng.nextInt(8) == val));
        }

    }


    TEST_CASE("nextInt(n) where n is NOT power of two") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt(10));
        }

        RNG rng(SEED);
        std::vector<i32> expected = {2, 9, 4, 7, 4};

        for (i32 val : expected) {
            CHECK((rng.nextInt(10) == val));
        }
    }

    TEST_CASE("nextInt(2_000_000_001) - triggers modulo retry path") {
        {
            RNG rng(SEED);
            MEASURE_LOOP((rng.nextInt(2'000'000'001)));
        }

        RNG rng(SEED);
        const int highN = 2'000'000'001;

        std::vector<int> results;
        for (int i = 0; i < 5; ++i) {
            results.push_back(rng.nextInt(highN));
        }

        for (int val : results) {
            CHECK((val >= 0));
            CHECK((val < highN));
        }
    }

    TEST_CASE("nextInt(2_147_483_646) - near INT32_MAX") {
        {
            RNG rng(SEED);
            MEASURE_LOOP((rng.nextInt(2'147'483'646)));
        }

        RNG rng(SEED);
        const int almostMax = 2'147'483'646;

        std::vector<int> results;
        for (int i = 0; i < 5; ++i) {
            results.push_back(rng.nextInt(almostMax));
        }

        for (int val : results) {
            CHECK((val >= 0));
            CHECK((val < almostMax));
        }
    }

    TEST_CASE("nextInt<8>() (template, power-of-2)") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt<8>());
        }

        RNG rng(SEED);
        std::vector<i32> expected = {0, 1, 4, 0, 6};

        for (i32 val : expected) {
            CHECK((rng.nextInt<8>() == val));
        }
    }

    TEST_CASE("nextInt<9>() (template, NOT power-of-2)") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt<9>());
        }

        RNG rng(SEED);
        std::vector<i32> expected = {6, 3, 7, 6, 4};

        for (i32 val : expected) {
            CHECK((rng.nextInt<9>() == val));
        }
    }

    TEST_CASE("nextInt(min, max) where min == max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt(5, 5));
        }

        RNG rng(SEED);
        CHECK((rng.nextInt(5, 5) == 5));
    }

    TEST_CASE("nextInt(min, max) where min < max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextInt(3, 5));
        }

        RNG rng(SEED);
        std::vector<i32> expected = {3, 3, 4, 3, 4};

        for (i32 val : expected) {
            CHECK((rng.nextInt(3, 5) == val));
        }
    }

    TEST_CASE("nextInt<8, 8>() where Min == Max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP((rng.nextInt<8, 8>()));
        }

        RNG rng(SEED);
        CHECK((rng.nextInt<8, 8>() == 8));
    }

    TEST_CASE("nextInt<8, 10>() where Min < Max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP((rng.nextInt<8, 10>()));
        }

        RNG rng(SEED);
        std::vector<i32> expected = {8, 8, 9, 8, 9};

        for (i32 val : expected) {
            CHECK((rng.nextInt<8, 10>() == val));
        }
    }

    TEST_CASE("nextLong() deterministic & perf") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextLong());
        }

        RNG rng(SEED);
        std::vector<u64> expected = {
                347762063794264619ULL,
                11505900452061214742ULL,
                14712592529714190370ULL,
                8847702967637743564ULL,
                11225004205901179931ULL,
                14783535502332364181ULL,
                18102993808278747099ULL,
                2351815242975597044ULL,
                5862081076362588043ULL,
                11324524358410614949ULL
        };
        for (u64 v : expected)
            CHECK((rng.nextLong() == v));
    }

    TEST_CASE("nextLongI() deterministic & perf") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextLongI());
        }

        RNG rng(SEED);
        std::vector<i64> expected = {
                347762063794264619LL,
                -6940843621648336874LL,
                -3734151543995361246LL,
                8847702967637743564LL,
                -7221739867808371685LL,
                -3663208571377187435LL,
                -343750265430804517LL,
                2351815242975597044LL,
                5862081076362588043LL,
                -7122219715298936667LL
        };
        for (i64 v : expected)
            CHECK((rng.nextLongI() == v));
    }

    TEST_CASE("nextFloat(min, max) where min >= max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextFloat(5.0f, 4.0f));
        }

        RNG rng(SEED);
        CHECK((rng.nextFloat(5.0f, 4.0f) == doctest::Approx(5.0f)));
    }

    TEST_CASE("nextFloat(min, max) where min < max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextFloat(1.0f, 2.0f));
        }

        RNG rng(SEED);
        std::vector<float> expected = {
                1.0188522338867188f,
                1.1516166925430298f,
                1.6237361431121826f,
                1.0707621574401855f,
                1.7975711822509766f
        };

        for (float val : expected) {
            CHECK((rng.nextFloat(1.0f, 2.0f) == doctest::Approx(val).epsilon(1e-6f)));
        }
    }

    TEST_CASE("nextDouble(min, max) where min >= max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextDouble(2.0, 2.0));
        }

        RNG rng(SEED);
        CHECK((rng.nextDouble(2.0, 2.0) == doctest::Approx(2.0)));
    }

    TEST_CASE("nextDouble(min, max) where min < max") {
        {
            RNG rng(SEED);
            MEASURE_LOOP(rng.nextDouble(0.1, 0.2));
        }

        RNG rng(SEED);
        std::vector<double> expected = {
                0.1018852221244823,
                0.1623736114364299,
                0.1797571234561041,
                0.1479634937842910,
                0.1608508695324741
        };

        for (double val : expected) {
            CHECK((rng.nextDouble(0.1, 0.2) == doctest::Approx(val).epsilon(1e-14)));
        }
    }
}

/*
#include <iostream>
#include <iomanip>
#include "common/rng.hpp"

constexpr u64 SEED = 0x123456789ABCDEF0ULL;
constexpr int N    = 10;                    // how many samples per call

void reset(RNG& rng) { rng.setSeed(SEED); }

void header(const char* h)
{
    std::cout << "\n==== " << h << " ====\n";
    std::cout << std::showpos << std::dec;  // signed ints show ±
}

int main()
{
    RNG rng;

    // -------------------- nextInt() --------------------
    header("nextInt()");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt: " << rng.nextInt() << '\n';

    // -------------------- nextInt(n) power-of-2 --------------------
    header("nextInt(8)");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt(8): " << rng.nextInt(8) << '\n';

    // -------------------- nextInt(n) non-power-of-2 -----------------
    header("nextInt(10)");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt(10): " << rng.nextInt(10) << '\n';

    // -------------------- nextInt<8>() ------------------------------
    header("nextInt<8>()");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt<8>(): " << rng.nextInt<8>() << '\n';

    // -------------------- nextInt<9>() ------------------------------
    header("nextInt<9>()");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt<9>(): " << rng.nextInt<9>() << '\n';

    // -------------------- nextInt(min,max) edge cases ---------------
    header("nextInt(5,5)");
    reset(rng);
    std::cout << "nextInt(5,5): " << rng.nextInt(5, 5) << '\n';

    header("nextInt(3,5)");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt(3,5): " << rng.nextInt(3, 5) << '\n';

    // -------------------- template min/max variants -----------------
    header("nextInt<8,8>()");
    reset(rng);
    std::cout << "nextInt<8,8>(): " << rng.nextInt<8, 8>() << '\n';

    header("nextInt<8,10>()");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextInt<8,10>(): " << rng.nextInt<8, 10>() << '\n';

    // -------------------- nextFloat() special cases -----------------
    std::cout << std::noshowpos << std::fixed << std::setprecision(8);

    header("nextFloat(5,4)  (min >= max)");
    reset(rng);
    std::cout << "nextFloat(5,4): " << rng.nextFloat(5.f, 4.f) << '\n';

    header("nextFloat(1,2)");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextFloat(1,2): " << rng.nextFloat(1.f, 2.f) << '\n';

    // -------------------- nextDouble() special cases ----------------
    std::cout << std::setprecision(16);

    header("nextDouble(2,2) (min >= max)");
    reset(rng);
    std::cout << "nextDouble(2,2): " << rng.nextDouble(2.0, 2.0) << '\n';

    header("nextDouble(0.1,0.2)");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextDouble(0.1,0.2): " << rng.nextDouble(0.1, 0.2) << '\n';

    // =================================================================
    // NEW: nextLong()  &  nextLongI()
    // =================================================================
    std::cout << std::hex << std::showbase;

    header("nextLong()");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextLong:  " << rng.nextLong() << '\n';

    header("nextLongI()");
    reset(rng);
    for (int i = 0; i < N; ++i)
        std::cout << "nextLongI: " << rng.nextLongI() << '\n';

    return 0;
}

 */