#pragma once

#include <vector>
#include <stdexcept>

#include "LegacyCubiomes/cubiomes/rng.hpp"
#include "LegacyCubiomes/enchants/enchantmentData.hpp"

class WeightedRandom {
public:

    class Item {
    public:
        int itemWeight{};

        Item() = default;
        explicit Item(int itemWeightIn) : itemWeight(itemWeightIn) {}

    };


    static int getTotalWeight(const std::vector<EnchantmentData>& collection);

    static EnchantmentData getRandomItem(uint64_t *rng, const std::vector<EnchantmentData>& collection, int totalWeight);

    static EnchantmentData getRandomItem(const std::vector<EnchantmentData>& collection, int weightIn);

    static EnchantmentData getRandomItem(uint64_t *rng, const std::vector<EnchantmentData>& collection);

};



