#pragma once

#include "loot/classes/Loot.hpp"


namespace loot {
    class ShipwreckMap : public Loot<ShipwreckMap> {
    public:
        static void setup();
    };

    inline void ShipwreckMap::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&lce::items::TREASURE_MAP, 1);
        lootTables.emplace_back(items1, 1);

        items2.emplace_back(&lce::items::COMPASS, 1);
        items2.emplace_back(&lce::items::MAP, 1);
        items2.emplace_back(&lce::items::CLOCK, 1);
        items2.emplace_back(&lce::items::PAPER, 20, 1, 10);
        items2.emplace_back(&lce::items::FEATHER, 10, 1, 5);
        items2.emplace_back(&lce::items::BOOK, 5, 1, 5);
        lootTables.emplace_back(items2, 3);
    }
}
