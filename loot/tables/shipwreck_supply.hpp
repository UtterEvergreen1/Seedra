#pragma once

#include "loot/classes/EnchantRandomly.hpp"
#include "loot/classes/Loot.hpp"


namespace loot {
    class ShipwreckSupply : public Loot<ShipwreckSupply> {
    public:
        static void setup();
    };

    inline void ShipwreckSupply::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::PAPER, 8, 1, 12);
        items.emplace_back(&lce::items::POTATO, 7, 2, 6);
        items.emplace_back(&lce::items::POISONOUS_POTATO, 7, 2, 6);
        items.emplace_back(&lce::items::CARROT, 7, 4, 8);
        items.emplace_back(&lce::items::WHEAT, 7, 8, 21);
        items.emplace_back(&lce::items::COAL, 6, 2, 8);
        items.emplace_back(&lce::items::ROTTEN_FLESH, 5, 5, 24);
        items.emplace_back(&lce::items::PUMPKIN, 2, 1, 3);
        items.emplace_back(&lce::items::GUNPOWDER, 3, 1, 5);
        items.emplace_back(&lce::items::TNT, 1, 1, 2);
        items.emplace_back(&lce::items::LEATHER_HELMET, new EnchantRandomlyItem(), 3);
        items.emplace_back(&lce::items::LEATHER_TUNIC, new EnchantRandomlyItem(), 3);
        items.emplace_back(&lce::items::LEATHER_PANTS, new EnchantRandomlyItem(), 3);
        items.emplace_back(&lce::items::LEATHER_BOOTS, new EnchantRandomlyItem(), 3);

        lootTables.emplace_back(items, 3, 10);

        maxItemsPossible = 10;
    }
}
