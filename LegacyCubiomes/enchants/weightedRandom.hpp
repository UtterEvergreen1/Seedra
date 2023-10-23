#pragma once

#include <stdexcept>
#include <vector>

#include "LegacyCubiomes/enchants/enchantmentData.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WeightedRandom {
public:
    static int getTotalWeight(const std::vector<EnchantmentData>& collection);
    static EnchantmentData getRandomItem(RNG& rng, const std::vector<EnchantmentData>& collection, int totalWeight);
    static EnchantmentData getRandomItem(const std::vector<EnchantmentData>& collection, int weightIn);
    static EnchantmentData getRandomItem(RNG& rng, const std::vector<EnchantmentData>& collection);
};
