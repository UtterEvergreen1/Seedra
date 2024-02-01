#pragma once

#include "processor.hpp"
#include <iostream>

///=============================================================================
///                    C++ implementation of Java Random
///=============================================================================


class RNG {
    uint64_t seed = 0;

public:
    RNG() = default;
    explicit RNG(const uint64_t seed) : seed(seed) {}

    bool operator==(const RNG& other) const { return seed == other.seed; }

    bool operator==(const int& rngValue) const { return seed == rngValue; }

    ND RNG copy() const {
        const auto rng = RNG(seed);
        return rng;
    }

    static inline RNG getLargeFeatureSeed(const int64_t worldSeed, const int chunkX, const int chunkZ) {
        RNG rng{};
        rng.setSeed(worldSeed);
        const auto l2 = (int64_t) rng.nextLong();
        const auto l3 = (int64_t) rng.nextLong();
        const int64_t l4 = (int64_t) chunkX * l2 ^ (int64_t) chunkZ * l3 ^ worldSeed;
        rng.setSeed(l4);
        return rng;
    }


    static inline RNG getPopulationSeed(const int64_t worldSeed, const int chunkX, const int chunkZ) {
        RNG rng;
        rng.setSeed(worldSeed);
        auto a = (int64_t) rng.nextLong();
        auto b = (int64_t) rng.nextLong();
        a = (int64_t) (((a / 2) * 2) + 1);
        b = (int64_t) (((b / 2) * 2) + 1);
        const int64_t decoratorSeed = (chunkX * a + chunkZ * b) ^ worldSeed;
        rng.setSeed(decoratorSeed);
        return rng;
    }


    MU void advance() { seed = (seed * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; }
    MU void advance2() { seed = (seed * 0xBB20B4600A69 + 0x40942DE6BA) & 0xFFFFFFFFFFFF; }
    MU void advance4() { seed = (seed * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF; }
    MU void advance8() { seed = (seed * 128954768138017 + 137139456763464) & 0xFFFFFFFFFFFF; }
    MU void advance12() { seed = (seed * 0x199C3838D031 + 0xD4CF89E2CFCC) & 0xFFFFFFFFFFFF; }
    MU void advance109() { seed = (seed * 0xE3DB7EC1825D + 0xF751DEF08DC7) & 0xFFFFFFFFFFFF; }
    MU void advance520() { seed = (seed * 0x53E5A095E721 + 0xCACA74409848) & 0xFFFFFFFFFFFF; }
    MU void advance760() { seed = (seed * 0xE5CFDCCC10E1 + 0x2FC9E05B45B8) & 0xFFFFFFFFFFFF; }
    MU void advance772() { seed = (seed * 0x129FF9FE0B11 + 0x80152440A804) & 0xFFFFFFFFFFFF; }
    MU void advance17292() { seed = (seed * 257489430523441 + 184379205320524) & 0xFFFFFFFFFFFF; }

    ND MU uint64_t getSeed() const { return seed; }

    MU void setValue(const uint64_t value) { seed = value; }

    void setSeed(const uint64_t value) { seed = (value ^ 0x5deece66d) & 0xFFFFFFFFFFFF; }

    int next(const int bits) {
        advance();
        return (int) ((int64_t) seed >> (48 - bits));
    }

    bool nextBoolean() { return next(1) != 0; }

    int nextInt() { return next(32); }

    int nextInt(const int n) {
        int bits, val;
        const int m = n - 1;

        if ((m & n) == 0) {
            uint64_t x = n * (uint64_t) next(31);
            return (int) ((int64_t) x >> 31);
        }

        do {
            bits = next(31);
            val = bits % n;
        } while (bits - val + m < 0);
        return val;
    }

    int nextInt(const int minimum, const int maximum) {
        return minimum >= maximum ? minimum : nextInt(maximum - minimum + 1) + minimum;
    }

    int nextIntLegacy(const int minimum, const int maximum) { return nextInt(maximum - minimum + 1) + minimum; }

    uint64_t nextLong() { return ((uint64_t) next(32) << 32) + next(32); }

    int64_t nextLongI() { return static_cast<int64_t>((static_cast<uint64_t>(next(32)) << 32) + next(32)); }

    float nextFloat() { return (float) next(24) / (float) 0x1000000; }

    MU float nextFloat(float minimum, float maximum) {
        return minimum >= maximum ? minimum : nextFloat() * (maximum - minimum) + minimum;
    }

    double nextDouble() {
        uint64_t x = next(26);
        x <<= 27;
        x += next(27);
        return (int64_t) x / (double) 0x20000000000000;
    }

    MU double nextDouble(const double minimum, const double maximum) {
        return minimum >= maximum ? minimum : nextDouble() * (maximum - minimum) + minimum;
    }

    /// Jumps forwards in the random number sequence by simulating 'n' calls to next.
    MU void skipNextN(const uint64_t n) {
        uint64_t m = 1;
        uint64_t a = 0;
        uint64_t im = 0x5deece66d;
        uint64_t ia = 0xb;
        uint64_t k;
        for (k = n; k; k >>= 1) {
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

    MU double nextGaussian() {
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
        const double dVar1 = nextFloat();
        const double dVar2 = nextFloat(); // passes this to function?
        return static_cast<float>(dVar1 - dVar2);
    }


    // nextGaussianInt__6RandomFi
    MU int nextGaussianInt(const int param2) {
        const int iVar1 = nextInt();
        const int iVar2 = nextInt(param2);
        return iVar1 - iVar2;
    }
};
