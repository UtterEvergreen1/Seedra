#include "weightedRandom.hpp"


int WeightedRandom::getTotalWeight(const std::vector<EnchantmentData>& collection) {
    int totalWeight = 0;
    for (const auto& item : collection) {
        totalWeight += item.obj->rarity->getWeight();
    }
    return totalWeight;
}

EnchantmentData WeightedRandom::getRandomItem(uint64_t *rng, const std::vector<EnchantmentData>& collection, int totalWeight) {
    int weight = nextInt(rng, totalWeight);
    return getRandomItem(collection, weight);
    // printf("weight chosen: %d, totalWeight %d\n", weight, totalWeight);
}

EnchantmentData WeightedRandom::getRandomItem(const std::vector<EnchantmentData>& collection, int weightIn) {
    int count = 0;
    for (auto it : collection) {
        weightIn -= it.obj->rarity->getWeight();
        if (weightIn < 0)
            return it;
        count += 1;
    }
    return {};
}

EnchantmentData WeightedRandom::getRandomItem(uint64_t *rng, const std::vector<EnchantmentData>& collection) {
    return getRandomItem(rng, collection, getTotalWeight(collection));
}
