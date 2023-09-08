#pragma once

#include "base_classes/stronghold_loot.hpp"

using namespace Items;

namespace loot_tables {
    class StrongholdLibrary : public StrongholdLoot {
    public:
        static void setup();
    };

    void StrongholdLibrary::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&BOOK,             100, 1, 3);
        items.emplace_back(&PAPER,            100, 2, 7);
        items.emplace_back(&MAP,                     5);
        items.emplace_back(&COMPASS,                 5);
        items.emplace_back(&ENCHANTED_BOOK,         60); // function=enchant_with_levels, levels=30, treasure=true
        lootTables.emplace_back(items,       2, 10, 270);

        maxItemsPossible = 10;
    }

}

