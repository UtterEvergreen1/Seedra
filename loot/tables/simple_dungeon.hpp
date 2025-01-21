#pragma once

#include "loot/classes/EnchantRandomly.hpp"
#include "loot/classes/Loot.hpp"


namespace loot {
    class SimpleDungeon : public Loot<SimpleDungeon> {
    public:
        static void setup();
    };

    inline void SimpleDungeon::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&lce::items::SADDLE, 20);
        items1.emplace_back(&lce::items::GOLDEN_APPLE, 15);
        items1.emplace_back(&lce::items::ENCHANTED_GOLDEN_APPLE, 2);
        items1.emplace_back(&lce::items::DISC_13, 15);
        items1.emplace_back(&lce::items::DISC_CAT, 15);
        items1.emplace_back(&lce::items::NAME_TAG, 20);
        items1.emplace_back(&lce::items::GOLDEN_HORSE_ARMOR, 10);
        items1.emplace_back(&lce::items::IRON_HORSE_ARMOR, 15);
        items1.emplace_back(&lce::items::DIAMOND_HORSE_ARMOR, 5);
        items1.emplace_back(&lce::items::ENCHANTED_BOOK, new EnchantRandomlyBook(), 10);
        lootTables.emplace_back(items1, 1, 3);

        // table 2
        items2.emplace_back(&lce::items::IRON_INGOT, 10, 1, 4);
        items2.emplace_back(&lce::items::GOLD_INGOT, 5, 1, 4);
        items2.emplace_back(&lce::items::BREAD, 20);
        items2.emplace_back(&lce::items::WHEAT, 20, 1, 4);
        items2.emplace_back(&lce::items::BUCKET, 10);
        items2.emplace_back(&lce::items::REDSTONE, 15, 1, 4);
        items2.emplace_back(&lce::items::COAL, 15, 1, 4);
        items2.emplace_back(&lce::items::MELON_SEEDS, 10, 2, 4);
        items2.emplace_back(&lce::items::PUMPKIN_SEEDS, 10, 2, 4);
        items2.emplace_back(&lce::items::BEETROOT_SEEDS, 10, 2, 4);
        lootTables.emplace_back(items2, 1, 4);

        // table 3
        items3.emplace_back(&lce::items::BONE, 10, 1, 8);
        items3.emplace_back(&lce::items::GUNPOWDER, 10, 1, 8);
        items3.emplace_back(&lce::items::ROTTEN_FLESH, 10, 1, 8);
        items3.emplace_back(&lce::items::STRING, 10, 1, 8);
        lootTables.emplace_back(items3, 3);

        maxItemsPossible = 10;
    }
}
