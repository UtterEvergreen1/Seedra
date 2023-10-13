#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"

namespace loot {
    class ShipwreckSupply : public Loot<ShipwreckSupply> {
    public:
        static void setup();
    };

    void ShipwreckSupply::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::PAPER,                                       8, 1, 12);
        items.emplace_back(&Items::POTATO,                                      7, 2, 6);
        items.emplace_back(&Items::POISONOUS_POTATO,                            7, 2, 6);
        items.emplace_back(&Items::CARROT,                                      7, 4, 8);
        items.emplace_back(&Items::WHEAT,                                       7, 8, 21);
        items.emplace_back(&Items::COAL,                                        6, 2, 8);
        items.emplace_back(&Items::ROTTEN_FLESH,                                5, 5, 24);
        items.emplace_back(&Items::PUMPKIN,                                     2, 1, 3);
        items.emplace_back(&Items::GUNPOWDER,                                   3, 1, 5);
        items.emplace_back(&Items::TNT,                                         1, 1, 2);
        items.emplace_back(&Items::LEATHER_HELMET, new EnchantRandomlyItem(),   3);
        items.emplace_back(&Items::LEATHER_TUNIC,  new EnchantRandomlyItem(),   3);
        items.emplace_back(&Items::LEATHER_PANTS,  new EnchantRandomlyItem(),   3);
        items.emplace_back(&Items::LEATHER_BOOTS,  new EnchantRandomlyItem(),   3);

        lootTables.emplace_back(items,                                          3, 10);

        maxItemsPossible = 10;
    }
}

