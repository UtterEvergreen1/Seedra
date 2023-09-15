#pragma once

#include "base_classes/stronghold_loot.hpp"

using namespace Items;


namespace loot_tables {
    class StrongholdCrossing : public StrongholdLoot {
    public:
        static void setup();
    };

    void StrongholdCrossing::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&IRON_INGOT,       50, 1, 5);
        items.emplace_back(&GOLD_INGOT,       25, 1, 3);
        items.emplace_back(&REDSTONE,         25, 4, 9);
        items.emplace_back(&COAL,             50, 3, 8);
        items.emplace_back(&BREAD,            75, 1, 3);
        items.emplace_back(&APPLE,            75, 1, 3);
        items.emplace_back(&IRON_PICKAXE,     5);
        items.emplace_back(&ENCHANTED_BOOK,   6); // function=enchant_with_levels, levels=30, treasure=true
        items.emplace_back(&INK_SACK,         75, 1, 3);
        lootTables.emplace_back(items,             1, 4, 386);

        maxItemsPossible = 4;
    }
}

