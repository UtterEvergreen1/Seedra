#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"

namespace loot {
    class DesertTemple : public Loot<DesertTemple> {
    public:
        static void setup();
    };

    void DesertTemple::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&Items::DIAMOND,                                    5, 1, 3);
        items1.emplace_back(&Items::IRON_INGOT,                                 15, 1, 5);
        items1.emplace_back(&Items::GOLD_INGOT,                                 15, 2, 7);
        items1.emplace_back(&Items::EMERALD,                                    15, 1, 3);
        items1.emplace_back(&Items::BONE,                                       25, 4, 6);
        items1.emplace_back(&Items::SPIDER_EYE,                                 25, 1, 3);
        items1.emplace_back(&Items::ROTTEN_FLESH,                               25, 3, 7);
        items1.emplace_back(&Items::SADDLE,                                     20);
        items1.emplace_back(&Items::IRON_HORSE_ARMOR,                           15);
        items1.emplace_back(&Items::GOLDEN_HORSE_ARMOR,                         10);
        items1.emplace_back(&Items::DIAMOND_HORSE_ARMOR,                        5);
        items1.emplace_back(&Items::ENCHANTED_BOOK, new EnchantRandomlyBook(),  20);
        items1.emplace_back(&Items::GOLDEN_APPLE,                               20);
        items1.emplace_back(&Items::ENCHANTED_GOLDEN_APPLE,                     2);
        items1.emplace_back(&Items::AIR,                                        15);
        lootTables.emplace_back(items1,                                         2, 4);

        // table 2
        items2.emplace_back(&Items::BONE,                                       10, 1, 8);
        items2.emplace_back(&Items::GUNPOWDER,                                  10, 1, 8);
        items2.emplace_back(&Items::ROTTEN_FLESH,                               10, 1, 8);
        items2.emplace_back(&Items::STRING,                                     10, 1, 8);
        items2.emplace_back(&Items::SAND,                                       10, 1, 8);
        lootTables.emplace_back(items2,                                         4);

        maxItemsPossible = 8;
    }
}

