#include "fastEnchantArray.hpp"


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
    // std::cout << "weight " << weight << std::endl;
    // std::cout << "total weight " << theTotalWeight << std::endl;

    // get the enchantment
    for (int enchIndex = 0; enchIndex < totalEnchants; enchIndex++) {

        // Check if the enchantment is in the deletion list
        bool isDeleted = false;
        for (int i = 0; i < deletions.getIndex(); i++) {
            if (enchIndex == deletions.getValueAt(i)) {
                isDeleted = true;
                break;
            }
        }

        // If not in the deletion list, decrement weight and check
        if (!isDeleted) {
            weight -= data[enchIndex].obj->rarity->getWeight();

            // If the right weight is found, add it to the enchantments
            if (weight < 0) {
                enchants.addItem(enchIndex);
                return;
            }
        }

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
    Enchantment *ench_pt;

    for (int cost = 0; cost < VECTOR_COUNT; cost++) {
        int enchIndex = 0;
        dataArrays[cost] = new ELDataArray();
        auto array = dataArrays[cost];

        bool enchantFound = false;
        for (auto & it : Enchantment::REGISTRY) {
            ench_pt = it.second;
            for (int level = ench_pt->maxLevel; level > 0; --level) {

                if (cost >= ench_pt->getMinCost(level) && cost <= ench_pt->getMaxCost(level)) {
                    array->addData(ench_pt, level);
                    array->totalWeight += ench_pt->rarity->getWeight();
                    enchantFound = true;
                    break;
                }
            }

            if (!enchantFound) {

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

