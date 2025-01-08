#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "enchantment.hpp"


template<int items>
class IndexArrayTemplate {
    static constexpr i8 ITEM_COUNT = items;
    i8 indexes[ITEM_COUNT] = {};
    i8 currentIndex = 0;

public:
    IndexArrayTemplate() = default;
    void clear() { currentIndex = 0; }

    ND int getValueAt(int indexIn) const { return indexes[indexIn]; }
    ND int getLastValueIndex() const { return indexes[currentIndex - 1]; }

    ND int getIndex() const { return currentIndex; }

    void addItem(c_int indexIn) { indexes[currentIndex++] = static_cast<i8>(indexIn); }

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
    i8 totalWeight = 0;
    i8 totalEnchants = 0;

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
    static constexpr i8 VECTOR_COUNT = 48;
    ELDataArray* dataArrays[VECTOR_COUNT] = {nullptr};
    bool areVectorsSetup = false;

public:
    static i8 TOTAL_WEIGHT;
    static i8 CUMULATIVE_WEIGHT_ALL[Enchantment::MAX_ENCHANTMENT_COUNT];

    EnchantedBookEnchantsLookupTable() = default;
    MU explicit EnchantedBookEnchantsLookupTable(c_bool allocate) {
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
