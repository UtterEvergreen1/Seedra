#pragma once

#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"
#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"

namespace loot {
    class StrongholdCrossing : public StrongholdLoot<StrongholdCrossing> {
    public:
        static void setup();
    };

    void StrongholdCrossing::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::IRON_INGOT,                                      50, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT,                                      25, 1, 3);
        items.emplace_back(&Items::REDSTONE,                                        25, 4, 9);
        items.emplace_back(&Items::COAL,                                            50, 3, 8);
        items.emplace_back(&Items::BREAD,                                           75, 1, 3);
        items.emplace_back(&Items::APPLE,                                           75, 1, 3);
        items.emplace_back(&Items::IRON_PICKAXE,                                    5);
        items.emplace_back(&Items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30),   6);
        items.emplace_back(&Items::INK_SACK,                                        75, 1, 3);

        lootTables.emplace_back(items,                                              1, 4);

        maxItemsPossible = 4;
    }
}

