#pragma once

#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"

#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"

namespace loot_tables {
    class StrongholdLibrary : public StrongholdLoot<StrongholdLibrary> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void StrongholdLibrary::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::BOOK,             100, 1, 3);
        items.emplace_back(&Items::PAPER,            100, 2, 7);
        items.emplace_back(&Items::MAP,              5);
        items.emplace_back(&Items::COMPASS,          5);
        items.emplace_back(&Items::ENCHANTED_BOOK,   60);
        // function=enchant_with_levels, levels=30, treasure=true

        lootTables.emplace_back(items,             2, 10);


        maxItemsPossible = 10;
    }

    template <bool shuffle>
    Container StrongholdLibrary::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::ENCHANTED_BOOK_ID) {
                    EnchantmentHelper::EnchantWithLevelsBook::apply(lootTableSeed, &result, 30);
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

