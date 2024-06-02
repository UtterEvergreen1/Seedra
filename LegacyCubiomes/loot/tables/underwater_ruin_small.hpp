#pragma once

#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"
#include "LegacyCubiomes/loot/classes/Loot.hpp"


namespace loot {
    class UnderwaterRuinSmall : public Loot<UnderwaterRuinSmall> {
    public:
        static void setup();
    };

    inline void UnderwaterRuinSmall::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&lce::items::COAL, 10, 1, 4);
        items1.emplace_back(&lce::items::STONE_AXE, 2);
        items1.emplace_back(&lce::items::ROTTEN_FLESH, 5);
        items1.emplace_back(&lce::items::EMERALD, 1);
        items1.emplace_back(&lce::items::WHEAT, 10, 2, 3);
        lootTables.emplace_back(items1, 2, 8);

        items2.emplace_back(&lce::items::LEATHER_TUNIC, 1);
        items2.emplace_back(&lce::items::GOLDEN_HELMET, 1);
        items2.emplace_back(&lce::items::FISHING_ROD, new EnchantRandomlyItem(), 5);
        items2.emplace_back(&lce::items::TREASURE_MAP, 5);

        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;
    }
}
