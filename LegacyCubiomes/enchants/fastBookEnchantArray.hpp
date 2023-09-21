#pragma once

#include <algorithm>

#include "enchantment.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"


class IndexArraySmall {
private:
    static constexpr int ITEM_COUNT = 12;
    int indexes[ITEM_COUNT] = {0};
    int currentIndex = 0;

public:
    IndexArraySmall() = default;
    inline void clear() {
        currentIndex = 0;
    }

    ND inline int getValueAt(int indexIn) const {return indexes[indexIn];}
    ND inline int getLastValueIndex() const {return indexes[currentIndex - 1];}

    ND inline int getIndex() const {return currentIndex;}

    inline void addItem(int indexIn) {indexes[currentIndex++] = indexIn;}

    MU inline int getEnchantmentIndex(int indexIn) {
        for (int i = 0; i < currentIndex; i++)
            if (currentIndex > indexes[i])
                indexIn--;
        return indexIn;
    }
};


class IndexArrayLarge {
private:
    static constexpr int ITEM_COUNT = 16;
    int indexes[ITEM_COUNT] = {0};
    int currentIndex = 0;

public:
    IndexArrayLarge() = default;
    inline void clear() {
        currentIndex = 0;
    }

    ND inline int getValueAt(int indexIn) const {return indexes[indexIn];}
    ND MU inline int getLastValueIndex() const {return indexes[currentIndex - 1];}

    ND inline int getIndex() const {return currentIndex;}

    inline void addItem(int indexIn) {indexes[currentIndex++] = indexIn;}

    inline int getEnchantmentIndex(int indexIn) {
        for (int i = 0; i < currentIndex; i++)
            if (currentIndex > indexes[i])
                indexIn--;
        return indexIn;
    }
};


class EnchantedBookEnchantsLookupTable;

class ELDataArray {
public:
    EnchantedBookEnchantsLookupTable* rootPtr = nullptr;
    int totalWeight = 0;
    int totalEnchants = 0;

    IndexArrayLarge deletions = IndexArrayLarge();
    IndexArraySmall enchants = IndexArraySmall();
    EnchantmentData data[Enchantment::MAX_ENCHANTMENT_COUNT] = {};
    ELDataArray() = default;

    explicit ELDataArray(EnchantedBookEnchantsLookupTable* rootPtrIn) {
        rootPtr = rootPtrIn;
    }

    void addData(Enchantment* ench, int id);

    inline void clear() {
        deletions.clear();
        enchants.clear();
    }

    MU EnchantmentData* getIndex(int indexIn);

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
    ELDataArray* dataArrays[48] = {nullptr};
    bool areVectorsSetup = false;

public:
    EnchantedBookEnchantsLookupTable() = default;
    MU explicit EnchantedBookEnchantsLookupTable(bool allocate) {if (allocate) setup();}

    ~EnchantedBookEnchantsLookupTable() {deallocate();}
    ND MU bool isSetup() const {return areVectorsSetup;}

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

    int TOTAL_WEIGHT = 0;
    int CUMULATIVE_WEIGHT_ALL[Enchantment::MAX_ENCHANTMENT_COUNT] = {0};
};












