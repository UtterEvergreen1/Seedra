#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class ShipwreckMap : public Loot<ShipwreckMap> {
    public:
        static void setup();
    };

    void ShipwreckMap::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&TREASURE_MAP,  1);
        lootTables.emplace_back(items1,     1);

        items2.emplace_back(&COMPASS,     1);
        items2.emplace_back(&MAP,         1);
        items2.emplace_back(&CLOCK,       1);
        items2.emplace_back(&PAPER,      20, 1, 10);
        items2.emplace_back(&FEATHER,    10, 1,  5);
        items2.emplace_back(&BOOK,        5, 1,  5);
        lootTables.emplace_back(items2,   3);


    }
}