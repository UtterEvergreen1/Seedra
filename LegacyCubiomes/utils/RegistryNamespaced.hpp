#pragma once
#include <vector>

template<typename T>
class RegistryNamespaced {
public:
    RegistryNamespaced() {}
    inline void registerValue(T* value) {
        this->allValues.emplace_back(value);
    }

    inline T* operator[](int index) const {
        return sortedRegistry[index];
    }

    inline const std::vector<T*>& getRegistry() const {
        return sortedRegistry;
    }

    /**
     * Orders the values from allValues into sortedRegistry for accurate enchantment order for each console and version
     * @param lookupValues the look up values obtained from the enchantment order table
     */
    void orderValues(const std::vector<int>& lookupValues) {
        sortedRegistry.resize(lookupValues.size());
        for (int sortedIndex : lookupValues) {
            sortedRegistry.at(sortedIndex) = allValues[lookupValues.at(sortedIndex)];
        }
    }

    /**
     * Returns the size of current registry
     * @return size of current registry
     */
    inline int size() const {
        return this->sortedRegistry.size();
    }

    /// Deallocates all the stored values
    inline void clear() {
        for(const T* value : allValues){
            delete value;
        }
        allValues.clear();
        sortedRegistry.clear();
    }
private:
    std::vector<T*> allValues;
    std::vector<T*> sortedRegistry;
};

