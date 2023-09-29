#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"

namespace loot_tables {
    class ShipwreckTreasure : public Loot<ShipwreckTreasure> {
    public:
        static void setup();
    };

    void ShipwreckTreasure::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&Items::IRON_INGOT,          90, 1, 5);
        items1.emplace_back(&Items::GOLD_INGOT,          10, 1, 5);
        items1.emplace_back(&Items::EMERALD,             40, 1, 5);
        items1.emplace_back(&Items::DIAMOND,             5);
        items1.emplace_back(&Items::BOTTLE_O_ENCHANTING, 5);
        lootTables.emplace_back(items1, 3, 6);

        items2.emplace_back(&Items::IRON_NUGGET,         50, 1, 10);
        items2.emplace_back(&Items::GOLD_NUGGET,         10, 1, 10);
        items2.emplace_back(&Items::LAPIS_LAZULI,        20, 1, 10);
        lootTables.emplace_back(items2, 2, 5);

        maxItemsPossible = 11;

    }
}