#pragma once

#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"
#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"


namespace loot {
    class StrongholdCrossing : public StrongholdLoot<StrongholdCrossing> {
    public:
        static void setup();
    };

    inline void StrongholdCrossing::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::IRON_INGOT, 50, 1, 5);
        items.emplace_back(&lce::items::GOLD_INGOT, 25, 1, 3);
        items.emplace_back(&lce::items::REDSTONE, 25, 4, 9);
        items.emplace_back(&lce::items::COAL, 50, 3, 8);
        items.emplace_back(&lce::items::BREAD, 75, 1, 3);
        items.emplace_back(&lce::items::APPLE, 75, 1, 3);
        items.emplace_back(&lce::items::IRON_PICKAXE, 5);
        items.emplace_back(&lce::items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30), 6);
        items.emplace_back(&lce::items::INK_SACK, 75, 1, 3);

        lootTables.emplace_back(items, 1, 4);

        maxItemsPossible = 4;
    }
}
