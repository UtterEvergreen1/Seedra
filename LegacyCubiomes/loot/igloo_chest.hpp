#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class Igloo : public Loot<Igloo> {
    public:
        static void setup();
    };

    void Igloo::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&APPLE,        15, 1, 3);
        items1.emplace_back(&COAL,         15, 1, 4);
        items1.emplace_back(&GOLD_NUGGET,  10, 1, 3);
        items1.emplace_back(&STONE_AXE,    2);
        items1.emplace_back(&ROTTEN_FLESH, 10);
        items1.emplace_back(&EMERALD,      1);
        items1.emplace_back(&WHEAT,        10, 2, 3);
        lootTables.emplace_back(items1,    2, 8);

        // table 2
        items2.emplace_back(&GOLDEN_APPLE, 1);
        lootTables.emplace_back(items2,   1);

        maxItemsPossible = 9;
    }
}

