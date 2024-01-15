#include "weightedRandom.hpp"


int WeightedRandom::getTotalWeight(const EnchDataVec_t& collection) {
    int totalWeight = 0;
    for (const auto& item: collection) { totalWeight += item.obj->rarity->getWeight(); }
    return totalWeight;
}


EnchantmentData WeightedRandom::getRandomItem(RNG& rng, const EnchDataVec_t& collection, const int totalWeight) {
    const int weight = rng.nextInt(totalWeight);
    return getRandomItem(collection, weight);
}


EnchantmentData WeightedRandom::getRandomItem(const EnchDataVec_t& collection, int weightIn) {
    int count = 0;
    for (const auto it: collection) {
        weightIn -= it.obj->rarity->getWeight();
        if (weightIn < 0) return it;
        count += 1;
    }
    return {};
}


EnchantmentData WeightedRandom::getRandomItem(RNG& rng, const EnchDataVec_t& collection) {
    return getRandomItem(rng, collection, getTotalWeight(collection));
}
