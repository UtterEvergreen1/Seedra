#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"


namespace loot_tables {

    class NetherBridge : public Loot<NetherBridge> {
    public:
        static void setup();
    };

    void NetherBridge::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::DIAMOND,              5, 1, 3);
        items.emplace_back(&Items::IRON_INGOT,           5, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT,           15, 1, 3);
        items.emplace_back(&Items::GOLDEN_SWORD,         5);
        items.emplace_back(&Items::GOLDEN_CHESTPLATE,    5);
        items.emplace_back(&Items::FLINT_AND_STEEL,      5);
        items.emplace_back(&Items::NETHER_WART,          5, 3, 7);
        items.emplace_back(&Items::SADDLE,               10);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR,   8);
        items.emplace_back(&Items::IRON_HORSE_ARMOR,     5);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR,  3);
        items.emplace_back(&Items::OBSIDIAN,             2, 2, 4);
        lootTables.emplace_back(items,           2, 4);

        maxItemsPossible = 4;
    }


}

