#pragma once

#include <algorithm>

#include "enchantment.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"


class IndexArray {
private:
    static constexpr int ITEM_COUNT = 16;
    int indexes[ITEM_COUNT] = {0};
    int currentIndex = 0;

public:
    IndexArray() = default;
    inline void clear() {
        currentIndex = 0;
    }

    ND inline int getValueAt(int indexIn) const {return indexes[indexIn];}
    ND inline int getLastValueIndex() const {return indexes[currentIndex - 1];}

    ND inline int getIndex() const {return currentIndex;}

    inline void addItem(int indexIn) {indexes[currentIndex++] = indexIn;}

    inline int getEnchantmentIndex(int indexIn) {
        for (int i = 0; i < currentIndex; i++)
            if (currentIndex > indexes[i])
                indexIn--;
        return indexIn;
    }
};



class ELDataArray {
public:
    static constexpr int ENCHANTMENT_COUNT = 29;
    int totalWeight = 0;
    int totalEnchants = 0;

    IndexArray deletions = IndexArray();
    IndexArray enchants = IndexArray();
    EnchantmentData data[ENCHANTMENT_COUNT] = {};
    ELDataArray() = default;

    void addData(Enchantment* ench, int id);

    inline void clear() {
        deletions.clear();
        enchants.clear();
    }

    EnchantmentData* getIndex(int indexIn);
    EnchantmentData* getLastEnchantmentAdded();
    void addRandomItem(uint64_t *rng);

    /**
     * This should only be used by a setup function!
     * @param itemStackIn
     */
    void addEnchantments(ItemStack *itemStackIn);


};



class EnchantedBookEnchantsLookupTable {
private:
    static constexpr int VECTOR_COUNT = 48;
    ELDataArray* dataArrays[VECTOR_COUNT] = {nullptr};
    bool areVectorsSetup = false;

public:
    EnchantedBookEnchantsLookupTable() = default;
    explicit EnchantedBookEnchantsLookupTable(bool allocate) {if (allocate) setup();}
    ~EnchantedBookEnchantsLookupTable() {deallocate();}
    ND bool isSetup() const {return areVectorsSetup;}

    /**
     * level must be between 0 and 47 or bad things will happen.
     * @param cost the enchantment level
     * @return
     */
    ELDataArray* get(int cost);

    /**
     * This assumes the item is an enchanted book!
     * This must be ran else the data from it will not work.
     */
    void setup();

    void deallocate();

    static int TOTAL_WEIGHT;
    static int CUMULATIVE_WEIGHT_ALL[29];
};












