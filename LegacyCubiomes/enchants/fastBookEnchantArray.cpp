#include "fastBookEnchantArray.hpp"


//==============================================================================
//                              ELDataArray
//==============================================================================


void ELDataArray::addData(Enchantment* ench, int id) {
    data[totalEnchants++] = EnchantmentData(ench, id);
}


EnchantmentData* ELDataArray::getIndex(int indexIn) {
    indexIn = deletions.getEnchantmentIndex(indexIn);
    return &data[indexIn];
}


EnchantmentData* ELDataArray::getLastEnchantmentAdded() {
    return &data[enchants.getLastValueIndex()];
}


void ELDataArray::addRandomItem(uint64_t *rng) {

    // get the total weight
    int theTotalWeight = this->totalWeight;
    for (int i = 0; i < deletions.getIndex(); i++) {
        int enchIndex = deletions.getValueAt(i);
        theTotalWeight -= data[enchIndex].getRarityWeight();
    }

    // get the rng weight
    int weight = nextInt(rng, theTotalWeight);

    // if it's the full enchantment list,
    // use a faster function
    if (theTotalWeight == EnchantedBookEnchantsLookupTable::TOTAL_WEIGHT) {
        size_t low = 0;
        size_t high = Enchantment::count;
        while (low < high) {
            size_t mid = (low + high) >> 1;
            if (EnchantedBookEnchantsLookupTable::CUMULATIVE_WEIGHT_ALL[mid] > weight) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }
        enchants.addItem((int)low);
        return;
    }

    // get the enchantment
    for (int enchIndex = 0; enchIndex < totalEnchants; enchIndex++) {

        for (int i = 0; i < deletions.getIndex(); i++)
            if (enchIndex == deletions.getValueAt(i))
                goto END;

        weight -= data[enchIndex].obj->rarity->getWeight();

        // If the right weight is found, add it to the enchantments
        if (weight < 0) {
            enchants.addItem(enchIndex);
            return;
        }

        END:;
    }
}


void ELDataArray::addEnchantments(ItemStack *itemStackIn) {
    for (int i = 0; i <= enchants.getIndex() - 1; i++) {
        itemStackIn->addEnchantmentData(&data[enchants.getValueAt(i)]);
    }
}





//==============================================================================
//                    EnchantedBookEnchantsLookupTable
//==============================================================================


ELDataArray* EnchantedBookEnchantsLookupTable::get(int cost) {
    ELDataArray* array = dataArrays[cost];
    array->clear();
    return array;
}


void EnchantedBookEnchantsLookupTable::setup() {

    // sets up base cumulative weights
    int sum = 0;
    for (int i = 0; i < Enchantment::count; ++i) {
        auto ench = Enchantment::REGISTRY[i];
        sum += ench->rarity->getWeight();
        CUMULATIVE_WEIGHT_ALL[i] = sum;
    }
    TOTAL_WEIGHT = sum;

    // sets up the dataArrays
    Enchantment *ench_pt;
    for (int cost = 0; cost < VECTOR_COUNT; cost++) {
        dataArrays[cost] = new ELDataArray();
        auto array = dataArrays[cost];

        for (auto & it : Enchantment::REGISTRY) {
            ench_pt = it.second;
            for (int level = ench_pt->maxLevel; level > 0; --level) {

                if (cost >= ench_pt->getMinCost(level) && cost <= ench_pt->getMaxCost(level)) {
                    array->addData(ench_pt, level);
                    array->totalWeight += ench_pt->rarity->getWeight();
                    break;
                }
            }
        }
    }
    areVectorsSetup = true;
}


void EnchantedBookEnchantsLookupTable::deallocate() {
    for (auto & vector : dataArrays) {
        delete vector;
        vector = nullptr;
    }
    areVectorsSetup = false;
}

int EnchantedBookEnchantsLookupTable::TOTAL_WEIGHT = 0;
int EnchantedBookEnchantsLookupTable::CUMULATIVE_WEIGHT_ALL[Enchantment::MAX_ENCHANTMENT_COUNT] = {0};