#pragma once

#include "LegacyCubiomes/enchants/enchantmentData.hpp"
#include "LegacyCubiomes/utils/rng.hpp"


class WeightedRandom {
public:
    static int getTotalWeight(const EnchDataVec_t& collection);
    static EnchantmentData getRandomItem(RNG& rng, const EnchDataVec_t& collection, int totalWeight);
    static EnchantmentData getRandomItem(const EnchDataVec_t& collection, int weightIn);
    static EnchantmentData getRandomItem(RNG& rng, const EnchDataVec_t& collection);
};
