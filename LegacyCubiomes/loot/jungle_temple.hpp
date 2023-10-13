#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"

namespace loot {
    class JungleTemple : public Loot<JungleTemple> {
    public:
        static void setup();
    };

    void JungleTemple::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::EMERALD,                                         15, 1, 3);
        items.emplace_back(&Items::IRON_INGOT,                                      50, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT,                                      75, 2, 7);
        items.emplace_back(&Items::EMERALD,                                         10, 1, 3);
        items.emplace_back(&Items::BONE,                                            100, 4, 6);
        items.emplace_back(&Items::ROTTEN_FLESH,                                    80, 3, 7);
        items.emplace_back(&Items::SADDLE,                                          15);
        items.emplace_back(&Items::IRON_HORSE_ARMOR,                                5);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR,                              5);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR,                             5);
        items.emplace_back(&Items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30),   6);

        lootTables.emplace_back(items,                                              2, 6);

        maxItemsPossible = 6;
    }
}

