#pragma once

#include <vector>
#include <stdexcept>

#include "LegacyCubiomes/cubiomes/rng.hpp"


class WeightedRandom {
public:
    class Item {
    public:
        int itemWeight{};

        Item() = default;
        explicit Item(int itemWeightIn) : itemWeight(itemWeightIn) {}

    };

    template <typename T>
    static int getTotalWeight(const std::vector<T>& collection) {
        int totalWeight = 0;
        for (const auto& item : collection) {
            totalWeight += item.itemWeight;
        }
        return totalWeight;
    }

    template <typename T>
    static T getRandomItem(uint64_t *rng, const std::vector<T>& collection, int totalWeight) {
        if (totalWeight <= 0) {
            throw std::invalid_argument("Total weight must be greater than 0");
        }

        int weight = nextInt(rng, totalWeight);
        // printf("weight chosen: %d, totalWeight %d\n", weight, totalWeight);
        return getRandomItem(collection, weight);
    }

    template <typename T>
    static T getRandomItem(const std::vector<T>& collection, int weight) {
        int count = 0;
        for (const auto& item : collection) {
            weight -= item.itemWeight;
            if (weight < 0) {
                return item;
            }
            count += 1;
        }
        return T(); // default-constructed item
    }

    template <typename T>
    static T getRandomItem(uint64_t *rng, const std::vector<T>& collection) {
        return getRandomItem(rng, collection, getTotalWeight(collection));
    }
};




