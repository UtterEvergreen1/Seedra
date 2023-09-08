#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {

    class Blacksmith : public Loot<Blacksmith> {
        public:
            static void setup();
    };

        void Blacksmith::setup() {
            std::vector<ItemEntry> items;

            items.emplace_back(&DIAMOND,               3, 1, 3);
            items.emplace_back(&IRON_INGOT,           10, 1, 5);
            items.emplace_back(&GOLD_INGOT,            5, 1, 3);
            items.emplace_back(&BREAD,                15, 1, 3);
            items.emplace_back(&APPLE,                15, 1, 3);
            items.emplace_back(&IRON_PICKAXE,                5);
            items.emplace_back(&IRON_SWORD,                  5);
            items.emplace_back(&IRON_CHESTPLATE,             5);
            items.emplace_back(&IRON_HELMET,                 5);
            items.emplace_back(&IRON_LEGGINGS,               5);
            items.emplace_back(&IRON_BOOTS,                  5);
            items.emplace_back(&OBSIDIAN,              5, 3, 7);
            items.emplace_back(&OAK_SAPLING,           5, 3, 7);
            items.emplace_back(&SADDLE,                      3);
            items.emplace_back(&IRON_HORSE_ARMOR,            1);
            items.emplace_back(&GOLDEN_HORSE_ARMOR,          1);
            items.emplace_back(&DIAMOND_HORSE_ARMOR,         1);
            lootTables.emplace_back(items,            3, 8, 94);

            maxItemsPossible = 8;
        }

}
