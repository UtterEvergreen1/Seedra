#pragma once

#include "loot/classes/Loot.hpp"

namespace loot {
    class Igloo : public Loot<Igloo> {
    public:
        static void setup();
    };

    inline void Igloo::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&lce::items::APPLE, 15, 1, 3);
        items1.emplace_back(&lce::items::COAL, 15, 1, 4);
        items1.emplace_back(&lce::items::GOLD_NUGGET, 10, 1, 3);
        items1.emplace_back(&lce::items::STONE_AXE, 2);
        items1.emplace_back(&lce::items::ROTTEN_FLESH, 10);
        items1.emplace_back(&lce::items::EMERALD, 1);
        items1.emplace_back(&lce::items::WHEAT, 10, 2, 3);
        lootTables.emplace_back(items1, 2, 8);

        // table 2
        items2.emplace_back(&lce::items::GOLDEN_APPLE, 1);
        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;
    }
} // namespace loot
