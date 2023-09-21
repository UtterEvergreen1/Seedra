#pragma once

#include <numeric>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <iostream>

#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "LegacyCubiomes/mc/items.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"
#include "LegacyCubiomes/enchants/enchantment.hpp"
#include "LegacyCubiomes/enchants/enchantmentData.hpp"

/**
 * Classes in this file:
 * LootItem
 * UniformRoll
 * ItemEntry
 * ItemStack
 * LootTable
 */



class LootItem {
public:
    const Items::Item* item;
    int weight;
    LootItem() : item(nullptr), weight(0) {}
    LootItem(const Items::Item* item, int weight) : item(item), weight(weight) {}
};


class UniformRoll {
public:
    int min;
    int max;
    UniformRoll() : min(0), max(0) {}
    UniformRoll(int min, int max) : min(min), max(max) {}
};


class ItemEntry : public LootItem, public UniformRoll {
public:
    ItemEntry() = default;
    ItemEntry(const Items::Item* item, int weight) : LootItem(item, weight), UniformRoll(1, 1) {}
    ItemEntry(const Items::Item* item, int weight, int min, int max) : LootItem(item, weight), UniformRoll(min, max) {}
};


class ItemStack {
public:
    const Items::Item* item;
    int stackSize{};
    std::vector<EnchantmentData> enchantments;

    ItemStack() : item(nullptr), stackSize(0) {}
    explicit ItemStack(const Items::Item* item) : item(item), stackSize(0) {}
    ItemStack(const Items::Item* item, int stackSize) : item(item), stackSize(stackSize) {}

    ItemStack splitStack(int amount);
    ND const Items::Item* getItem() const;

    void addEnchantment(Enchantment* enchantment, int level);
    void addEnchantmentData(EnchantmentData* enchantmentData);

    friend std::ostream& operator<<(std::ostream& out, const ItemStack &itemStack) {
        if (itemStack.stackSize > 1) {
            out << itemStack.item->getName() << " (" << itemStack.stackSize << ")";
        } else {
            out << itemStack.item->getName();
        }
        if (!itemStack.enchantments.empty()) {
            out << " [";
            for (size_t i = 0; i < itemStack.enchantments.size(); ++i) {
                out << itemStack.enchantments[i].toString();
                if (i < itemStack.enchantments.size() - 1)
                    out << ", ";
            }
            out << "]";
        }
        return out;
    }

    #ifdef INCLUDE_QT
    friend QDebug ItemStack::operator<<(QDebug out, const ItemStack &itemStack) {
        if (itemStack.stackSize > 1) {
            out.nospace() << *(itemStack.itemEntry) << " (" << itemStack.stackSize << ")";
        }
        else {
            out.nospace() << *(itemStack.itemEntry);
        }
        return out.nospace();
    }
    #endif
};



class LootTable : public UniformRoll {
public:
    std::vector<ItemEntry> items;
    std::vector<int> cumulativeWeights;

    int totalWeight{};
    int maxItemsPossible{};

    LootTable() = default;

    LootTable(const std::vector<ItemEntry> &items, int amount) :
            UniformRoll(amount, amount), items(items) {
        computeCumulativeWeights();
    }

    LootTable(const std::vector<ItemEntry> &items, int min, int max) :
            UniformRoll(min, max), items(items) {
        computeCumulativeWeights();
    }

    template<bool legacy>
    static int getInt(uint64_t *random, int minimum, int maximum) {
        if constexpr (legacy)
            return nextInt(random, maximum - minimum + 1) + minimum;
        else
            return minimum >= maximum ? minimum : (nextInt(random, maximum - minimum + 1) + minimum);
    }

    void computeCumulativeWeights();

    /**
     * Uses a custom binary search and cumulative weights to find the items.
     * @tparam legacy
     * @param rng
     * @return
     */
    template<bool legacy>
    ItemStack createLootRoll(uint64_t *rng) const {
        int randomWeight = nextInt(rng, totalWeight);
        // std::cout << randomWeight << " " << totalWeight << std::endl;

        size_t high = cumulativeWeights.size();
        size_t low = 0;
        while (low < high) {
            size_t mid = (low + high) >> 1;
            if (cumulativeWeights[mid] > randomWeight) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }

        const ItemEntry &selectedItem = items[low];
        return {selectedItem.item, LootTable::getInt<legacy>(rng, selectedItem.min, selectedItem.max)};

    }
};




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

