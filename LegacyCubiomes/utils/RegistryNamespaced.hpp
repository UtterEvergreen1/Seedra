#pragma once

#include <vector>


template<typename T>
class RegistryNamespaced {
    std::vector<T*> allValues;
    std::vector<T*> sortedRegistry;

public:
    RegistryNamespaced() = default;

    void registerValue(T* value) { allValues.emplace_back(value); }

    T* operator[](int index) const { return sortedRegistry[index]; }

    const std::vector<T*>& getRegistry() const { return sortedRegistry; }

    /**
     * Orders the values from allValues into sortedRegistry for accurate enchantment order for each console and version
     * @param lookupValues the look up values obtained from the enchantment order table
     */
    void orderValues(const std::vector<int>& lookupValues) {
        size_t lookupSize = lookupValues.size();
        sortedRegistry.resize(lookupSize);
        for (size_t i = 0; i < lookupSize; ++i) {
            int sortedIndex = lookupValues[i];
            sortedRegistry[i] = allValues[sortedIndex];
        }
    }

    /**
     * Returns the size of current registry
     * @return size of current registry
     */
    ND int size() const { return sortedRegistry.size(); }

    /// Deallocates all the stored values
    void clear() {
        for (const T* value: allValues) { delete value; }
        allValues.clear();
        sortedRegistry.clear();
    }
};
