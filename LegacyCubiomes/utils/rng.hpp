#pragma once

#include "processor.hpp"
#include <iostream>

///=============================================================================
///                    C++ implementation of Java Random
///=============================================================================


class RNG {
private:
    uint64_t seed = 0;

public:
    RNG() : seed(0) {}
    explicit RNG(uint64_t seed) : seed(seed) {}

    bool operator==(const RNG &other) const {
        return seed == other.seed;
    }

    bool operator==(const int &rngValue) const {
        return seed == rngValue;
    }

    ND RNG copy() const {
        RNG rng = RNG(seed);
        return rng;
    }

    static inline RNG getLargeFeatureSeed(int64_t worldSeed, int chunkX, int chunkZ) {
        RNG rng{};
        rng.setSeed(worldSeed);
        auto l2 = (int64_t) rng.nextLong();
        auto l3 = (int64_t) rng.nextLong();
        int64_t l4 = (int64_t) chunkX * l2 ^ (int64_t) chunkZ * l3 ^ worldSeed;
        rng.setSeed(l4);
        return rng;
    }


    static inline RNG getPopulationSeed(int64_t worldSeed, int chunkX, int chunkZ) {
        RNG rng;
        rng.setSeed(worldSeed);
        auto a = (int64_t)rng.nextLong();
        auto b = (int64_t)rng.nextLong();
        a = (int64_t) (((a / 2) * 2) + 1);
        b = (int64_t) (((b / 2) * 2) + 1);
        int64_t decoratorSeed = (chunkX * a + chunkZ * b) ^ worldSeed;
        rng.setSeed(decoratorSeed);
        return rng;
    }


    MU inline void advance() {seed = (seed * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF;}
    MU inline void advance2() {seed = (seed * 0xBB20B4600A69 + 0x40942DE6BA) & 0xFFFFFFFFFFFF;}
    MU inline void advance4() {seed = (seed * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF;}
    MU inline void advance8() {seed = (seed * 128954768138017 + 137139456763464) & 0xFFFFFFFFFFFF;}
    MU inline void advance12() {seed  = (seed * 0x199C3838D031 + 0xD4CF89E2CFCC) & 0xFFFFFFFFFFFF;}
    MU inline void advance109() {seed = (seed * 0xE3DB7EC1825D + 0xF751DEF08DC7) & 0xFFFFFFFFFFFF;}
    MU inline void advance520() {seed = (seed * 0x53E5A095E721 + 0xCACA74409848) & 0xFFFFFFFFFFFF;}
    MU inline void advance760() {seed = (seed * 0xE5CFDCCC10E1 + 0x2FC9E05B45B8) & 0xFFFFFFFFFFFF;}
    MU inline void advance772() {seed = (seed * 0x129FF9FE0B11 + 0x80152440A804) & 0xFFFFFFFFFFFF;}

    ND MU inline uint64_t getSeed() const {
        return seed;
    }

    MU inline void setValue(uint64_t value) {
        seed = value;
    }

    inline void setSeed(uint64_t value) {
        seed = (value ^ 0x5deece66d) & 0xFFFFFFFFFFFF;
    }

    inline int next(const int bits) {
        advance();
        return (int) ((int64_t) seed >> (48 - bits));
    }

    inline bool nextBoolean() {
        return next(1) != 0;
    }

    inline int nextInt() {
        return next(32);
    }

    inline int nextInt(const int n) {
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

    inline int nextInt(int minimum, int maximum) {
        return minimum >= maximum ? minimum : nextInt(maximum - minimum + 1) + minimum;
    }

    inline int nextIntLegacy(int minimum, int maximum) {
        return nextInt(maximum - minimum + 1) + minimum;
    }

    inline uint64_t nextLong() {
        return ((uint64_t) next(32) << 32) + next(32);
    }

    inline float nextFloat() {
        return (float) next(24) / (float) 0x1000000;
    }

    MU inline float nextFloat(float minimum, float maximum) {
        return minimum >= maximum ? minimum : nextFloat() * (maximum - minimum) + minimum;
    }

    inline double nextDouble() {
        uint64_t x = next(26);
        x <<= 27;
        x += next(27);
        return (int64_t) x / (double) 0x20000000000000;
    }

    MU inline double nextDouble(double minimum, double maximum) {
        return minimum >= maximum ? minimum : nextDouble() * (maximum - minimum) + minimum;
    }

    /// Jumps forwards in the random number sequence by simulating 'n' calls to next.
    MU inline void skipNextN(uint64_t n) {
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
};

