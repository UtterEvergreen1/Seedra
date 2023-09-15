#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {

    class NetherBridge : public Loot<NetherBridge> {
    public:
        static void setup();
    };

    void NetherBridge::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&DIAMOND,              5, 1, 3);
        items.emplace_back(&IRON_INGOT,           5, 1, 5);
        items.emplace_back(&GOLD_INGOT,           15, 1, 3);
        items.emplace_back(&GOLDEN_SWORD,         5);
        items.emplace_back(&GOLDEN_CHESTPLATE,    5);
        items.emplace_back(&FLINT_AND_STEEL,      5);
        items.emplace_back(&NETHER_WART,          5, 3, 7);
        items.emplace_back(&SADDLE,               10);
        items.emplace_back(&GOLDEN_HORSE_ARMOR,   8);
        items.emplace_back(&IRON_HORSE_ARMOR,     5);
        items.emplace_back(&DIAMOND_HORSE_ARMOR,  3);
        items.emplace_back(&OBSIDIAN,             2, 2, 4);
        lootTables.emplace_back(items,                 2, 4, 73);

        maxItemsPossible = 4;
    }


}

