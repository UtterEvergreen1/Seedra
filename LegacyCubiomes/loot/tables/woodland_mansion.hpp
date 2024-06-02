#pragma once

#include "LegacyCubiomes/loot/classes/EnchantRandomly.hpp"
#include "LegacyCubiomes/loot/classes/Loot.hpp"


namespace loot {
    class Mansion : public Loot<Mansion> {
    public:
        static void setup();
    };

    inline void Mansion::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&lce::items::LEAD, 100);
        items1.emplace_back(&lce::items::GOLDEN_APPLE, 75);
        items1.emplace_back(&lce::items::ENCHANTED_GOLDEN_APPLE, 10);
        items1.emplace_back(&lce::items::DISC_13, 15);
        items1.emplace_back(&lce::items::DISC_CAT, 15);
        items1.emplace_back(&lce::items::NAME_TAG, 100);
        items1.emplace_back(&lce::items::CHAINMAIL_CHESTPLATE, 50);
        items1.emplace_back(&lce::items::DIAMOND_HOE, 75);
        items1.emplace_back(&lce::items::DIAMOND_CHESTPLATE, 25);
        items1.emplace_back(&lce::items::ENCHANTED_BOOK, new EnchantRandomlyBook(), 60);
        lootTables.emplace_back(items1, 1, 3);

        // table 2
        items2.emplace_back(&lce::items::IRON_INGOT, 50, 1, 4);
        items2.emplace_back(&lce::items::GOLD_INGOT, 25, 1, 4);
        items2.emplace_back(&lce::items::BREAD, 100);
        items2.emplace_back(&lce::items::WHEAT, 100, 1, 4);
        items2.emplace_back(&lce::items::BUCKET, 50);
        items2.emplace_back(&lce::items::REDSTONE, 75, 1, 4);
        items2.emplace_back(&lce::items::COAL, 75, 1, 4);
        items2.emplace_back(&lce::items::MELON_SEEDS, 50, 2, 4);
        items2.emplace_back(&lce::items::PUMPKIN_SEEDS, 50, 2, 4);
        items2.emplace_back(&lce::items::BEETROOT_SEEDS, 50, 2, 4);
        lootTables.emplace_back(items2, 1, 4);

        // table 3
        items3.emplace_back(&lce::items::BONE, 50, 1, 8);
        items3.emplace_back(&lce::items::GUNPOWDER, 50, 1, 8);
        items3.emplace_back(&lce::items::ROTTEN_FLESH, 50, 1, 8);
        items3.emplace_back(&lce::items::STRING, 50, 1, 8);
        lootTables.emplace_back(items3, 3);

        maxItemsPossible = 10;
    }
}
