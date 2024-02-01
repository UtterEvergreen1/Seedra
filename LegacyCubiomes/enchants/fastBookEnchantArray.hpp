#pragma once

#include <algorithm>
#include <exception>

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "enchantment.hpp"


template<int items>
class IndexArrayTemplate {
    static constexpr int8_t ITEM_COUNT = items;
    int8_t indexes[ITEM_COUNT] = {0};
    int8_t currentIndex = 0;

public:
    IndexArrayTemplate() = default;
    void clear() { currentIndex = 0; }

    ND int getValueAt(int indexIn) const { return indexes[indexIn]; }
    ND int getLastValueIndex() const { return indexes[currentIndex - 1]; }

    ND int getIndex() const { return currentIndex; }

    void addItem(const int indexIn) { indexes[currentIndex++] = static_cast<int8_t>(indexIn); }

    MU int getEnchantmentIndex(int indexIn) {
        for (int i = 0; i < currentIndex; i++)
            if (currentIndex > indexes[i]) indexIn--;
        return indexIn;
    }
};

typedef IndexArrayTemplate<12> IndexArraySmall;
typedef IndexArrayTemplate<16> IndexArrayLarge;


class ELDataArray {
public:
    int8_t totalWeight = 0;
    int8_t totalEnchants = 0;

    IndexArrayLarge deletions = IndexArrayLarge();
    IndexArraySmall enchants = IndexArraySmall();
    EnchantmentData data[Enchantment::MAX_ENCHANTMENT_COUNT] = {};
    ELDataArray() = default;

    void addData(Enchantment* ench, int id);

    void clear() {
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
    void addEnchantments(ItemStack& itemStackIn);
};

class EnchantedBookEnchantsLookupTable {
    static constexpr int8_t VECTOR_COUNT = 48;
    ELDataArray* dataArrays[VECTOR_COUNT] = {nullptr};
    bool areVectorsSetup = false;

public:
    static int8_t TOTAL_WEIGHT;
    static int8_t CUMULATIVE_WEIGHT_ALL[Enchantment::MAX_ENCHANTMENT_COUNT];

    EnchantedBookEnchantsLookupTable() = default;
    MU explicit EnchantedBookEnchantsLookupTable(const bool allocate) {
        if (allocate) setup();
    }
    ~EnchantedBookEnchantsLookupTable() { deallocate(); }
    ND MU bool isSetup() const { return areVectorsSetup; }

    /**
     * level must be between 0 and 47 or bad things will happen.
     * @param cost the enchantment level
     * @return
     */
    ND ELDataArray* get(int cost) const;

    /**
     * This assumes the item is an enchanted book!
     * This must be ran else the data from it will not work.
     */
    void setup();

    void deallocate();
};
