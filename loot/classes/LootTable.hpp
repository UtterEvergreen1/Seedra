#pragma once

#include "lce/items/items.hpp"

#include "enchants/enchantment.hpp"
#include "support/rng.hpp"
#include "ItemEntry.hpp"
#include "ItemStack.hpp"
#include "UniformRoll.hpp"


class LootTable : public UniformRoll {
public:
    std::vector<ItemEntry> items;
    std::vector<u16> cumulativeWeights;
    int totalWeight;

    LootTable(const std::vector<ItemEntry>& items, c_int amount)
        : UniformRoll(amount, amount), items(items), totalWeight(0) {
        computeCumulativeWeights();
    }

    LootTable(const std::vector<ItemEntry>& items, c_int min, c_int max)
        : UniformRoll(min, max), items(items), totalWeight(0) {
        computeCumulativeWeights();
    }

    template<bool legacy>
    static u8 getInt(RNG& rng, c_u8 minimum, c_u8 maximum) {
        if constexpr (legacy) return rng.nextIntLegacy(minimum, maximum);
        else
            return rng.nextInt(minimum, maximum);
    }

    void computeCumulativeWeights();

    /**
     * Uses a custom binary search and cumulative weights to find the items.
     * @tparam legacy if the version is below elytra update
     * @param rng the rng state
     * @return
     */
    template<bool legacy>
    ND ItemStack createLootRoll(RNG& rng) const {
        c_int randomWeight = rng.nextInt(totalWeight);

        size_t high = cumulativeWeights.size();
        size_t low = 0;
        while (low < high) {
            const size_t mid = (low + high) >> 1;
            if (cumulativeWeights[mid] > randomWeight) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }

        const ItemEntry& selectedItem = items[low];
        ItemStack itemStack = {selectedItem.item,
                               LootTable::getInt<legacy>(rng, selectedItem.getMin(), selectedItem.getMax())};
        const size_t functionsSize = selectedItem.functions.size();
        if EXPECT_FALSE (functionsSize) {
            for (size_t index = 0; index < functionsSize; index++) {
                selectedItem.functions[index]->apply(itemStack, rng);
            }
        }

        return std::move(itemStack);
    }
};
