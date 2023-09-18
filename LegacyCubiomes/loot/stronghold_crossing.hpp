#pragma once

#include "base_classes/stronghold_loot.hpp"

using namespace Items;


namespace loot_tables {
    class StrongholdCrossing : public StrongholdLoot<StrongholdCrossing> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void StrongholdCrossing::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&IRON_INGOT,       50, 1, 5);
        items.emplace_back(&GOLD_INGOT,       25, 1, 3);
        items.emplace_back(&REDSTONE,         25, 4, 9);
        items.emplace_back(&COAL,             50, 3, 8);
        items.emplace_back(&BREAD,            75, 1, 3);
        items.emplace_back(&APPLE,            75, 1, 3);
        items.emplace_back(&IRON_PICKAXE,     5);
        items.emplace_back(&ENCHANTED_BOOK,   6); // function=enchant_with_levels, levels=30, treasure=true
        items.emplace_back(&INK_SACK,         75, 1, 3);
        lootTables.emplace_back(items,             1, 4, 386);

        maxItemsPossible = 4;
    }

    template <bool shuffle>
    Container StrongholdCrossing::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item == &Items::ENCHANTED_BOOK) {
                    EnchantmentHelper::EnchantWithLevels::apply(lootTableSeed, &result, 30, true, true);
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, *lootTableSeed);
            return container;
        }
        else
            return  {27, chestContents};
    }
}

