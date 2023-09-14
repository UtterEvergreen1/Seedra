#pragma once

#include "base_classes/loot.hpp"

using namespace Items;


namespace loot_tables {
    class SpawnBonusChest : public Loot<SpawnBonusChest> {
    public:
        static void setup();
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
        std::vector<ItemEntry> items9;
        std::vector<ItemEntry> items10;
        std::vector<ItemEntry> items11;
        std::vector<ItemEntry> items12;
        std::vector<ItemEntry> items13;
        std::vector<ItemEntry> items14;

        // table 1
        items1.emplace_back(&STONE_AXE,               1);
        items1.emplace_back(&WOODEN_AXE,              3);
        lootTables.emplace_back(items1,            1, 4);

        // table 2
        items2.emplace_back(&STONE_PICKAXE,           1);
        items2.emplace_back(&WOODEN_PICKAXE,          3);
        lootTables.emplace_back(items2,            1, 4);

        // table 3
        items3.emplace_back(&APPLE,             1, 0, 3);
        lootTables.emplace_back(items3,           1, 1);

        // table 4
        items4.emplace_back(&BREAD,             1, 0, 2);
        lootTables.emplace_back(items4,           1, 1);

        // table 5
        items5.emplace_back(&RAW_SALMON,             1, 0, 2);
        lootTables.emplace_back(items5,           1, 1);

        // table 6
        items6.emplace_back(&STICK,             1, 1, 8);
        lootTables.emplace_back(items6,           1, 1);

        // table 7
        items7.emplace_back(&OAK_WOOD_PLANK,             1, 1, 8);
        lootTables.emplace_back(items7,           1, 1);

        // table 8
        items8.emplace_back(&ACACIA_WOOD,             1, 0, 3);
        items8.emplace_back(&DARK_OAK_WOOD,             1, 0, 3);
        lootTables.emplace_back(items8,           1, 2);

        // table 9
        items9.emplace_back(&OAK_WOOD,             1, 1, 3);
        items9.emplace_back(&SPRUCE_WOOD,             1, 1, 3);
        items9.emplace_back(&BIRCH_WOOD,             1, 1, 3);
        items9.emplace_back(&JUNGLE_WOOD,             1, 1, 3);
        lootTables.emplace_back(items9,           1, 4);

        // table 10
        items10.emplace_back(&POTATO,             3, 1, 2);
        items10.emplace_back(&CARROT,        3, 1, 2);
        lootTables.emplace_back(items10,           1, 6);

        // table 11
        items11.emplace_back(&JUNGLE_SAPLING,             1, 1, 5);
        lootTables.emplace_back(items11,           1, 1);

        // table 12
        items12.emplace_back(&MELON_SEEDS,             3, 0, 2);
        items12.emplace_back(&PUMPKIN_SEEDS,             3, 0, 2);
        items12.emplace_back(&BEETROOT_SEEDS,             3, 0, 2);
        lootTables.emplace_back(items12,           1, 9);

        // table 13
        items13.emplace_back(&CACTUS,             3, 0, 2);
        items13.emplace_back(&COCOA_BEANS,             2, 0, 2);
        lootTables.emplace_back(items13,           1, 5);

        // table 14
        items14.emplace_back(&BROWN_MUSHROOM,             2, 0, 2);
        lootTables.emplace_back(items14,           1, 2);

        //java
        /*// table 3
        items3.emplace_back(&APPLE,             5, 1, 2);
        items3.emplace_back(&BREAD,             3, 1, 2);
        items3.emplace_back(&RAW_SALMON,        3, 1, 2);
        lootTables.emplace_back(items3,           3, 11);

        // table 4
        items4.emplace_back(&STICK,           10, 1, 12);
        items4.emplace_back(&OAK_WOOD_PLANK,  10, 1, 12);
        items4.emplace_back(&OAK_WOOD,         10, 1, 3); // CAN BE dataTag min0 max3
        items4.emplace_back(&ACACIA_WOOD,      10, 1, 3); // CAN BE dataTag min0 max1
        lootTables.emplace_back(items4,           4, 40);*/

        maxItemsPossible = 9;
    }

}

