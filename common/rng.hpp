#pragma once

#include "lce/processor.hpp"
#include <chrono>

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

    void setSeed(const u64 value) noexcept { mySeed = (value ^ RNG_MAGIC) & RNG_MASK; }

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

    MU static RNG getLargeFeatureSeed(const i64 worldSeed, const i32 chunkX, const i32 chunkZ) noexcept {
        RNG rng;
        rng.setSeed(worldSeed);
        const auto l2 = (i64) rng.nextLong();
        const auto l3 = (i64) rng.nextLong();
        const i64 l4 = (i64) chunkX * l2 ^ (i64) chunkZ * l3 ^ worldSeed;
        rng.setSeed(l4);
        return rng;
    }


    MU static RNG getPopulationSeed(const i64 worldSeed, const i32 chunkX, const i32 chunkZ) noexcept {
        RNG rng;
        rng.setSeed(worldSeed);
        auto a = (i64) rng.nextLong();
        auto b = (i64) rng.nextLong();
        a = ((a / 2) * 2) + 1;
        b = ((b / 2) * 2) + 1;
        const i64 decoratorSeed = (chunkX * a + chunkZ * b) ^ worldSeed;
        rng.setSeed(decoratorSeed);
        return rng;
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
        u64 im = RNG_MAGIC;
        u64 ia = RNG_ADDEND;
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
        advance();
        return (i32) ((i64) mySeed >> (48 - BITS));
    }

    // ========================================================================
    ///                               Bool
    // ========================================================================

    MU bool nextBoolean() noexcept { return next<1>() != 0; }

    // ========================================================================
    ///                               Int
    // ========================================================================

    MU __forceinline i32 nextInt() noexcept {
        const i32 result = next<32>();
        return result;
    }

    MU __forceinline i32 nextInt(const i32 n) noexcept {
        i32 bits, val;

        if (((n - 1) & n) == 0) {
            advance();
            const i32 k = __builtin_ctz(n);
            const i32 result = (i32) ((i64) mySeed >> (48 - k));
            return result;
        }

        do {
            bits = next<31>();
            val = bits % n;
        } while (bits - val + n - 1 < 0);
        return val;
    }

    MU __forceinline i32 nextInt(const size_t n) noexcept {
        return nextInt(static_cast<int>(n));
    }

    template<i32 N>
    MU __forceinline i32 nextInt() noexcept {
        static_assert(N > 0, "N must be positive");

        if constexpr ((N & (N - 1)) == 0) {
            advance();
            constexpr i32 k = __builtin_ctz(N);
            c_i32 result = (i32) ((i64) mySeed >> (48 - k));
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
    MU int nextInt(const i32 theMin, const i32 theMax) noexcept {
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

    MU u64 nextLong() noexcept { return ((u64) next<32>() << 32) + next<32>(); }

    MU i64 nextLongI() noexcept { return (i64) (((u64)(next<32>()) << 32) + next<32>()); }

    // ========================================================================
    ///                               Float
    // ========================================================================

    MU float nextFloat() noexcept { return (float) next<24>() / (float) 0x1000000; }

    MU float nextFloat(const float theMin, const float theMax) noexcept {
        return theMin >= theMax ? theMin : nextFloat() * (theMax - theMin) + theMin;
    }

    // ========================================================================
    ///                              Double
    // ========================================================================

    MU double nextDouble() noexcept {
        u64 x = next<26>();
        x <<= 27;
        x += next<27>();
        return (i64) x / (double) 0x20000000000000;
    }

    MU double nextDouble(const double theMin, const double theMax) noexcept {
        return theMin >= theMax ? theMin : nextDouble() * (theMax - theMin) + theMin;
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
        const i32 iVar1 = nextInt();
        const i32 iVar2 = nextInt(param2);
        return iVar1 - iVar2;
    }

    // ========================================================================
    ///                             Private
    // ========================================================================

private:
    static constexpr u64 RNG_MASK = (1ULL << 48) - 1;

    static constexpr u64 RNG_ADDEND = 0xB;

    static constexpr u64 RNG_MAGIC = 0x5deece66dULL;

    static constexpr u64 computeMultiplier(const u64 amount) noexcept {
        u64 m = 1;
        u64 im = RNG_MAGIC;
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
        u64 im = RNG_MAGIC;
        u64 ia = RNG_ADDEND;
        for (u64 k = amount; k; k >>= 1) {
            if (k & 1) {
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        return a & RNG_MASK;
    }
};