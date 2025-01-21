#include "weightedRandom.hpp"


int WeightedRandom::getTotalWeight(const EnchDataVec_t& collection) {
    int totalWeight = 0;
    for (c_auto& item: collection) { totalWeight += item.obj->rarity; }
    return totalWeight;
}


EnchantmentData WeightedRandom::getRandomItem(RNG& rng, const EnchDataVec_t& collection, c_int totalWeight) {
    c_int weight = rng.nextInt(totalWeight);
    return getRandomItem(collection, weight);
}


EnchantmentData WeightedRandom::getRandomItem(const EnchDataVec_t& collection, int weightIn) {
    int count = 0;
    for (c_auto& it: collection) {
        weightIn -= it.obj->rarity;
        if (weightIn < 0) return it;
        count += 1;
    }
    return {};
}


EnchantmentData WeightedRandom::getRandomItem(RNG& rng, const EnchDataVec_t& collection) {
    return getRandomItem(rng, collection, getTotalWeight(collection));
}
