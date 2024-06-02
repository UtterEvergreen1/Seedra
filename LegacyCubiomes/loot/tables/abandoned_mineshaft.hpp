#pragma once

#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"
#include "LegacyCubiomes/loot/classes/Loot.hpp"


namespace loot {
    class AbandonedMineshaft : public Loot<AbandonedMineshaft> {
    public:
        static void setup();
    };

    inline void AbandonedMineshaft::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&lce::items::GOLDEN_APPLE, 20);
        items1.emplace_back(&lce::items::ENCHANTED_GOLDEN_APPLE, 1);
        items1.emplace_back(&lce::items::NAME_TAG, 30);
        items1.emplace_back(&lce::items::ENCHANTED_BOOK, new EnchantRandomlyBook(), 10);
        items1.emplace_back(&lce::items::IRON_PICKAXE, 5);
        items1.emplace_back(&lce::items::AIR, 5);
        lootTables.emplace_back(items1, 1);

        // table 2
        items2.emplace_back(&lce::items::IRON_INGOT, 10, 1, 5);
        items2.emplace_back(&lce::items::GOLD_INGOT, 5, 1, 3);
        items2.emplace_back(&lce::items::REDSTONE, 5, 4, 9);
        items2.emplace_back(&lce::items::LAPIS_LAZULI, 5, 4, 9);
        items2.emplace_back(&lce::items::DIAMOND, 3, 1, 2);
        items2.emplace_back(&lce::items::COAL, 10, 3, 8);
        items2.emplace_back(&lce::items::BREAD, 15, 1, 3);
        items2.emplace_back(&lce::items::MELON_SEEDS, 10, 2, 4);
        items2.emplace_back(&lce::items::PUMPKIN_SEEDS, 10, 2, 4);
        items2.emplace_back(&lce::items::BEETROOT_SEEDS, 10, 2, 4);
        lootTables.emplace_back(items2, 2, 4);

        // table 3
        items3.emplace_back(&lce::items::RAIL, 20, 4, 8);
        items3.emplace_back(&lce::items::POWERED_RAIL, 5, 1, 4);
        items3.emplace_back(&lce::items::DETECTOR_RAIL, 5, 1, 4);
        items3.emplace_back(&lce::items::ACTIVATOR_RAIL, 5, 1, 4);
        items3.emplace_back(&lce::items::TORCH, 15, 1, 16);
        lootTables.emplace_back(items3, 3);

        maxItemsPossible = 8;
    }
} // namespace loot
