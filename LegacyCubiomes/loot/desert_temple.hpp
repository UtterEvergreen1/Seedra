#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class DesertTemple : public Loot<DesertTemple> {
    public:
        static void setup();
    };

    void DesertTemple::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&DIAMOND,                 5, 1, 3);
        items1.emplace_back(&IRON_INGOT,              15, 1, 5);
        items1.emplace_back(&GOLD_INGOT,              15, 2, 7);
        items1.emplace_back(&EMERALD,                 15, 1, 3);
        items1.emplace_back(&BONE,                    25, 4, 6);
        items1.emplace_back(&SPIDER_EYE,              25, 1, 3);
        items1.emplace_back(&ROTTEN_FLESH,            25, 3, 7);
        items1.emplace_back(&SADDLE,                  20);
        items1.emplace_back(&IRON_HORSE_ARMOR,        15);
        items1.emplace_back(&GOLDEN_HORSE_ARMOR,      10);
        items1.emplace_back(&DIAMOND_HORSE_ARMOR,     5);
        items1.emplace_back(&ENCHANTED_BOOK,          20); // function=enchant_randomly
        items1.emplace_back(&GOLDEN_APPLE,            20);
        items1.emplace_back(&ENCHANTED_GOLDEN_APPLE,  2);
        items1.emplace_back(&AIR,                     15);
        lootTables.emplace_back(items1,              2, 4, 232);

        // table 2
        items2.emplace_back(&BONE,                    10, 1, 8);
        items2.emplace_back(&GUNPOWDER,               10, 1, 8);
        items2.emplace_back(&ROTTEN_FLESH,            10, 1, 8);
        items2.emplace_back(&STRING,                  10, 1, 8);
        items2.emplace_back(&SAND,                    10, 1, 8);
        lootTables.emplace_back(items2,              4, 50);

        maxItemsPossible = 8;
    }
}

