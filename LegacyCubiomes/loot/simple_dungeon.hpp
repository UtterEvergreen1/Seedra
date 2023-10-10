#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"


namespace loot {
    class SimpleDungeon : public Loot<SimpleDungeon> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(RNG& lootTableSeed);
    };

    void SimpleDungeon::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&Items::SADDLE,                      20);
        items1.emplace_back(&Items::GOLDEN_APPLE,                15);
        items1.emplace_back(&Items::ENCHANTED_GOLDEN_APPLE,      2);
        items1.emplace_back(&Items::DISC_13,                     15);
        items1.emplace_back(&Items::DISC_CAT,                    15);
        items1.emplace_back(&Items::NAME_TAG,                    20);
        items1.emplace_back(&Items::GOLDEN_HORSE_ARMOR,          10);
        items1.emplace_back(&Items::IRON_HORSE_ARMOR,            15);
        items1.emplace_back(&Items::DIAMOND_HORSE_ARMOR,         5);
        items1.emplace_back(&Items::ENCHANTED_BOOK,              10);
        // function=enchant_randomly
        lootTables.emplace_back(items1,                  1, 3);

        // table 2
        items2.emplace_back(&Items::IRON_INGOT,                  10, 1, 4);
        items2.emplace_back(&Items::GOLD_INGOT,                  5, 1, 4);
        items2.emplace_back(&Items::BREAD,                       20);
        items2.emplace_back(&Items::WHEAT,                       20, 1, 4);
        items2.emplace_back(&Items::BUCKET,                      10);
        items2.emplace_back(&Items::REDSTONE,                    15, 1, 4);
        items2.emplace_back(&Items::COAL,                        15, 1, 4);
        items2.emplace_back(&Items::MELON_SEEDS,                 10, 2, 4);
        items2.emplace_back(&Items::PUMPKIN_SEEDS,               10, 2, 4);
        items2.emplace_back(&Items::BEETROOT_SEEDS,              10, 2, 4);
        lootTables.emplace_back(items2,                  1, 4);

        // table 3
        items3.emplace_back(&Items::BONE,                        10, 1, 8);
        items3.emplace_back(&Items::GUNPOWDER,                   10, 1, 8);
        items3.emplace_back(&Items::ROTTEN_FLESH,                10, 1, 8);
        items3.emplace_back(&Items::STRING,                      10, 1, 8);
        lootTables.emplace_back(items3,                    3);

        maxItemsPossible = 10;
    }

    template <bool shuffle>
    Container SimpleDungeon::getLootFromLootTableSeed(RNG& lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        lootTableSeed.setSeed(lootTableSeed.getSeed());

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.getMin(), table.getMax());
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::ENCHANTED_BOOK_ID) {
                    EnchantmentHelper::EnchantRandomlyBook::apply(lootTableSeed, &result);
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return  {27, chestContents};
    }
}

