#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"

namespace loot {
    class Blacksmith : public Loot<Blacksmith> {
    public:
        static void setup();
    };

    void Blacksmith::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::DIAMOND, 3, 1, 3);
        items.emplace_back(&Items::IRON_INGOT, 10, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT, 5, 1, 3);
        items.emplace_back(&Items::BREAD, 15, 1, 3);
        items.emplace_back(&Items::APPLE, 15, 1, 3);
        items.emplace_back(&Items::IRON_PICKAXE, 5);
        items.emplace_back(&Items::IRON_SWORD, 5);
        items.emplace_back(&Items::IRON_CHESTPLATE, 5);
        items.emplace_back(&Items::IRON_HELMET, 5);
        items.emplace_back(&Items::IRON_LEGGINGS, 5);
        items.emplace_back(&Items::IRON_BOOTS, 5);
        items.emplace_back(&Items::OBSIDIAN, 5, 3, 7);
        items.emplace_back(&Items::OAK_SAPLING, 5, 3, 7);
        items.emplace_back(&Items::SADDLE, 3);
        items.emplace_back(&Items::IRON_HORSE_ARMOR, 1);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR, 1);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR, 1);
        lootTables.emplace_back(items, 3, 8);

        maxItemsPossible = 8;
    }
} // namespace loot
