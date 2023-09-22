#pragma once

#include "base_classes/loot.hpp"

#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"

using namespace Items;

namespace loot_tables {
    class JungleTemple : public Loot<JungleTemple> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void JungleTemple::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&EMERALD,             15, 1, 3);
        items.emplace_back(&IRON_INGOT,          50, 1, 5);
        items.emplace_back(&GOLD_INGOT,          75, 2, 7);
        items.emplace_back(&EMERALD,             10, 1, 3);
        items.emplace_back(&BONE,               100, 4, 6);
        items.emplace_back(&ROTTEN_FLESH,        80, 3, 7);
        items.emplace_back(&SADDLE,              15);
        items.emplace_back(&IRON_HORSE_ARMOR,    5);
        items.emplace_back(&GOLDEN_HORSE_ARMOR,  5);
        items.emplace_back(&DIAMOND_HORSE_ARMOR, 5);
        items.emplace_back(&ENCHANTED_BOOK,      6);
        // function=enchant_with_levels, levels=30, treasure=true

        lootTables.emplace_back(items,           2, 6);

        maxItemsPossible = 6;
    }

    template <bool shuffle>
    Container JungleTemple::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::ENCHANTED_BOOK_ID) {
                    EnchantmentHelper::EnchantWithLevelsBook::apply<true>(lootTableSeed, &result, 30);
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

