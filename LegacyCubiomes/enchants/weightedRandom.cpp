#include "weightedRandom.hpp"


int WeightedRandom::getTotalWeight(const std::vector<EnchantmentData>& collection) {
    int totalWeight = 0;
    for (const auto& item : collection) {
        totalWeight += item.obj->rarity->getWeight();
    }
    // std::cout << "\ntotal weight: " << totalWeight << std::endl;
    return totalWeight;
}

EnchantmentData WeightedRandom::getRandomItem(uint64_t *rng, const std::vector<EnchantmentData>& collection, int totalWeight) {
    if (totalWeight <= 0) {
        throw std::invalid_argument("Total weight must be greater than 0");
    }

    int weight = nextInt(rng, totalWeight);
    std::cout << "weight " << weight << std::endl;
    std::cout << "total weight " << totalWeight << std::endl;
    // printf("weight chosen: %d, totalWeight %d\n", weight, totalWeight);
    return getRandomItem(collection, weight);
}

EnchantmentData WeightedRandom::getRandomItem(const std::vector<EnchantmentData>& collection, int weightIn) {
    int count = 0;
    for (auto it : collection) {
        weightIn -= it.obj->rarity->getWeight();
        if (weightIn < 0) {
            return it;
        }
        count += 1;
    }
    return {}; // default-constructed item
}

EnchantmentData WeightedRandom::getRandomItem(uint64_t *rng, const std::vector<EnchantmentData>& collection) {
    return getRandomItem(rng, collection, getTotalWeight(collection));
}
