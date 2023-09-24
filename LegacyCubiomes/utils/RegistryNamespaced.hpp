#pragma once
#include <boost/unordered_map.hpp>
#include "ResourceLocation.hpp"

template<typename T>
class RegistryNamespaced {
public:
    struct FakePointerHasher {
        std::size_t operator()(const int& fakePointer) const {
            return fakePointer + (fakePointer >> 3);
        }
    };
    RegistryNamespaced() {}
    void registerMapping(int id, const ResourceLocation& key, T* value, int fakePointer) {
        //this->idLookup.emplace(value, id);
        //this->registryObjects.emplace(key, value);
        this->pointerLookup.emplace(fakePointer, value);
        this->allValues.emplace_back(value);
        //this->reverseLookup.emplace(value, key);
    }

    inline T* operator[](int index) const {
        return sortedRegistry[index];
    }

    inline const std::vector<T*>& getRegistry() const {
        return sortedRegistry;
    }

    /**
     * Orders the values from pointerLookup into sortedRegistry for quick lookup
     * crucial to be called at the end of setup
     */
    void orderMapping() {
        typename boost::unordered_map<int, T*, FakePointerHasher>::const_iterator getOrder;
        sortedRegistry.clear();
        for(getOrder = pointerLookup.cbegin(); getOrder != pointerLookup.cend(); ++getOrder) {
            sortedRegistry.push_back(getOrder->second);
        }
    }

    void reorderMapping(const std::vector<int>& lookupValues) {
        pointerLookup.clear();
        for (int i = 0; i < lookupValues.size(); ++i) {
            pointerLookup.emplace(lookupValues[i], allValues[i]);
        }
        orderMapping();
    }

    /**
     * Returns the size of current registry
     * @return size of current registry
     */
    inline int size() {
        return this->sortedRegistry.size();
    }

    /// Deallocates all the stored values
    inline void clear() {
        for(const T* value : allValues){
            delete value;
        }
        allValues.clear();
        sortedRegistry.clear();
        pointerLookup.clear();
    }


    //inline int getId(const T* value) const { return this->idLookup.find(value); }

    //inline T* get(ResourceLocation name) const { return this->registryObjects.at(name); }
private:
    //boost::unordered_map<ResourceLocation, T*> registryObjects;
    //boost::unordered_map<T*, int> idLookup;
    std::vector<T*> allValues;
    //boost::unordered_map<T*, ResourceLocation> reverseLookup;
    std::vector<T*> sortedRegistry;
    boost::unordered_map<int, T*, FakePointerHasher> pointerLookup;
};

