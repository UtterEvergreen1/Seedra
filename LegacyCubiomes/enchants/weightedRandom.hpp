#pragma once

#include <vector>
#include <stdexcept>

#include "LegacyCubiomes/utils/rng.hpp"
#include "LegacyCubiomes/enchants/enchantmentData.hpp"

class WeightedRandom {
public:
    static int getTotalWeight(const std::vector<EnchantmentData>& collection);
    static EnchantmentData getRandomItem(RNG& rng, const std::vector<EnchantmentData>& collection, int totalWeight);
    static EnchantmentData getRandomItem(const std::vector<EnchantmentData>& collection, int weightIn);
    static EnchantmentData getRandomItem(RNG& rng, const std::vector<EnchantmentData>& collection);
};
