#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"

namespace loot {
    class UnderwaterRuinSmall : public Loot<UnderwaterRuinSmall> {
    public:
        static void setup();
    };

    void UnderwaterRuinSmall::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&Items::COAL,                                       10, 1, 4);
        items1.emplace_back(&Items::STONE_AXE,                                  2);
        items1.emplace_back(&Items::ROTTEN_FLESH,                               5);
        items1.emplace_back(&Items::EMERALD,                                    1);
        items1.emplace_back(&Items::WHEAT,                                      10, 2, 3);
        lootTables.emplace_back(items1,                                         2, 8);

        items2.emplace_back(&Items::LEATHER_TUNIC,                              1);
        items2.emplace_back(&Items::GOLDEN_HELMET,                              1);
        items2.emplace_back(&Items::FISHING_ROD, new EnchantRandomlyItem(),     5);
        items2.emplace_back(&Items::TREASURE_MAP,                               5);

        lootTables.emplace_back(items2,                                         1);

        maxItemsPossible = 9;
    }
}

