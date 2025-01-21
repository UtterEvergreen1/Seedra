#pragma once
#include "loot/classes/Loot.hpp"


namespace loot {
    class NetherBridge : public Loot<NetherBridge> {
    public:
        static void setup();
    };

    inline void NetherBridge::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::DIAMOND, 5, 1, 3);
        items.emplace_back(&lce::items::IRON_INGOT, 5, 1, 5);
        items.emplace_back(&lce::items::GOLD_INGOT, 15, 1, 3);
        items.emplace_back(&lce::items::GOLDEN_SWORD, 5);
        items.emplace_back(&lce::items::GOLDEN_CHESTPLATE, 5);
        items.emplace_back(&lce::items::FLINT_AND_STEEL, 5);
        items.emplace_back(&lce::items::NETHER_WART, 5, 3, 7);
        items.emplace_back(&lce::items::SADDLE, 10);
        items.emplace_back(&lce::items::GOLDEN_HORSE_ARMOR, 8);
        items.emplace_back(&lce::items::IRON_HORSE_ARMOR, 5);
        items.emplace_back(&lce::items::DIAMOND_HORSE_ARMOR, 3);
        items.emplace_back(&lce::items::OBSIDIAN, 2, 2, 4);
        lootTables.emplace_back(items, 2, 4);

        maxItemsPossible = 4;
    }
}
