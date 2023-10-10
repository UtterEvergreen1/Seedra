#pragma once

#include <algorithm>
#include <exception>

#include "enchantment.hpp"
#include "LegacyCubiomes/loot/classes/Loot.hpp"


class IndexArraySmall {
private:
    static constexpr int8_t ITEM_COUNT = 12;
    int8_t indexes[ITEM_COUNT] = {0};
    int8_t currentIndex = 0;

public:
    IndexArraySmall() = default;
    inline void clear() {
        currentIndex = 0;
    }

    ND inline int getValueAt(int indexIn) const {return indexes[indexIn];}
    ND inline int getLastValueIndex() const {return indexes[currentIndex - 1];}

    ND inline int getIndex() const {return currentIndex;}

    inline void addItem(int indexIn) {indexes[currentIndex++] = (int8_t)indexIn;}

    MU inline int getEnchantmentIndex(int indexIn) {
        for (int i = 0; i < currentIndex; i++)
            if (currentIndex > indexes[i])
                indexIn--;
        return indexIn;
    }
};


class IndexArrayLarge {
private:
    static constexpr int8_t ITEM_COUNT = 16;
    int8_t indexes[ITEM_COUNT] = {0};
    int8_t currentIndex = 0;

public:
    IndexArrayLarge() = default;
    inline void clear() {
        currentIndex = 0;
    }

    ND inline int getValueAt(int indexIn) const {return indexes[indexIn];}
    ND MU inline int getLastValueIndex() const {return indexes[currentIndex - 1];}

    ND inline int getIndex() const {return currentIndex;}

    inline void addItem(int indexIn) {indexes[currentIndex++] = (int8_t)indexIn;}

    inline int getEnchantmentIndex(int indexIn) {
        for (int i = 0; i < currentIndex; i++)
            if (currentIndex > indexes[i])
                indexIn--;
        return indexIn;
    }
};



class ELDataArray {
public:
    int8_t totalWeight = 0;
    int8_t totalEnchants = 0;

    IndexArrayLarge deletions = IndexArrayLarge();
    IndexArraySmall enchants = IndexArraySmall();
    EnchantmentData data[Enchantment::MAX_ENCHANTMENT_COUNT] = {};
    ELDataArray() = default;

    void addData(Enchantment* ench, int id);

    inline void clear() {
        deletions.clear();
        enchants.clear();
    }

    MU EnchantmentData* getIndex(int indexIn);

    EnchantmentData* getLastEnchantmentAdded();
    void addRandomItem(RNG& rng);

    /**
     * This should only be used by a setWorldSize function!
     * @param itemStackIn
     */
    void addEnchantments(ItemStack *itemStackIn);


};



class EnchantedBookEnchantsLookupTable {
private:
    static constexpr int8_t VECTOR_COUNT = 48;
    ELDataArray* dataArrays[VECTOR_COUNT] = {nullptr};
    bool areVectorsSetup = false;

public:
    static int8_t TOTAL_WEIGHT;
    static int8_t CUMULATIVE_WEIGHT_ALL[Enchantment::MAX_ENCHANTMENT_COUNT];

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

};












