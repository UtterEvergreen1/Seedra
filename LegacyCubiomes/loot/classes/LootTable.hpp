#pragma once

#include "lce/items/items.hpp"

#include "ItemEntry.hpp"
#include "LegacyCubiomes/enchants/enchantment.hpp"
#include "LegacyCubiomes/enchants/enchantmentData.hpp"
#include "LegacyCubiomes/loot/classes/ItemStack.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "UniformRoll.hpp"


class LootTable : public UniformRoll {
public:
    std::vector<ItemEntry> items;
    std::vector<uint16_t> cumulativeWeights;
    int totalWeight;

    LootTable(const std::vector<ItemEntry>& items, const int amount)
        : UniformRoll(amount, amount), items(items), totalWeight(0) {
        computeCumulativeWeights();
    }

    LootTable(const std::vector<ItemEntry>& items, const int min, const int max)
        : UniformRoll(min, max), items(items), totalWeight(0) {
        computeCumulativeWeights();
    }

    template<bool legacy>
    static uint8_t getInt(RNG& rng, const uint8_t minimum, const uint8_t maximum) {
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
        const int randomWeight = rng.nextInt(totalWeight);
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

        const ItemEntry& selectedItem = items[low];
        ItemStack itemStack = {selectedItem.item,
                               LootTable::getInt<legacy>(rng, selectedItem.getMin(), selectedItem.getMax())};
        int functionsSize = (int) selectedItem.functions.size();
        if EXPECT_FALSE (functionsSize) {
            for (int index = 0; index < functionsSize; index++) {
                selectedItem.functions[index]->apply(itemStack, rng);
            }
        }

        return std::move(itemStack);
    }
};
