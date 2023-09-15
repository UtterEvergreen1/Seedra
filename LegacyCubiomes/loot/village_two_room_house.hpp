#pragma once


#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class VillageTwoRoomHouse : public Loot<VillageTwoRoomHouse> {
    public:
        static void setup();
    };

    void VillageTwoRoomHouse::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&POTATO,              10, 5, 8);
        items.emplace_back(&CARROT,              10, 4, 8);
        items.emplace_back(&WHEAT,               15, 8, 12);
        items.emplace_back(&WHEAT_SEEDS,         5, 2, 4);
        items.emplace_back(&BEETROOT,            5, 5, 8);
        items.emplace_back(&WOODEN_HOE,          1);
        lootTables.emplace_back(items,                6, 8, 46);

        maxItemsPossible = 6;
    }
}

