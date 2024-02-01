#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"


namespace loot {
    class ShipwreckMap : public Loot<ShipwreckMap> {
    public:
        static void setup();
    };

    inline void ShipwreckMap::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&Items::TREASURE_MAP, 1);
        lootTables.emplace_back(items1, 1);

        items2.emplace_back(&Items::COMPASS, 1);
        items2.emplace_back(&Items::MAP, 1);
        items2.emplace_back(&Items::CLOCK, 1);
        items2.emplace_back(&Items::PAPER, 20, 1, 10);
        items2.emplace_back(&Items::FEATHER, 10, 1, 5);
        items2.emplace_back(&Items::BOOK, 5, 1, 5);
        lootTables.emplace_back(items2, 3);
    }
}
