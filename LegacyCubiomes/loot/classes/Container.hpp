#pragma once

#include <map>
#include <numeric>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>

#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "LegacyCubiomes/loot/classes/LootTable.hpp"


class Container {
public:
    size_t numSlots{};
    std::vector<ItemStack> inventorySlots;
    static const inline int8_t CHEST_SIZE = 27;

    Container() : numSlots(CHEST_SIZE), inventorySlots(CHEST_SIZE) {}

    // don't mark explicit
    Container(std::vector<ItemStack>&& items) : inventorySlots(std::move(items)) {}
    explicit Container(int slots) : numSlots(slots), inventorySlots(slots) {}

    Container(int size, const std::vector<ItemStack>& inventorySlots) :
            numSlots(size), inventorySlots(inventorySlots) {}
    Container(int size, std::vector<ItemStack>&& items) : numSlots(size), inventorySlots(items) {}

    void shuffleIntoContainer(std::vector<ItemStack>& items, uint64_t rng);

    void printCombinedItems();
    friend std::ostream& operator<<(std::ostream& out, const Container &container);
#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Container &container);
#endif

    inline void setInventorySlotContents(int index, const ItemStack& stack){inventorySlots[index] = stack;}
    inline void setMoveInventorySlotContents(int index, ItemStack&& stack) {inventorySlots[index] = std::move(stack);}
    inline void placeIntoContainer(std::vector<ItemStack>& items) {inventorySlots = items;}


private:

    template <typename T>
    inline static void randomShuffle(std::vector<T> &items, uint64_t* rng) {
        int size = static_cast<int>(items.size());
        if (size <= 1) return;

        for (int rangeLimit = size - 1; rangeLimit > 0; --rangeLimit) {
            int randomIndex = nextInt(rng, rangeLimit);
            if (rangeLimit != randomIndex) {
                std::swap(items[randomIndex], items[rangeLimit]);
            }
        }
    }


};

