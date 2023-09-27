#pragma once

#include <map>
#include <numeric>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <iostream>
#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "LegacyCubiomes/loot/classes/LootTable.hpp"


class Container {
public:
    size_t numSlots{};
    std::vector<ItemStack> inventorySlots;

    Container() = default;
    explicit Container(int numSlots) : numSlots(numSlots), inventorySlots(numSlots) {}
    Container(int numSlots, const std::vector<ItemStack>& inventorySlots) :
            numSlots(numSlots), inventorySlots(inventorySlots) {}

    inline void setInventorySlotContents(int index, const ItemStack& stack){
        inventorySlots[index] = stack;
    }

    inline void placeIntoContainer(std::vector<ItemStack>& items){
        inventorySlots = items;
    }

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

    void printCombinedItems() {
        std::map<const Items::Item*, int> itemCount;

        for (const auto& itemStack : inventorySlots) {
            if (itemStack.stackSize > 0) {
                if (itemCount.find(itemStack.item) != itemCount.end()) {
                    itemCount[itemStack.item] += itemStack.stackSize;
                } else {
                    itemCount[itemStack.item] = itemStack.stackSize;
                }
            }
        }

        for (const auto& pair : itemCount) {
            ItemStack itemStack = ItemStack(pair.first, pair.second);
            std::cout << itemStack << std::endl;
        }
    }

    /**
     * shuffle items into container
     * @param items
     * @param rng
     */
    void shuffleIntoContainer(std::vector<ItemStack>& items, uint64_t rng) {
        // pre-allocate here
        int itemIndex;
        int splitAmount;
        uint64_t rngState = rng;
        std::vector<int> slotOrder(numSlots);
        std::iota(slotOrder.begin(), slotOrder.end(), 0);
        std::vector<ItemStack> stackableItems;

        randomShuffle<int>(slotOrder, &rngState);

        // First pass: Move items with stackSize > 1 to stackableItems
        for (const ItemStack& item : items) {
            if (item.stackSize > 1) {
                stackableItems.push_back(item);
            }
        }

        // Second pass: Compact items
        auto writeIter = items.begin();
        for (const ItemStack& item : items) {
            if (item.stackSize <= 1) {
                *writeIter = item;
                ++writeIter;
            }
        }
        items.resize(writeIter - items.begin());

        numSlots -= items.size();
        while(numSlots > 0 && !stackableItems.empty()) {
            itemIndex = LootTable::getInt<false>(&rngState, 0, static_cast<int>(stackableItems.size()) - 1);
            ItemStack originalStack = stackableItems[itemIndex];
            stackableItems.erase(stackableItems.begin() + itemIndex);

            splitAmount = LootTable::getInt<false>(&rngState, 1, originalStack.stackSize >> 1);
            ItemStack splittedStack = originalStack.splitStack(splitAmount);

            if (originalStack.stackSize == 0 || next(&rngState, 1) == 0)
                items.emplace_back(originalStack);
            else
                stackableItems.emplace_back(originalStack);

            if (splittedStack.stackSize == 0 || next(&rngState, 1) == 0)
                items.emplace_back(splittedStack);
            else
                stackableItems.emplace_back(splittedStack);
        }

        items.insert(items.end(), std::make_move_iterator(stackableItems.begin()),
                     std::make_move_iterator(stackableItems.end()));
        randomShuffle<ItemStack>(items, &rngState);
        for (const ItemStack& itemStack : items) {
            if(slotOrder.empty()) // Tried to over-fill a container
                return;

            if (itemStack.stackSize > 0) {
                itemIndex = slotOrder.back();
                slotOrder.pop_back();
                setInventorySlotContents(itemIndex, itemStack);
            }
            else {
                slotOrder.pop_back();
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Container &container) {
        int contents = (int)container.inventorySlots.size();
        out << "\n{\n";
        for(int i = 0; i < contents; i++){
            const ItemStack& itemStack = container.inventorySlots[i];
            if (itemStack.stackSize > 0) {
                out << i << ": " << itemStack << "\n";
            }
        }
        out << "}";
        return out;
    }


#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Container &container) {
        int contents = container.inventorySlots.size();
        out.nospace() << "\n{\n";
        for(int i = 0; i < contents; i++){
            const ItemStack& itemStack = container.inventorySlots[i];
            if (itemStack.stackSize > 0) {
                out.nospace() << i << ": " << itemStack << "\n";
            }
        }
        out.nospace() << "}";
        return out.space();
    }
#endif
};

