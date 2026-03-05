#include "include/doctest.h"
#include "common/rng.hpp"

TEST_SUITE("RNG") {

    TEST_CASE("setSeed produces deterministic state") {
        RNG rng1(12345ULL);
        RNG rng2(12345ULL);
        CHECK(rng1 == rng2);
    }

    TEST_CASE("identical seeds produce identical sequences") {
        RNG rng1(1234567890ULL);
        RNG rng2(1234567890ULL);

        for (int i = 0; i < 1000; ++i) {
            CHECK(rng1.nextLong() == rng2.nextLong());
        }
    }

    TEST_CASE("different seeds produce different sequences") {
        RNG rng1(111ULL);
        RNG rng2(222ULL);

        bool allSame = true;
        for (int i = 0; i < 100; ++i) {
            if (rng1.nextLong() != rng2.nextLong()) {
                allSame = false;
                break;
            }
        }
        CHECK_FALSE(allSame);
    }

    TEST_CASE("nextInt(n) stays in range") {
        RNG rng(42ULL);
        for (int i = 0; i < 10000; ++i) {
            int val = rng.nextInt(100);
            CHECK(val >= 0);
            CHECK(val < 100);
        }
    }

    TEST_CASE("nextInt power-of-two bound") {
        RNG rng(99ULL);
        for (int i = 0; i < 10000; ++i) {
            int val = rng.nextInt(16); // power of 2
            CHECK(val >= 0);
            CHECK(val < 16);
        }
    }

    TEST_CASE("nextFloat is in [0, 1)") {
        RNG rng(77ULL);
        for (int i = 0; i < 10000; ++i) {
            float f = rng.nextFloat();
            CHECK(f >= 0.0f);
            CHECK(f < 1.0f);
        }
    }

    TEST_CASE("nextDouble is in [0, 1)") {
        RNG rng(88ULL);
        for (int i = 0; i < 10000; ++i) {
            double d = rng.nextDouble();
            CHECK(d >= 0.0);
            CHECK(d < 1.0);
        }
    }

    TEST_CASE("nextBoolean produces both true and false") {
        RNG rng(55ULL);
        bool seenTrue = false, seenFalse = false;
        for (int i = 0; i < 1000; ++i) {
            if (rng.nextBoolean()) seenTrue = true;
            else seenFalse = true;
            if (seenTrue && seenFalse) break;
        }
        CHECK(seenTrue);
        CHECK(seenFalse);
    }

    TEST_CASE("nextInt(min, max) stays in range") {
        RNG rng(33ULL);
        for (int i = 0; i < 10000; ++i) {
            int val = rng.nextInt(10, 20);
            CHECK(val >= 10);
            CHECK(val <= 20);
        }
    }

    TEST_CASE("nextFloat(min, max) stays in range") {
        RNG rng(44ULL);
        for (int i = 0; i < 10000; ++i) {
            float f = rng.nextFloat(5.0f, 10.0f);
            CHECK(f >= 5.0f);
            CHECK(f <= 10.0f);
        }
    }

    TEST_CASE("advance<N> matches N calls to next<32>") {
        RNG rng1(100ULL);
        RNG rng2(100ULL);

        // Advance rng1 by 5 calls manually
        for (int i = 0; i < 5; ++i) {
            rng1.next<32>();
        }

        // Advance rng2 by skip
        rng2.skipNextN(5);

        CHECK(rng1.getSeed() == rng2.getSeed());
    }

    TEST_CASE("skipNextN(0) does not change state") {
        RNG rng(200ULL);
        u64 before = rng.getSeed();
        rng.skipNextN(0);
        CHECK(rng.getSeed() == before);
    }

    TEST_CASE("getChunkSeed is deterministic") {
        u64 s1 = RNG::getChunkSeed(12345, 3, 7);
        u64 s2 = RNG::getChunkSeed(12345, 3, 7);
        CHECK(s1 == s2);
    }

    TEST_CASE("getChunkSeed differs for different chunks") {
        u64 s1 = RNG::getChunkSeed(12345, 0, 0);
        u64 s2 = RNG::getChunkSeed(12345, 1, 0);
        u64 s3 = RNG::getChunkSeed(12345, 0, 1);
        CHECK(s1 != s2);
        CHECK(s1 != s3);
    }

    TEST_CASE("ConstructWithoutSetSeed preserves raw value") {
        RNG rng = RNG::ConstructWithoutSetSeed(0xDEAD'BEEF'CAFE);
        CHECK(rng.getSeed() == 0xDEAD'BEEF'CAFE);
    }

    TEST_CASE("template nextInt<N> matches runtime nextInt(N)") {
        RNG rng1(500ULL);
        RNG rng2(500ULL);

        for (int i = 0; i < 100; ++i) {
            CHECK(rng1.nextInt<7>() == rng2.nextInt(7));
        }
    }

    // ----------------------------------------------------------------
    //  getLargeFeatureSeed
    // ----------------------------------------------------------------
    TEST_CASE("getLargeFeatureSeed is deterministic") {
        RNG r1 = RNG::getLargeFeatureSeed(12345LL, 3, 7);
        RNG r2 = RNG::getLargeFeatureSeed(12345LL, 3, 7);
        CHECK(r1 == r2);
    }

    TEST_CASE("getLargeFeatureSeed differs for different world seeds") {
        RNG r1 = RNG::getLargeFeatureSeed(11111LL, 0, 0);
        RNG r2 = RNG::getLargeFeatureSeed(22222LL, 0, 0);
        CHECK_FALSE(r1 == r2);
    }

    TEST_CASE("getLargeFeatureSeed differs for different chunk positions") {
        RNG r1 = RNG::getLargeFeatureSeed(99999LL, 0, 0);
        RNG r2 = RNG::getLargeFeatureSeed(99999LL, 1, 0);
        RNG r3 = RNG::getLargeFeatureSeed(99999LL, 0, 1);
        // Very unlikely to all match
        CHECK_FALSE(r1 == r2);
        CHECK_FALSE(r1 == r3);
    }

    // ----------------------------------------------------------------
    //  getPopulationSeed
    // ----------------------------------------------------------------
    TEST_CASE("getPopulationSeed is deterministic") {
        RNG r1 = RNG::getPopulationSeed(777LL, 5, 5);
        RNG r2 = RNG::getPopulationSeed(777LL, 5, 5);
        CHECK(r1 == r2);
    }

    TEST_CASE("getPopulationSeed produces same RNG as constructed from getChunkSeed") {
        constexpr i64 worldSeed = 314159265LL;
        constexpr i32 cx = 3, cz = -2;
        u64 cs = RNG::getChunkSeed(worldSeed, cx, cz);
        RNG expected(cs);
        RNG actual = RNG::getPopulationSeed(worldSeed, cx, cz);
        CHECK(expected == actual);
    }

    // ----------------------------------------------------------------
    //  advance<N> template vs skipNextN
    // ----------------------------------------------------------------
    TEST_CASE("advance<3> matches skipNextN(3)") {
        RNG r1(42ULL);
        RNG r2(42ULL);
        r1.advance<3>();
        r2.skipNextN(3);
        CHECK(r1.getSeed() == r2.getSeed());
    }

    TEST_CASE("advance<1> matches one next<32>") {
        RNG r1(99ULL);
        RNG r2(99ULL);
        r1.next<32>();
        r2.advance<1>();
        CHECK(r1.getSeed() == r2.getSeed());
    }

    // ----------------------------------------------------------------
    //  nextInt(min, max)
    // ----------------------------------------------------------------
    TEST_CASE("nextInt(min, max) when min > max returns min") {
        RNG rng(1ULL);
        // Non-legacy path: if theMin >= theMax, returns theMin
        int val = rng.nextInt(10, 5);
        CHECK(val == 10);
    }

    TEST_CASE("nextInt(min, max) with equal bounds returns that value") {
        RNG rng(2ULL);
        int val = rng.nextInt(7, 7);
        CHECK(val == 7);
    }

    // ----------------------------------------------------------------
    //  nextDouble(min, max)
    // ----------------------------------------------------------------
    TEST_CASE("nextDouble(min, max) stays in range") {
        RNG rng(55ULL);
        for (int i = 0; i < 10000; ++i) {
            double d = rng.nextDouble(1.0, 2.0);
            CHECK(d >= 1.0);
            CHECK(d <= 2.0);
        }
    }

    TEST_CASE("nextDouble(min, max) when min >= max returns min") {
        RNG rng(10ULL);
        CHECK(rng.nextDouble(5.0, 5.0) == doctest::Approx(5.0));
        CHECK(rng.nextDouble(5.0, 4.0) == doctest::Approx(5.0));
    }

    // ----------------------------------------------------------------
    //  nextLong reproducibility
    // ----------------------------------------------------------------
    TEST_CASE("nextLong matches between two identical-seed RNGs") {
        RNG r1(123456789ULL);
        RNG r2(123456789ULL);
        for (int i = 0; i < 200; ++i) {
            CHECK(r1.nextLong() == r2.nextLong());
        }
    }

    // ----------------------------------------------------------------
    //  setValue / getSeed round-trip
    // ----------------------------------------------------------------
    TEST_CASE("setValue stores raw value without hashing") {
        RNG rng(0ULL);
        rng.setValue(0xCAFE'BABE'1234'5678ULL);
        CHECK(rng.getSeed() == 0xCAFE'BABE'1234'5678ULL);
    }

    // ----------------------------------------------------------------
    //  template nextInt<Min, Max>
    // ----------------------------------------------------------------
    TEST_CASE("template nextInt<0, 9> stays in [0, 9]") {
        RNG rng(77ULL);
        for (int i = 0; i < 1000; ++i) {
            int v = rng.nextInt<0, 9>();
            CHECK(v >= 0);
            CHECK(v <= 9);
        }
    }

    TEST_CASE("template nextInt<Min, Max> when Min >= Max returns Min") {
        RNG rng(1ULL);
        int v = rng.nextInt<5, 5>();
        CHECK(v == 5);
    }
}

