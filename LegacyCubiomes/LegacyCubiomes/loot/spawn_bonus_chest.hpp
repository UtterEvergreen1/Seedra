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

        // table 1
        items1.emplace_back(&STONE_AXE,               1);
        items1.emplace_back(&WOODEN_AXE,              3);
        lootTables.emplace_back(items1,            1, 4);

        // table 2
        items2.emplace_back(&STONE_PICKAXE,           1);
        items2.emplace_back(&WOODEN_PICKAXE,          3);
        lootTables.emplace_back(items2,            1, 4);

        // table 3
        items3.emplace_back(&APPLE,             5, 1, 2);
        items3.emplace_back(&BREAD,             3, 1, 2);
        items3.emplace_back(&RAW_SALMON,        3, 1, 2);
        lootTables.emplace_back(items3,           3, 11);

        // table 4
        items4.emplace_back(&STICK,           10, 1, 12);
        items4.emplace_back(&OAK_WOOD_PLANK,  10, 1, 12);
        items4.emplace_back(&OAK_WOOD,         10, 1, 3); // CAN BE dataTag min0 max3
        items4.emplace_back(&ACACIA_WOOD,      10, 1, 3); // CAN BE dataTag min0 max1
        lootTables.emplace_back(items4,           4, 40);

        maxItemsPossible = 9;
    }

}

