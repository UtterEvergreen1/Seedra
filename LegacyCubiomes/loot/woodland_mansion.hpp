#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"

namespace loot {
    class Mansion : public Loot<Mansion> {
    public:
        static void setup();
    };

    void Mansion::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&Items::LEAD,                                       100);
        items1.emplace_back(&Items::GOLDEN_APPLE,                               75);
        items1.emplace_back(&Items::ENCHANTED_GOLDEN_APPLE,                     10);
        items1.emplace_back(&Items::DISC_13,                                    15);
        items1.emplace_back(&Items::DISC_CAT,                                   15);
        items1.emplace_back(&Items::NAME_TAG,                                   100);
        items1.emplace_back(&Items::CHAINMAIL_CHESTPLATE,                       50);
        items1.emplace_back(&Items::DIAMOND_HOE,                                75);
        items1.emplace_back(&Items::DIAMOND_CHESTPLATE,                         25);
        items1.emplace_back(&Items::ENCHANTED_BOOK, new EnchantRandomlyBook(),  60);
        lootTables.emplace_back(items1,                                         1, 3);

        // table 2
        items2.emplace_back(&Items::IRON_INGOT,                                 50, 1, 4);
        items2.emplace_back(&Items::GOLD_INGOT,                                 25, 1, 4);
        items2.emplace_back(&Items::BREAD,                                      100);
        items2.emplace_back(&Items::WHEAT,                                      100, 1, 4);
        items2.emplace_back(&Items::BUCKET,                                     50);
        items2.emplace_back(&Items::REDSTONE,                                   75, 1, 4);
        items2.emplace_back(&Items::COAL,                                       75, 1, 4);
        items2.emplace_back(&Items::MELON_SEEDS,                                50, 2, 4);
        items2.emplace_back(&Items::PUMPKIN_SEEDS,                              50, 2, 4);
        items2.emplace_back(&Items::BEETROOT_SEEDS,                             50, 2, 4);
        lootTables.emplace_back(items2,                                         1, 4);

        // table 3
        items3.emplace_back(&Items::BONE,                                       50, 1, 8);
        items3.emplace_back(&Items::GUNPOWDER,                                  50, 1, 8);
        items3.emplace_back(&Items::ROTTEN_FLESH,                               50, 1, 8);
        items3.emplace_back(&Items::STRING,                                     50, 1, 8);
        lootTables.emplace_back(items3,                                         3);

        maxItemsPossible = 10;
    }
}

