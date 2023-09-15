#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class SimpleDungeon : public Loot<SimpleDungeon> {
    public:
        static void setup();
    };

    void SimpleDungeon::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&SADDLE,                      20);
        items1.emplace_back(&GOLDEN_APPLE,                15);
        items1.emplace_back(&ENCHANTED_GOLDEN_APPLE,      2);
        items1.emplace_back(&DISC_13,                     15);
        items1.emplace_back(&DISC_CAT,                    15);
        items1.emplace_back(&NAME_TAG,                    20);
        items1.emplace_back(&GOLDEN_HORSE_ARMOR,          10);
        items1.emplace_back(&IRON_HORSE_ARMOR,            15);
        items1.emplace_back(&DIAMOND_HORSE_ARMOR,         5);
        items1.emplace_back(&ENCHANTED_BOOK,              10); // function=enchant_randomly
        lootTables.emplace_back(items1,                  1, 3, 127);

        // table 2
        items2.emplace_back(&IRON_INGOT,                  10, 1, 4);
        items2.emplace_back(&GOLD_INGOT,                  5, 1, 4);
        items2.emplace_back(&BREAD,                       20);
        items2.emplace_back(&WHEAT,                       20, 1, 4);
        items2.emplace_back(&BUCKET,                      10);
        items2.emplace_back(&REDSTONE,                    15, 1, 4);
        items2.emplace_back(&COAL,                        15, 1, 4);
        items2.emplace_back(&MELON_SEEDS,                 10, 2, 4);
        items2.emplace_back(&PUMPKIN_SEEDS,               10, 2, 4);
        items2.emplace_back(&BEETROOT_SEEDS,              10, 2, 4);
        lootTables.emplace_back(items2,                  1, 4, 125);

        // table 3
        items3.emplace_back(&BONE,                        10, 1, 8);
        items3.emplace_back(&GUNPOWDER,                   10, 1, 8);
        items3.emplace_back(&ROTTEN_FLESH,                10, 1, 8);
        items3.emplace_back(&STRING,                      10, 1, 8);
        lootTables.emplace_back(items3,                    3, 40);

        maxItemsPossible = 10;
    }
}

