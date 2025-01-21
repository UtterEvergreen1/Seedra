#include "fastBookEnchantArray.hpp"

//==============================================================================
//                              ELDataArray
//==============================================================================


void ELDataArray::addData(Enchantment* ench, const int id) { data[totalEnchants++] = EnchantmentData(ench, id); }


EnchantmentData* ELDataArray::getIndex(int indexIn) {
    indexIn = deletions.getEnchantmentIndex(indexIn);
    return &data[indexIn];
}


EnchantmentData* ELDataArray::getLastEnchantmentAdded() { return &data[enchants.getLastValueIndex()]; }


void ELDataArray::addRandomItem(RNG& rng) {

    // get the total weight
    int theTotalWeight = static_cast<u8>(this->totalWeight);
    for (int i = 0; i < deletions.getIndex(); i++) {
        c_int enchIndex = deletions.getValueAt(i);
        theTotalWeight -= data[enchIndex].obj->rarity;
    }

    // get the rng weight
    int weight = rng.nextInt(theTotalWeight);

    // if it's the full enchantment list,
    // use a faster function
    if (theTotalWeight == EnchantedBookEnchantsLookupTable::TOTAL_WEIGHT) {
        size_t low = 0;
        size_t high = Enchantment::count;
        while (low < high) {
            if (const size_t mid = (low + high) >> 1;
                EnchantedBookEnchantsLookupTable::CUMULATIVE_WEIGHT_ALL[mid] > weight) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }
        enchants.addItem(static_cast<int>(low));
        return;
    }

    // get the enchantment
    for (int enchIndex = 0; enchIndex < totalEnchants; enchIndex++) {

        bool isDeleted = false;
        for (int i = 0; i < deletions.getIndex(); i++) {
            if (enchIndex == deletions.getValueAt(i)) {
                isDeleted = true;
                break;
            }
        }

        if (isDeleted) {
            continue;
        }

        weight -= data[enchIndex].obj->rarity;

        // If the right weight is found, add it to the enchantments
        if (weight < 0) {
            enchants.addItem(enchIndex);
            return;
        }

    END:;
    }
}


void ELDataArray::addEnchantments(ItemStack& itemStackIn) {
    for (int i = 0; i < enchants.getIndex(); i++) {
        itemStackIn.addEnchantmentData(&data[enchants.getValueAt(i)]);
    }
}


//==============================================================================
//                    EnchantedBookEnchantsLookupTable
//==============================================================================


ELDataArray* EnchantedBookEnchantsLookupTable::get(c_int cost) const {
    ELDataArray* array = dataArrays[cost];
    array->clear();
    return array;
}


void EnchantedBookEnchantsLookupTable::setup() {

    // sets up base cumulative weights
    int sum = 0;
    for (int i = 0; i < Enchantment::REGISTRY.size(); ++i) {
        const Enchantment* ench = Enchantment::REGISTRY[i];
        sum += ench->rarity;
        CUMULATIVE_WEIGHT_ALL[i] = static_cast<i8>(sum);
    }
    TOTAL_WEIGHT = static_cast<i8>(sum);

    // sets up the dataArrays
    for (int cost = 0; cost < VECTOR_COUNT; cost++) {
        dataArrays[cost] = new ELDataArray();
        c_auto array = dataArrays[cost];

        int i = 0;
        for (Enchantment* ench_pt: Enchantment::REGISTRY.getRegistry()) {
            if (ench_pt == nullptr) throw std::runtime_error("Enchantment pointer is NULL; the table is incorrect!");
            for (int level = ench_pt->maxLevel; level > 0; --level) {
                const auto minCost = ench_pt->getMinCost(level);
                const auto maxCost = ench_pt->getMaxCost(level);
                if (cost >= minCost && cost <= maxCost) {
                    array->addData(ench_pt, level);
                    array->totalWeight += ench_pt->rarity;
                    break;
                }
            }
            i++;
        }
    }
    areVectorsSetup = true;
}


void EnchantedBookEnchantsLookupTable::deallocate() {
    for (auto& vector: dataArrays) {
        delete vector;
        vector = nullptr;
    }
    areVectorsSetup = false;
}

i8 EnchantedBookEnchantsLookupTable::TOTAL_WEIGHT = 0;
i8 EnchantedBookEnchantsLookupTable::CUMULATIVE_WEIGHT_ALL[Enchantment::MAX_ENCHANTMENT_COUNT] = {0};
