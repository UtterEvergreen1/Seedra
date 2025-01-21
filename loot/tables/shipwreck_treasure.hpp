#pragma once

#include "loot/classes/Loot.hpp"


namespace loot {
    class ShipwreckTreasure : public Loot<ShipwreckTreasure> {
    public:
        static void setup();
    };

    inline void ShipwreckTreasure::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&lce::items::IRON_INGOT, 90, 1, 5);
        items1.emplace_back(&lce::items::GOLD_INGOT, 10, 1, 5);
        items1.emplace_back(&lce::items::EMERALD, 40, 1, 5);
        items1.emplace_back(&lce::items::DIAMOND, 5);
        items1.emplace_back(&lce::items::BOTTLE_O_ENCHANTING, 5);
        lootTables.emplace_back(items1, 3, 6);

        items2.emplace_back(&lce::items::IRON_NUGGET, 50, 1, 10);
        items2.emplace_back(&lce::items::GOLD_NUGGET, 10, 1, 10);
        items2.emplace_back(&lce::items::LAPIS_LAZULI, 20, 1, 10);
        lootTables.emplace_back(items2, 2, 5);

        maxItemsPossible = 11;
    }
}
