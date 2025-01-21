#pragma once

#include "support/rng.hpp"
#include "enchantmentData.hpp"


class WeightedRandom {
public:
    static int getTotalWeight(const EnchDataVec_t& collection);
    static EnchantmentData getRandomItem(RNG& rng, const EnchDataVec_t& collection, int totalWeight);
    static EnchantmentData getRandomItem(const EnchDataVec_t& collection, int weightIn);
    static EnchantmentData getRandomItem(RNG& rng, const EnchDataVec_t& collection);
};
