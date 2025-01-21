#pragma once

#include "loot/classes/Loot.hpp"
#include "loot/classes/SetMetadata.hpp"

namespace loot {
    class SpawnBonusChest : public Loot<SpawnBonusChest> {
    public:
        static void setup();
    };

    inline void SpawnBonusChest::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;
        std::vector<ItemEntry> items4;
        std::vector<ItemEntry> items5;
        std::vector<ItemEntry> items6;
        std::vector<ItemEntry> items7;
        std::vector<ItemEntry> items8;
        const std::vector LOG1_vec = {&lce::items::OAK_WOOD, &lce::items::SPRUCE_WOOD,
                                      &lce::items::BIRCH_WOOD, &lce::items::JUNGLE_WOOD};
        const std::vector LOG2_vec = {&lce::items::ACACIA_WOOD, &lce::items::DARK_OAK_WOOD};

        // table 1
        items1.emplace_back(&lce::items::JUNGLE_SAPLING, 1, 1, 5);
        lootTables.emplace_back(items1, 1);

        // table 2
        items2.emplace_back(&lce::items::MELON_SEEDS, 1, 1, 5);
        lootTables.emplace_back(items2, 1);

        // table 3
        items3.emplace_back(&lce::items::PUMPKIN_SEEDS, 1, 1, 5);
        lootTables.emplace_back(items3, 1);

        // table 4
        items4.emplace_back(&lce::items::BEETROOT_SEEDS, 1, 1, 5);
        lootTables.emplace_back(items4, 1);

        // table 5
        items5.emplace_back(&lce::items::STONE_AXE, 1);
        items5.emplace_back(&lce::items::WOODEN_AXE, 3);
        lootTables.emplace_back(items5, 1);

        // table 6
        items6.emplace_back(&lce::items::STONE_PICKAXE, 1);
        items6.emplace_back(&lce::items::WOODEN_PICKAXE, 3);
        lootTables.emplace_back(items6, 1);

        // table 7
        items7.emplace_back(&lce::items::APPLE, 5, 1, 2);
        items7.emplace_back(&lce::items::BREAD, 3, 1, 2);
        items7.emplace_back(&lce::items::RAW_SALMON, 3, 1, 2);
        lootTables.emplace_back(items7, 3);

        // table 8
        items8.emplace_back(&lce::items::STICK, 10, 1, 8);
        items8.emplace_back(&lce::items::OAK_WOOD_PLANK, 10, 1, 12);
        items8.emplace_back(&lce::items::OAK_WOOD, new SetMetadata({0, 3}, LOG1_vec), 10, 1, 3);
        items8.emplace_back(&lce::items::ACACIA_WOOD, new SetMetadata({0, 1}, LOG2_vec), 10, 1, 3);
        lootTables.emplace_back(items8, 4);

        maxItemsPossible = 9;
    }
}
