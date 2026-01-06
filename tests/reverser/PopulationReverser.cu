// PopulationReverser_device.cu
#include "PopulationReverser.cuh"

#include <cstdio>

#include "rng.cuh"

__device__ u64 PopulationReverser::modInverse(u64 a, u64 mod) {
    i64 t = 0, newt = 1;
    i64 r = mod, newr = a;
    while (newr) {
        i64 q = r / newr;
        t = (t - q * newt);
        r = (r - q * newr);
        // swap
        i64 tmp = newt;
        newt = t;
        t = tmp;
        tmp = newr;
        newr = r;
        r = tmp;
    }
    return (t < 0) ? t + mod : t;
}

__device__ i64 PopulationReverser::getPartialAddend(i64 partialSeed, int x, int z, int bits) {
    u64 mask = ((u64) 1 << bits) - 1;
    u64 partial = ((u64) partialSeed ^ JAVA_LCG::MULTIPLIER) & MASK_48;
    i64 a = ((JAVA_LCG::M2 * (partial & mask) + JAVA_LCG::A2) & MASK_48) >> 16;
    i64 b = ((JAVA_LCG::M4 * (partial & mask) + JAVA_LCG::A4) & MASK_48) >> 16;
    a = (a / 2) * 2 + 1;
    b = (b / 2) * 2 + 1;
    return x * a + z * b;
}

__device__ void PopulationReverser::addWorldSeed(i64 firstAddend,
                                                 int multTrailingZeroes,
                                                 i64 inv,
                                                 i64 c,
                                                 int x, int z,
                                                 i64 chunkSeed,
                                                 i64 *out, int &cnt) {
    u64 bottom32 = chunkSeed & MASK_32;
    if (__ffs(firstAddend) - 1 >= multTrailingZeroes) {
        i64 b = (((inv * firstAddend) >> multTrailingZeroes)
                 ^ (JAVA_LCG::MULTIPLIER >> 16)) & ((1ULL << (16 - multTrailingZeroes)) - 1);
        if (multTrailingZeroes) {
            i64 mask = (1ULL << multTrailingZeroes) - 1;
            i64 smallInv = inv & mask;
            i64 target = (((b ^ (bottom32 >> 16)) & mask)
                          - ((getPartialAddend((b << 16) + c, x, z, 32 - multTrailingZeroes) >> 16))) & mask;
            b += (((target * smallInv) ^ (JAVA_LCG::MULTIPLIER >> (32 - multTrailingZeroes))) & mask)
                    << (16 - multTrailingZeroes);
        }
        i64 bottom = (b << 16) + c;
        i64 target2 = (bottom ^ (bottom32)) >> 16;
        i64 secondAdd = (getPartialAddend(bottom, x, z, 32) >> 16) & ((1ULL << 16) - 1);
        i64 topBits = (((inv * (target2 - secondAdd)) >> multTrailingZeroes)
                       ^ (JAVA_LCG::MULTIPLIER >> 32)) & ((1ULL << (16 - multTrailingZeroes)) - 1);
        for (; topBits < (1ULL << 16); topBits += (1ULL << (16 - multTrailingZeroes))) {
            u64 cand = (topBits << 32) + bottom;
            if ((RNG::getChunkSeed(cand, x, z) & MASK_48) == (chunkSeed & MASK_48)) {
                out[cnt++] = cand;
            }
        }
    }
}

__device__ int PopulationReverser::getSeedsFromChunkSeed(u64 chunkSeed, int x, int z, i64 *out) {
    int cnt = 0;
    if (x == 0 && z == 0) {
        out[cnt++] = chunkSeed;
        return cnt;
    }
    u64 f = chunkSeed & MASK_16;
    u64 firstM = (JAVA_LCG::M2 * x + JAVA_LCG::M4 * z) & MASK_16;
    int tz = __ffs(firstM) - 1; // ctz
    u64 inv = modInverse(firstM >> tz, 1ULL << 16);
    int xCount = __ffs(x) - 1; // ctz
    int zCount = __ffs(z) - 1; // ctz
    int total = __ffs(x | z) - 1; // ctz

    // prepare 9 offsets: i*x + j*z
    int offsets[9];
    int idx = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // only insert if unique
            bool unique = true;
            int value = i * x + j * z;
            for (int k = 0; k < idx; k++) {
                if (offsets[k] == value) {
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            offsets[idx++] = value;
        }
    }

    u64 cStart = ((xCount == zCount)
                      ? (chunkSeed & ((1ULL << (xCount + 1)) - 1))
                      : ((chunkSeed & ((1ULL << (total + 1)) - 1)) ^ (1ULL << total)));
    for (u64 c = cStart; c < (1ULL << 16); c += (1ULL << (total + 1))) {
        u64 target = (c ^ f) & MASK_16;
        u64 partial = (c ^ JAVA_LCG::MULTIPLIER) & MASK_16;

        i64 a16 = (JAVA_LCG::M2 * partial + JAVA_LCG::A2) >> 16;
        i64 b16 = (JAVA_LCG::M4 * partial + JAVA_LCG::A4) >> 16;

        i64 magic = x * a16 + z * b16;
        for (int offIdx = 0; offIdx < 9; offIdx++) {
            int off = offsets[offIdx];
            if (off == 0 && offIdx != 0) continue; // skip zero offset when not at index 0
            i64 firstAdd = target - ((magic + off) & MASK_16);
            addWorldSeed(firstAdd, tz, inv, c, x, z, chunkSeed, out, cnt);
        }
    }
    return cnt;
}
