#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class JungleTemple : public Loot<JungleTemple> {
    public:
        static void setup();
    };

    void JungleTemple::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&DIAMOND,              3, 1, 3);
        items.emplace_back(&IRON_INGOT,          10, 1, 5);
        items.emplace_back(&GOLD_INGOT,          15, 2, 7);
        items.emplace_back(&EMERALD,              2, 1, 3);
        items.emplace_back(&BONE,                20, 4, 6);
        items.emplace_back(&ROTTEN_FLESH,        16, 3, 7);
        items.emplace_back(&SADDLE,                     3);
        items.emplace_back(&IRON_HORSE_ARMOR,           1);
        items.emplace_back(&GOLDEN_HORSE_ARMOR,         1);
        items.emplace_back(&DIAMOND_HORSE_ARMOR,        1);
        items.emplace_back(&ENCHANTED_BOOK,             1); // function=enchant_with_levels, levels=30, treasure=true
        lootTables.emplace_back(items,           2, 6, 73);

        maxItemsPossible = 6;
    }
}



