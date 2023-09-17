#pragma once

#include "base_classes/loot.hpp"

using namespace Items;


namespace loot_tables {
    class SpawnBonusChest : public Loot<SpawnBonusChest> {
    public:
        static void setup();

        template<bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t lootTableSeed);
    };

    void SpawnBonusChest::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;
        std::vector<ItemEntry> items4;
        std::vector<ItemEntry> items5;
        std::vector<ItemEntry> items6;
        std::vector<ItemEntry> items7;
        std::vector<ItemEntry> items8;

        // table 1
        items1.emplace_back(&JUNGLE_SAPLING,       1, 1, 5);
        lootTables.emplace_back(items1,           1, 1);

        // table 2
        items2.emplace_back(&MELON_SEEDS,          1, 1, 5);
        lootTables.emplace_back(items2,           1, 1);

        // table 3
        items3.emplace_back(&PUMPKIN_SEEDS,        1, 1, 5);
        lootTables.emplace_back(items3,           1, 1);

        // table 4
        items4.emplace_back(&BEETROOT_SEEDS,       1, 1, 5);
        lootTables.emplace_back(items4,           1, 1);

        // table 5
        items5.emplace_back(&STONE_AXE,            1);
        items5.emplace_back(&WOODEN_AXE,           3);
        lootTables.emplace_back(items5,           1, 4);

        // table 6
        items6.emplace_back(&STONE_PICKAXE,        1);
        items6.emplace_back(&WOODEN_PICKAXE,       3);
        lootTables.emplace_back(items6,           1, 4);

        // table 7
        items7.emplace_back(&APPLE,                5, 1, 2);
        items7.emplace_back(&BREAD,                3, 1, 2);
        items7.emplace_back(&RAW_SALMON,           3, 1, 2);
        lootTables.emplace_back(items7,           3, 11);

        // table 8
        items8.emplace_back(&STICK,                10, 1, 8);
        items8.emplace_back(&OAK_WOOD_PLANK,       10, 1, 12);
        items8.emplace_back(&OAK_WOOD,             10, 1, 3); // another roll of setData 0-3
        items8.emplace_back(&ACACIA_WOOD,          10, 1, 3);// another roll of setData 0-1
        lootTables.emplace_back(items8,           4, 40);

        maxItemsPossible = 9;
    }



    template <bool shuffle>
    Container SpawnBonusChest::getLootFromLootTableSeed(uint64_t lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(&lootTableSeed, lootTableSeed);

        // generate loot
        for(const LootTable& table : loot_tables::SpawnBonusChest::lootTables){
            rollCount = LootTable::getInt<false>(&lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(&lootTableSeed);

                if EXPECT_FALSE(result.item == &Items::ACACIA_WOOD) {
                    nextInt(&lootTableSeed, 0, 1);
                }
                else if EXPECT_FALSE(result.item == &Items::OAK_WOOD) {
                    nextInt(&lootTableSeed, 0, 3);
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

