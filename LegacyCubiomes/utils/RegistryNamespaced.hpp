#pragma once

#include <vector>


template<typename T>
class RegistryNamespaced {
private:
    std::vector<T*> allValues;
    std::vector<T*> sortedRegistry;

public:
    RegistryNamespaced() = default;

    inline void registerValue(T* value) { allValues.emplace_back(value); }

    inline T* operator[](int index) const { return sortedRegistry[index]; }

    inline const std::vector<T*>& getRegistry() const { return sortedRegistry; }

    /**
     * Orders the values from allValues into sortedRegistry for accurate enchantment order for each console and version
     * @param lookupValues the look up values obtained from the enchantment order table
     */
    void orderValues(const std::vector<int>& lookupValues) {
        int sortedIndex;
        int lookupSize = lookupValues.size();
        sortedRegistry.resize(lookupSize);
        for (int i = 0; i < lookupSize; ++i) {
            sortedIndex = lookupValues[i];
            sortedRegistry[i] = allValues[sortedIndex];
        }
    }

    /**
     * Returns the size of current registry
     * @return size of current registry
     */
    ND inline int size() const { return sortedRegistry.size(); }

    /// Deallocates all the stored values
    inline void clear() {
        for (const T* value: allValues) { delete value; }
        allValues.clear();
        sortedRegistry.clear();
    }
};
