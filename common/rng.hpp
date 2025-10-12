#pragma once

#include <immintrin.h>

#include "lce/processor.hpp"
#include <chrono>


/*
 * Add (0): 0
 * Mult(0): 1
 *
 * Add (1): 11
 * Mult(1): 25214903917
 *
 * Add (2): 277363943098
 * Mult(2): 205749139540585
 *
 * Add (3): 11718085204285
 * Mult(3): 233752471717045
 *
 * Add (4): 49720483695876
 * Mult(4): 55986898099985
 *
 * Add (5): 102626409374399
 * Mult(5): 120950523281469
 *
 * Add (6): 25707281917278
 * Mult(6): 76790647859193
 *
 * Add (7): 25979478236433
 * Mult(7): 61282721086213
 */




// ==========================================================================
///                    C++ implementation of Java Random
// ==========================================================================

class RNG final {
    u64 mySeed = 0;

public:
    RNG() = default;

    explicit RNG(c_u64 seedIn) { this->setSeed(seedIn); }

    bool operator==(const RNG& other) const noexcept { return mySeed == other.mySeed; }

    bool operator==(const u64& rngValue) const noexcept { return mySeed == rngValue; }

    MU ND u64 getSeed() const noexcept { return mySeed; }

    MU void setValue(const u64 value) noexcept { mySeed = value; }

    void setSeed(const u64 value) noexcept { mySeed = (value ^ RNG_MULT_1) & RNG_MASK; }

    /// generate a random seed based on the current system clock (in nanoseconds)
    MU void setRandomSeed() {
        this->setSeed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }


    // ========================================================================
    ///                           Static Helpers
    // ========================================================================

    MU static RNG initializeWithRandomSeed() {
        RNG rng{};
        rng.setRandomSeed();
        return rng;
    }

    MU static RNG ConstructWithoutSetSeed(const u64 seedIn) noexcept {
        RNG rng;
        rng.mySeed = seedIn;
        return rng;
    }

    MU static i64 getRandomWorldSeed() {
        RNG rng = initializeWithRandomSeed();
        return rng.nextLongI();
    }

    MU static RNG getLargeFeatureSeed(c_i64 worldSeed, c_i32 chunkX, c_i32 chunkZ) noexcept {
        RNG rng;
        rng.setSeed(worldSeed);
        const auto l2 = (i64) rng.nextLong();
        const auto l3 = (i64) rng.nextLong();
        c_i64 l4 = (i64) chunkX * l2 ^ (i64) chunkZ * l3 ^ worldSeed;
        rng.setSeed(l4);
        return rng;
    }

    MU static uint64_t getChunkSeed(c_i64 worldSeed, c_i32 chunkX, c_i32 chunkZ) noexcept {
        RNG rng;
        rng.setSeed(worldSeed);
        auto a = (i64) rng.nextLong();
        auto b = (i64) rng.nextLong();
        a = ((a / 2) * 2) + 1;
        b = ((b / 2) * 2) + 1;
        return (chunkX * a + chunkZ * b) ^ worldSeed;
    }


    MU static RNG getPopulationSeed(c_i64 worldSeed, c_i32 chunkX, c_i32 chunkZ) noexcept {
        return RNG(getChunkSeed(worldSeed, chunkX, chunkZ));
    }

    // ========================================================================
    ///                              Advance
    // ========================================================================

    template <u64 amount = 1>
    MU void advance() noexcept {
        constexpr u64 multiplier = computeMultiplier(amount);
        constexpr u64 addend = computeAddend(amount);
        mySeed = (mySeed * multiplier + addend) & RNG_MASK;
    }

    /// Jumps forwards in the random number sequence by simulating 'n' calls to next.
    MU void skipNextN(c_u64 n) {
        u64 m = 1;
        u64 a = 0;
        u64 im = RNG_MULT_1;
        u64 ia = RNG_ADDEND_1;
        for (u64 k = n; k; k >>= 1) {
            if (k & 1) {
                m *= im;
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        mySeed = mySeed * m + a;
        mySeed &= RNG_MASK;
    }

    template<int BITS>
    MU i32 next() noexcept {
        mySeed = (mySeed * RNG_MULT_1 + RNG_ADDEND_1) & RNG_MASK;
        return (i32) ((i64) mySeed >> (48 - BITS));
    }

    // ========================================================================
    ///                               Bool
    // ========================================================================

    // TODO: optimize
    MU bool nextBoolean() noexcept { return next<1>() != 0; }

    // ========================================================================
    ///                               Int
    // ========================================================================

    MU FORCEINLINE i32 nextInt() noexcept {
        mySeed = (mySeed * RNG_MULT_1 + RNG_ADDEND_1) & RNG_MASK;
        return (i32) (mySeed >> 16);

    }

    MU FORCEINLINE i32 nextInt(c_i32 n) noexcept {
        i32 bits, val;

        if ((n & (n - 1)) == 0) {
            c_i32 k = CTZ(n);
            mySeed = (mySeed * RNG_MULT_1 + RNG_ADDEND_1) & RNG_MASK;
            c_i32 result = (i32) ((i64) mySeed >> (48 - k));
            return result;
        }

        do {
            bits = next<31>();
            val = bits % n;
        } while (bits - val + n - 1 < 0);
        return val;
    }

    MU FORCEINLINE i32 nextInt(const size_t n) noexcept {
        return nextInt(static_cast<int>(n));
    }

    template<i32 N>
    MU FORCEINLINE i32 nextInt() noexcept {
        static_assert(N > 0, "N must be positive");

        if constexpr ((N & (N - 1)) == 0) {
            mySeed = (mySeed * RNG_MULT_1 + RNG_ADDEND_1) & RNG_MASK;
            constexpr i32 k = std::countr_zero((u32)N);
            c_i32 result = (i32) (mySeed >> (48 - k));
            return result;
        } else {
            int val, bits;
            do {
                bits = next<31>();
                val = bits % N;
            } while (bits - val + N - 1 < 0);
            return val;
        }
    }

    template<bool legacy = false>
    MU int nextInt(c_i32 theMin, c_i32 theMax) noexcept {
        if constexpr (!legacy) {
            return theMin >= theMax
                           ? theMin
                           : nextInt(theMax - theMin + 1) + theMin;
        } else {
            return nextInt(theMax - theMin + 1) + theMin;
        }
    }


    template<u8 Min, u8 Max, bool legacy = false>
    MU int nextInt() noexcept {
        if constexpr (!legacy && Min >= Max) {
            return Min;
        } else {
            return nextInt<Max - Min + 1>() + Min;
        }
    }

    // ========================================================================
    ///                               Long
    // ========================================================================

    MU u64 nextLong() noexcept {
        u64 seed1 = (mySeed * RNG_MULT_1 + RNG_ADDEND_1);
        u64 seed2 = (mySeed * RNG_MULT_2 + RNG_ADDEND_2);

        mySeed = seed2 & RNG_MASK;

        u64 n1 = (i32) ((seed1 & RNG_MASK) >> 16);
        u64 n2 = (i32) ((seed2 & RNG_MASK) >> 16);

        u64 n = (n1 << 32) + n2;
        return n;
    }

    MU i64 nextLongI() noexcept {
        u64 seed1 = (mySeed * RNG_MULT_1 + RNG_ADDEND_1);
        u64 seed2 = (mySeed * RNG_MULT_2 + RNG_ADDEND_2);

        mySeed = seed2 & RNG_MASK;

        u64 n1 = (i32) ((seed1 & RNG_MASK) >> 16);
        u64 n2 = (i32) ((seed2 & RNG_MASK) >> 16);

        u64 n = (n1 << 32) + n2;
        return (i64)n;
    }

    // ========================================================================
    ///                               Float
    // ========================================================================

    MU FORCEINLINE float nextFloat() noexcept {
        constexpr float INV_2POW24 = 1.0f / 16777216.0f;
        mySeed = (mySeed * RNG_MULT_1 + RNG_ADDEND_1) & RNG_MASK;
        return static_cast<float>(mySeed >> 24) * INV_2POW24;
    }

    MU FORCEINLINE float nextFloat(c_float theMin, c_float theMax) noexcept {
        return theMin >= theMax
                       ? theMin
                       : nextFloat() * (theMax - theMin) + theMin;
    }

    // ========================================================================
    ///                              Double
    // ========================================================================

    MU FORCEINLINE double nextDouble() noexcept {
        u64 seed1 = mySeed * RNG_MULT_1 + RNG_ADDEND_1;
        u64 seed2 = mySeed * RNG_MULT_2 + RNG_ADDEND_2;

        mySeed = seed2 & RNG_MASK;

        u64 n1 = (seed1 & 0x0000'FFFF'FFC0'0000) << 5;
        u64 n2 = (seed2 & 0x0000'FFFF'FFE0'0000) >> 21;
        u64 n = n1 + n2;

        constexpr double INV_2POW53 = 1.0 / 9007199254740992.0;
        return static_cast<double>(n) * INV_2POW53;


        // u64 n1 = (u64) next<26>() << 27;
        // u64 n2 = next<27>();
        // u64 n = n1 + n2;
        // constexpr double INV_2POW53 = 1.0 / 9007199254740992.0;
        // return static_cast<double>(n) * INV_2POW53;
    }

    MU FORCEINLINE double nextDouble(c_double theMin, c_double theMax) noexcept {
        return theMin >= theMax
                       ? theMin
                       : nextDouble() * (theMax - theMin) + theMin;
    }

    // ========================================================================
    ///                              Gaussian
    // ========================================================================

    MU static double nextGaussian() noexcept {
        /*
          double dVar1;
          double dVar2;
          double __x;
          double dVar3;
          double dVar4;
          if (*(char *)(param_1 + 8) != '\0') {
            *(undefined *)(param_1 + 8) = 0;
            instructionSynchronize();
            return *(double *)(param_1 + 0x10);
          }
          dVar4 = 1.0;
          dVar3 = 0.0;
          do {
            do {
              dVar1 = (double)nextDouble__6RandomFv(param_1);
              dVar2 = (dVar1 + dVar1) - dVar4;
              dVar1 = (double)nextDouble__6RandomFv(param_1);
              dVar1 = (dVar1 + dVar1) - dVar4;
              __x = dVar2 * dVar2 + dVar1 * dVar1;
            } while (dVar4 <= __x);
          } while (__x == dVar3);
          dVar3 = log(__x);
          dVar3 = sqrt((dVar3 * -2.0) / __x);
          *(undefined *)(param_1 + 8) = 1;
          *(double *)(param_1 + 0x10) = dVar1 * dVar3;
          instructionSynchronize();
          return dVar2 * dVar3;
     */
        return 0.0;
    }

    // nextGaussianFloat__6RandomFv
    MU double nextGaussianFloat() noexcept {
        const double dVar1 = nextFloat();
        const double dVar2 = nextFloat(); // passes this to function?
        return static_cast<float>(dVar1 - dVar2);
    }

    // nextGaussianInt__6RandomFi
    MU i32 nextGaussianInt(const int param2) noexcept {
        c_i32 iVar1 = nextInt();
        c_i32 iVar2 = nextInt(param2);
        return iVar1 - iVar2;
    }

    // ========================================================================
    ///                             Private
    // ========================================================================


    static constexpr u64 computeMultiplier(const u64 amount) noexcept {
        u64 m = 1;
        u64 im = RNG_MULT_1;
        for (u64 k = amount; k; k >>= 1) {
            if (k & 1) {
                m *= im;
            }
            im *= im;
        }
        return m & RNG_MASK;
    }

    static constexpr u64 computeAddend(const u64 amount) noexcept {
        u64 a = 0;
        u64 im = RNG_MULT_1;
        u64 ia = RNG_ADDEND_1;
        for (u64 k = amount; k; k >>= 1) {
            if (k & 1) {
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        return a & RNG_MASK;
    }

private:

    static constexpr u64 RNG_MASK = (1ULL << 48) - 1;

    static constexpr u64 RNG_MULT_1 = 0x5deece66dULL;
    static constexpr u64 RNG_ADDEND_1 = 0xB;

    static constexpr u64 RNG_MULT_2 = 0x0000'BB20'B460'0A69;
    static constexpr u64 RNG_ADDEND_2 = 0x0000'0040'942D'E6BA;
};


