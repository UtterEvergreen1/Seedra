#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class Mansion : public Loot<Mansion> {
    public:
        static void setup();
    };

    void Mansion::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&LEAD,                        100);
        items1.emplace_back(&GOLDEN_APPLE,                75);
        items1.emplace_back(&ENCHANTED_GOLDEN_APPLE,      10);
        items1.emplace_back(&DISC_13,                     75);
        items1.emplace_back(&DISC_CAT,                    75);
        items1.emplace_back(&NAME_TAG,                    100);
        items1.emplace_back(&CHAINMAIL_CHESTPLATE,        50);
        items1.emplace_back(&DIAMOND_HOE,                 75);
        items1.emplace_back(&DIAMOND_CHESTPLATE,          25);
        items1.emplace_back(&ENCHANTED_BOOK,              60); // function=enchant_randomly
        lootTables.emplace_back(items1,                  1, 3, 645);

        // table 2
        items2.emplace_back(&IRON_INGOT,                  50, 1, 4);
        items2.emplace_back(&GOLD_INGOT,                  25, 1, 4);
        items2.emplace_back(&BREAD,                       100);
        items2.emplace_back(&WHEAT,                       100, 1, 4);
        items2.emplace_back(&BUCKET,                      50);
        items2.emplace_back(&REDSTONE,                    75, 1, 4);
        items2.emplace_back(&COAL,                        75, 1, 4);
        items2.emplace_back(&MELON_SEEDS,                 50, 2, 4);
        items2.emplace_back(&PUMPKIN_SEEDS,               50, 2, 4);
        items2.emplace_back(&BEETROOT_SEEDS,              50, 2, 4);
        lootTables.emplace_back(items2,                  1, 4, 625);

        // table 3
        items3.emplace_back(&BONE,                        50, 1, 8);
        items3.emplace_back(&GUNPOWDER,                   50, 1, 8);
        items3.emplace_back(&ROTTEN_FLESH,                50, 1, 8);
        items3.emplace_back(&STRING,                      50, 1, 8);
        lootTables.emplace_back(items3,                  3, 200);

        maxItemsPossible = 10;
    }
}

