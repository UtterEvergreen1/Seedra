#include "PopulationReverser.hpp"
#include "common/rng.hpp"

u64 PopulationReverser::modInverse(u64 a, u64 mod) {
    i64 t = 0, newt = 1;
    i64 r = mod, newr = a;
    while (newr != 0) {
        i64 quotient = r / newr;
        t = std::exchange(newt, t - quotient * newt);
        r = std::exchange(newr, r - quotient * newr);
    }
    return (t < 0) ? (t + mod) : t;
}

i64 PopulationReverser::getPartialAddend(i64 partialSeed, int x, int z, int bits) {
    i64 mask = (1ULL << bits) - 1;
    i64 partial = partialSeed ^ JAVA_LCG::MULTIPLIER;
    i64 a = ((JAVA_LCG::M2 * (partial & mask) + JAVA_LCG::A2) & MASK_48) >> 16;
    a = (a / 2) * 2 + 1;
    i64 b = ((JAVA_LCG::M4 * (partial & mask) + JAVA_LCG::A4) & MASK_48) >> 16;
    b = (b / 2) * 2 + 1;
    return x * a + z * b;
}


void PopulationReverser::addWorldSeed(i64 firstAddend, int multTrailingZeroes, i64 firstMultInv, i64 c, int x,
                                      int z, i64 chunkSeed, std::vector<i64> &worldSeeds) {
    i64 bottom32BitsChunkSeed = chunkSeed & MASK_32;

    if (__builtin_ctzll(firstAddend) >= multTrailingZeroes) {
        i64 b = (((firstMultInv * firstAddend) >> multTrailingZeroes) ^ (JAVA_LCG::MULTIPLIER >> 16)) & ((1ULL << (16 - multTrailingZeroes)) - 1);

        if (multTrailingZeroes != 0) {
            i64 smallMask = (1ULL << multTrailingZeroes) - 1;
            i64 smallMultInverse = firstMultInv & smallMask;

            i64 target = (((b ^ (bottom32BitsChunkSeed >> 16)) & smallMask)
                          - ((getPartialAddend((b << 16) + c, x, z, 32 - multTrailingZeroes) >> 16)) ) & smallMask;

            b += (((target * smallMultInverse) ^ (JAVA_LCG::MULTIPLIER >> (32 - multTrailingZeroes))) & smallMask) << (16 - multTrailingZeroes);
        }

        i64 bottom32BitsSeed = (b << 16) + c;
        i64 target2 = (bottom32BitsSeed ^ bottom32BitsChunkSeed) >> 16;

        i64 secondAddend = (getPartialAddend(bottom32BitsSeed, x, z, 32) >> 16) & MASK_16;
        i64 topBits = (((firstMultInv * (target2 - secondAddend)) >> multTrailingZeroes) ^ (JAVA_LCG::MULTIPLIER >> 32)) & ((1ULL << (16 - multTrailingZeroes)) - 1);

        for (; topBits < (1ULL << 16); topBits += (1ULL << (16 - multTrailingZeroes))) {
            i64 candidateSeed = (topBits << 32) + bottom32BitsSeed;
            if ((RNG::getChunkSeed(candidateSeed, x, z) & MASK_48) == (chunkSeed & MASK_48)) {
                worldSeeds.push_back(candidateSeed);
            }
        }
    }
}

std::vector<i64> PopulationReverser::getSeedsFromChunkSeed(u64 chunkSeed, int x, int z) {
    std::vector<i64> results;

    if (x == 0 && z == 0) {
        results.push_back(chunkSeed);
        return results;
    }

    u64 f = chunkSeed & MASK_16;
    u64 firstMultiplier = (JAVA_LCG::M2 * x + JAVA_LCG::M4 * z) & MASK_16;
    int trailingZeroes = __builtin_ctzll(firstMultiplier);
    u64 inv = modInverse(firstMultiplier >> trailingZeroes);
    int xCount = __builtin_ctzll(x);
    int zCount = __builtin_ctzll(z);
    int totalCount = __builtin_ctzll(x | z);

    std::unordered_set<int> possibleRoundingOffsets;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            possibleRoundingOffsets.insert(i * x + j * z);

    if (chunkSeed == 456268685) {
        printf ("chunkSeed = %llu, x = %d, z = %d\n firstM = %llu, tz = %d, inv = %llu\n"
                "xCount = %d, zCount = %d, total = %d\n",
                chunkSeed, x, z, firstMultiplier, trailingZeroes, inv, xCount, zCount, totalCount);
        // print possibleRoundingOffsets
        for (int offset : possibleRoundingOffsets) {
            printf("Offset: %d\n", offset);
        }
    }

    // Build the correct range for c
    u64 cStart = 0;
    if (xCount == zCount)
        cStart = chunkSeed & ((1ULL << (xCount + 1)) - 1);
    else
        cStart = (chunkSeed & ((1ULL << (totalCount + 1)) - 1)) ^ (1ULL << totalCount);

    for (u64 c = cStart; c < (1ULL << 16); c += (1ULL << (totalCount + 1))) {
        u64 target = (c ^ f) & MASK_16;
        u64 partial = (c ^ JAVA_LCG::MULTIPLIER) & MASK_16;

        i64 a16 = (JAVA_LCG::M2 * partial + JAVA_LCG::A2) >> 16;
        i64 b16 = (JAVA_LCG::M4 * partial + JAVA_LCG::A4) >> 16;

        i64 magic = x * a16 + z * b16;

        for (int offset: possibleRoundingOffsets) {
            u64 adjusted = (magic + offset) & MASK_16;
            i64 firstAddend = target - adjusted;

            addWorldSeed(
                firstAddend,
                trailingZeroes,
                inv,
                c,
                x,
                z,
                chunkSeed,
                results
            );
        }
    }

    if (chunkSeed == 456268685) {
        // Print the results for debugging
        printf("Results for chunkSeed = %llu, x = %d, z = %d:\n", chunkSeed, x, z);
        for (i64 seed : results) {
            printf("Seed: %lld\n", seed);
        }
    }

    return results;
}
