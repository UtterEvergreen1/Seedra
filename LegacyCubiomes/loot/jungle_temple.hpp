#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"


namespace loot_tables {
    class JungleTemple : public Loot<JungleTemple> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void JungleTemple::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::EMERALD,             15, 1, 3);
        items.emplace_back(&Items::IRON_INGOT,          50, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT,          75, 2, 7);
        items.emplace_back(&Items::EMERALD,             10, 1, 3);
        items.emplace_back(&Items::BONE,               100, 4, 6);
        items.emplace_back(&Items::ROTTEN_FLESH,        80, 3, 7);
        items.emplace_back(&Items::SADDLE,              15);
        items.emplace_back(&Items::IRON_HORSE_ARMOR,    5);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR,  5);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR, 5);
        items.emplace_back(&Items::ENCHANTED_BOOK,      6);
        // function=enchant_with_levels, levels=30, treasure=true

        lootTables.emplace_back(items,           2, 6);

        maxItemsPossible = 6;
    }

    template <bool shuffle>
    Container JungleTemple::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
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

