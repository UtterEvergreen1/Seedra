#pragma once
using i64 = int64_t;
using u64 = uint64_t;

constexpr u64 MASK_48 = (1ULL << 48) - 1;
constexpr u64 MASK_32 = (1ULL << 32) - 1;
constexpr u64 MASK_16 = (1ULL << 16) - 1;

struct LCG {
    u64 multiplier, addend;

    // Fast forward LCG by `steps`
    constexpr LCG combine(int steps) const {
        u64 mul = multiplier;
        u64 add = addend;
        u64 mul_n = 1;
        u64 add_n = 0;
        for (int i = steps; i > 0; i >>= 1) {
            if (i & 1) {
                mul_n = (mul_n * mul) & MASK_48;
                add_n = (add_n * mul + add) & MASK_48;
            }
            add = (mul + 1) * add & MASK_48;
            mul = (mul * mul) & MASK_48;
        }
        return {mul_n, add_n};
    }
};

struct JAVA_LCG {
    static constexpr u64 MULTIPLIER = 0x5DEECE66DULL;
    static constexpr u64 ADDEND = 0xBULL;

    static constexpr LCG base = LCG(MULTIPLIER, ADDEND);
    static constexpr LCG java2 = base.combine(2);
    static constexpr LCG java4 = base.combine(4);
    static constexpr u64 M2 = java2.multiplier;
    static constexpr u64 A2 = java2.addend;
    static constexpr u64 M4 = java4.multiplier;
    static constexpr u64 A4 = java4.addend;
};

class PopulationReverser {
    // Modular inverse mod 2^16
    static __device__ u64 modInverse(u64 a, u64 mod = 1ULL << 16);

    static __device__ i64 getPartialAddend(i64 partialSeed, int x, int z, int bits);

    static __device__ void addWorldSeed(
        i64 firstAddend,
        int multTrailingZeroes,
        i64 firstMultInv,
        i64 c,
        int x,
        int z,
        i64 chunkSeed,
        i64* out, int& cnt
    );

public:
    static __device__ int getSeedsFromChunkSeed(u64 chunkSeed, int x, int z, i64* out);
};
