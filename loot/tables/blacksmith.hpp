#pragma once

#include "loot/classes/Loot.hpp"


namespace loot {
    class Blacksmith : public Loot<Blacksmith> {
    public:
        static void setup();
    };

    inline void Blacksmith::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::DIAMOND, 3, 1, 3);
        items.emplace_back(&lce::items::IRON_INGOT, 10, 1, 5);
        items.emplace_back(&lce::items::GOLD_INGOT, 5, 1, 3);
        items.emplace_back(&lce::items::BREAD, 15, 1, 3);
        items.emplace_back(&lce::items::APPLE, 15, 1, 3);
        items.emplace_back(&lce::items::IRON_PICKAXE, 5);
        items.emplace_back(&lce::items::IRON_SWORD, 5);
        items.emplace_back(&lce::items::IRON_CHESTPLATE, 5);
        items.emplace_back(&lce::items::IRON_HELMET, 5);
        items.emplace_back(&lce::items::IRON_LEGGINGS, 5);
        items.emplace_back(&lce::items::IRON_BOOTS, 5);
        items.emplace_back(&lce::items::OBSIDIAN, 5, 3, 7);
        items.emplace_back(&lce::items::OAK_SAPLING, 5, 3, 7);
        items.emplace_back(&lce::items::SADDLE, 3);
        items.emplace_back(&lce::items::IRON_HORSE_ARMOR, 1);
        items.emplace_back(&lce::items::GOLDEN_HORSE_ARMOR, 1);
        items.emplace_back(&lce::items::DIAMOND_HORSE_ARMOR, 1);
        lootTables.emplace_back(items, 3, 8);

        maxItemsPossible = 8;
    }
}
