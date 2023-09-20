#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class UnderwaterRuinSmall : public Loot<UnderwaterRuinSmall> {
    public:
        static void setup();
    };

    void UnderwaterRuinSmall::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&COAL,         10, 1, 4);
        items1.emplace_back(&STONE_AXE,     2);
        items1.emplace_back(&ROTTEN_FLESH,  5);
        items1.emplace_back(&EMERALD,       1);
        items1.emplace_back(&WHEAT,        10, 2, 3);
        lootTables.emplace_back(items1, 2, 8);

        // TODO: has nautilus shell in console
        items2.emplace_back(&LEATHER_TUNIC, 1);
        items2.emplace_back(&GOLDEN_HELMET, 1);
        items2.emplace_back(&FISHING_ROD,   5); // enchant_randomly
        items2.emplace_back(&TREASURE_MAP,  5); // zoom=1
        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;
    }
}