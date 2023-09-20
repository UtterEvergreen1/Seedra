#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class ShipwreckTreasure : public Loot<ShipwreckTreasure> {
    public:
        static void setup();
    };

    void ShipwreckTreasure::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&IRON_INGOT,          90, 1, 5);
        items1.emplace_back(&GOLD_INGOT,          10, 1, 5);
        items1.emplace_back(&EMERALD,             40, 1, 5);
        items1.emplace_back(&DIAMOND,             5);
        items1.emplace_back(&BOTTLE_O_ENCHANTING, 5);
        lootTables.emplace_back(items1, 3, 6);

        items2.emplace_back(&IRON_NUGGET,         50, 1, 10);
        items2.emplace_back(&GOLD_NUGGET,         10, 1, 10);
        items2.emplace_back(&LAPIS_LAZULI,        20, 1, 10);
        lootTables.emplace_back(items2, 2, 5);

        maxItemsPossible = 11;

    }
}