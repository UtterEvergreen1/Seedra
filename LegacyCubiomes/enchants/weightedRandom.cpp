#include "weightedRandom.hpp"


int WeightedRandom::getTotalWeight(const std::vector<EnchantmentData>& collection) {
    int totalWeight = 0;
    for (const auto& item: collection) { totalWeight += item.obj->rarity->getWeight(); }
    return totalWeight;
}


EnchantmentData WeightedRandom::getRandomItem(RNG& rng, const std::vector<EnchantmentData>& collection,
                                              const int totalWeight) {
    const int weight = rng.nextInt(totalWeight);
    // printf("weight chosen: %d, totalWeight %d\n", weight, totalWeight);
    return getRandomItem(collection, weight);
}


EnchantmentData WeightedRandom::getRandomItem(const std::vector<EnchantmentData>& collection, int weightIn) {
    int count = 0;
    for (const auto it: collection) {
        weightIn -= it.obj->rarity->getWeight();
        if (weightIn < 0) return it;
        count += 1;
    }
    return {};
}


EnchantmentData WeightedRandom::getRandomItem(RNG& rng, const std::vector<EnchantmentData>& collection) {
    return getRandomItem(rng, collection, getTotalWeight(collection));
}
