#pragma once

#include <unordered_map>
#include <vector>

#include "lce/processor.hpp"
#include "enchantment.hpp"


class Enchantment;


class EnchantmentRegistry {
    std::vector<Enchantment*> allValues;
    std::vector<Enchantment*> sortedRegistry;

public:
    EnchantmentRegistry() = default;

    Enchantment* operator[](const int index) const { return sortedRegistry[index]; }

    void registerValue(Enchantment* value) { allValues.emplace_back(value); }

    ND const std::vector<Enchantment*>& getRegistry() const { return sortedRegistry; }

    /**
     * Orders the values from allValues into sortedRegistry for accurate ordering
     * @param lookupValues the look up values obtained from the order of items
     */
    MU void orderValues(const std::vector<int>& lookupValues) {
        const size_t lookupSize = lookupValues.size();
        sortedRegistry.resize(lookupSize);
        for (size_t i = 0; i < lookupSize; ++i) {
            const int sortedIndex = lookupValues[i];
            sortedRegistry[i] = allValues[sortedIndex];
        }
    }

    /**
     * Returns the size of current registry
     * @return size of current registry
     */
    ND size_t size() const { return sortedRegistry.size(); }

    /// Deallocates all the stored values
    void clear();
};
