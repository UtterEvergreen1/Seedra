#pragma once

#include "loot/classes/Loot.hpp"


namespace loot {
    class VillageTwoRoomHouse : public Loot<VillageTwoRoomHouse> {
    public:
        static void setup();
    };

    inline void VillageTwoRoomHouse::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::POTATO, 10, 5, 8);
        items.emplace_back(&lce::items::CARROT, 10, 4, 8);
        items.emplace_back(&lce::items::WHEAT, 15, 8, 12);
        items.emplace_back(&lce::items::WHEAT_SEEDS, 5, 2, 4);
        items.emplace_back(&lce::items::BEETROOT, 5, 5, 8);
        items.emplace_back(&lce::items::WOODEN_HOE, 1);
        lootTables.emplace_back(items, 6, 8);

        maxItemsPossible = 8;
    }
}
