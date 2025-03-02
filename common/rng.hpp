#pragma once

#include "lce/processor.hpp"
#include "StringHash.hpp"

#include <iostream>
#include <chrono>

///=============================================================================
///                    C++ implementation of Java Random
///=============================================================================


class RNG {
    u64 seed = 0;

public:
    RNG() = default;
    explicit RNG(c_u64 seedIn) { this->setSeed(seedIn); }
    explicit RNG(const std::string& seedIn) { this->setSeed(StringHash::hash(seedIn)); }

    bool operator==(const RNG& other) const { return seed == other.seed; }

    bool operator==(c_u64& rngValue) const { return seed == rngValue; }

    ND RNG copy() const {
        c_auto rng = RNG(seed);
        return rng;
    }

    static RNG ConstructWithoutSetSeed(c_u64 seedIn) {
        RNG rng;
        rng.seed = seedIn;
        return rng;
    }

    MU static int64_t getRandomWorldSeed() {
        RNG rng = initializeWithRandomSeed();
        return rng.nextLongI();
    }

    static RNG getLargeFeatureSeed(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng;
        rng.setSeed(worldSeed);
        c_auto l2 = (i64) rng.nextLong();
        c_auto l3 = (i64) rng.nextLong();
        c_i64 l4 = (i64) chunkX * l2 ^ (i64) chunkZ * l3 ^ worldSeed;
        rng.setSeed(l4);
        return rng;
    }


    static RNG getPopulationSeed(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng;
        rng.setSeed(worldSeed);
        auto a = (i64) rng.nextLong();
        auto b = (i64) rng.nextLong();
        // a = (i64) (((a / 2) * 2) + 1);
        // b = (i64) (((b / 2) * 2) + 1);
        a = ((a / 2) * 2) + 1;
        b = ((b / 2) * 2) + 1;
        c_i64 decoratorSeed = (chunkX * a + chunkZ * b) ^ worldSeed;
        rng.setSeed(decoratorSeed);
        return rng;
    }

    static RNG initializeWithRandomSeed() {
        RNG rng{};
        rng.setRandomSeed();
        return rng;
    }

    MU void setRandomSeed() {
        //generate a random seed based on the nanoseconds
        this->setSeed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }

    static constexpr uint64_t mask = (1ULL << 48) - 1;

    static constexpr uint64_t computeMultiplier(const uint64_t amount) {
        uint64_t m = 1;
        uint64_t im = 0x5deece66dULL;
        for (uint64_t k = amount; k; k >>= 1) {
            if (k & 1) {
                m *= im;
            }
            im *= im;
        }
        return m & mask;
    }

    static constexpr uint64_t computeAddend(const uint64_t amount) {
        uint64_t a = 0;
        uint64_t im = 0x5deece66dULL;
        uint64_t ia = 0xb;
        for (uint64_t k = amount; k; k >>= 1) {
            if (k & 1) {
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        return a & mask;
    }

    template <uint64_t amount = 1>
    void advance() {
        constexpr uint64_t multiplier = computeMultiplier(amount);
        constexpr uint64_t addend = computeAddend(amount);
        seed = (seed * multiplier + addend) & mask;
    }

    ND MU u64 getSeed() const { return seed; }

    MU void setValue(c_u64 value) { seed = value; }

    void setSeed(c_u64 value) { seed = (value ^ 0x5deece66d) & 0xFFFFFFFFFFFF; }

    int next(c_int bits) {
        advance();
        return (int) ((i64) seed >> (48 - bits));
    }

    bool nextBoolean() { return next(1) != 0; }

    int nextInt() { return next(32); }

    int nextInt(c_int n) {
        int bits, val;
        c_int m = n - 1;

        if ((m & n) == 0) {
            u64 x = n * (u64) next(31);
            return (int) ((i64) x >> 31);
        }

        do {
            bits = next(31);
            val = bits % n;
        } while (bits - val + m < 0);
        return val;
    }

    int nextInt(c_int minimum, c_int maximum) {
        return minimum >= maximum ? minimum : nextInt(maximum - minimum + 1) + minimum;
    }

    int nextIntLegacy(c_int minimum, c_int maximum) { return nextInt(maximum - minimum + 1) + minimum; }

    u64 nextLong() { return ((u64) next(32) << 32) + next(32); }

    i64 nextLongI() { return static_cast<i64>((static_cast<u64>(next(32)) << 32) + next(32)); }

    float nextFloat() { return (float) next(24) / (float) 0x1000000; }

    MU float nextFloat(const float minimum, const float maximum) {
        return minimum >= maximum ? minimum : nextFloat() * (maximum - minimum) + minimum;
    }

    double nextDouble() {
        u64 x = next(26);
        x <<= 27;
        x += next(27);
        return (i64) x / (double) 0x20000000000000;
    }

    MU double nextDouble(c_double minimum, c_double maximum) {
        return minimum >= maximum ? minimum : nextDouble() * (maximum - minimum) + minimum;
    }

    /// Jumps forwards in the random number sequence by simulating 'n' calls to next.
    MU void skipNextN(c_u64 n) {
        u64 m = 1;
        u64 a = 0;
        u64 im = 0x5deece66d;
        u64 ia = 0xb;
        for (u64 k = n; k; k >>= 1) {
            if (k & 1) {
                m *= im;
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        seed = seed * m + a;
        seed &= 0xFFFFFFFFFFFF;
    }

    MU static double nextGaussian() {
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
    MU double nextGaussianFloat() {
        c_double dVar1 = nextFloat();
        c_double dVar2 = nextFloat(); // passes this to function?
        return static_cast<float>(dVar1 - dVar2);
    }


    // nextGaussianInt__6RandomFi
    MU int nextGaussianInt(c_int param2) {
        c_int iVar1 = nextInt();
        c_int iVar2 = nextInt(param2);
        return iVar1 - iVar2;
    }
};
