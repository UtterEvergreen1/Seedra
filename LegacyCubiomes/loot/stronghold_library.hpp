#pragma once

#include "base_classes/stronghold_loot.hpp"

using namespace Items;

namespace loot_tables {
    class StrongholdLibrary : public StrongholdLoot<StrongholdLibrary> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void StrongholdLibrary::setup() {
        std::vector<ItemEntry> items;

        #ifdef INCLUDE_JAVA
            items.emplace_back(&BOOK,             20, 1, 3);
            items.emplace_back(&PAPER,            20, 2, 7);
            items.emplace_back(&MAP,               1      );
            items.emplace_back(&COMPASS,           1      );
            items.emplace_back(&ENCHANTED_BOOK,   10      ); // function=enchant_with_levels, levels=30, treasure=true
            lootTables.emplace_back(items, 2, 10, 52);
        #else
            items.emplace_back(&BOOK,             100, 1, 3);
            items.emplace_back(&PAPER,            100, 2, 7);
            items.emplace_back(&MAP,              5);
            items.emplace_back(&COMPASS,          5);
            items.emplace_back(&ENCHANTED_BOOK,   60); // function=enchant_with_levels, levels=30, treasure=true
            lootTables.emplace_back(items,             2, 10, 270);
        #endif

        maxItemsPossible = 10;
    }

    template <bool shuffle>
    Container StrongholdLibrary::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
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

