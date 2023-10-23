#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"

namespace loot {
    class VillageTwoRoomHouse : public Loot<VillageTwoRoomHouse> {
    public:
        static void setup();
    };

    void VillageTwoRoomHouse::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::POTATO, 10, 5, 8);
        items.emplace_back(&Items::CARROT, 10, 4, 8);
        items.emplace_back(&Items::WHEAT, 15, 8, 12);
        items.emplace_back(&Items::WHEAT_SEEDS, 5, 2, 4);
        items.emplace_back(&Items::BEETROOT, 5, 5, 8);
        items.emplace_back(&Items::WOODEN_HOE, 1);
        lootTables.emplace_back(items, 6, 8);

        maxItemsPossible = 8;
    }
} // namespace loot
