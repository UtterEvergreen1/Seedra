#pragma once

#include <cstdint>
#include <vector>

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/constants.hpp"


inline int getBiomeGroup(const int biomeID) {
    switch (biomeID) {
        case 12:
        case 13:
        case 26:
        case 30:
        case 31:
        case 140:
        case 158:
            return 0;
        case 3:
        case 5:
        case 9:
        case 19:
        case 20:
        case 25:
        case 32:
        case 33:
        case 34:
        case 131:
        case 133:
        case 160:
        case 161:
        case 162:
            return 1;
        case 1:
        case 4:
        case 16:
        case 18:
        case 27:
        case 28:
        case 29:
        case 129:
        case 132:
        case 155:
        case 156:
        case 157:
            return 2;
        case 6:
        case 21:
        case 22:
        case 23:
        case 134:
        case 149:
        case 151:
            return 3;
        case 2:
        case 17:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 130:
        case 163:
        case 164:
        case 165:
        case 166:
        case 167:
            return 4;
        case 0:
        case 10:
        case 24:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
            return 5;
        case 7:
        case 11:
            return 6;
        case 14:
        case 15:
            return 7;
        default:
            return 8;
    }
}

inline std::vector<float> getFracs(const int* biomes) {
    constexpr int biomesSize = 40000; // size of biomes
    std::vector fracs(8, 0.0F);
    int index = 0;
    while (index < biomesSize) {
        const int biomeGroup = getBiomeGroup(
                biomes[index]); // getting the biome group it is in (take the biomeID and assign it a catagory)
        fracs[biomeGroup]++;
        index++;
    }
    for (int div = 0; div < 8; div++) { fracs[div] /= biomesSize; }
    return fracs;
}

inline bool getIsMatch(const int* biomes) {
    const std::vector<float> floatFrac = getFracs(biomes);
    if (floatFrac[7] < 0.001F) { return false; }
    for (int i = 0; i < 5; i++) {
        if (floatFrac[i] < 0.01F) { return false; }
    }
    if (0.25F < floatFrac[5]) { return false; }
    if (0.1F < floatFrac[6]) { return false; }
    return true;
}

inline int64_t findBalancedSeed(const Generator* g, RNG rng) {
    const uint64_t timeStart = getSeconds();
    while (true) {
        const int64_t seed = rng.nextLongI();
        int* biomes = g->getBiomeRange(4, -100, -100, 200, 200);
        const bool isMatch = getIsMatch(biomes);
        free(biomes);
        if (isMatch) return seed;
        if (10 < getSeconds() - timeStart) return -1;
    }
}
