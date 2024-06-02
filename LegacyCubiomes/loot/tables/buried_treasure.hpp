#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"

namespace loot {
    class BuriedTreasure : public Loot<BuriedTreasure> {
    public:
        static void setup();

        template<bool shuffle>
        MU static Container getLoot(int64_t worldSeed, int chunkX, int chunkZ);
    };

    inline void BuriedTreasure::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&lce::items::HEART_OF_THE_SEA, 1);
        lootTables.emplace_back(items1, 1, 1);

        // table 2
        items2.emplace_back(&lce::items::PRISMARINE_CRYSTALS, 5, 1, 5);
        items2.emplace_back(&lce::items::IRON_INGOT, 20, 3, 5);
        items2.emplace_back(&lce::items::GOLD_INGOT, 10, 1, 5);
        items2.emplace_back(&lce::items::TNT, 10, 1, 2);
        items2.emplace_back(&lce::items::DIAMOND, 15);
        items2.emplace_back(&lce::items::DISC_WAIT, 5);
        items2.emplace_back(&lce::items::DISC_MELLOHI, 5);
        items2.emplace_back(&lce::items::NAME_TAG, 10);
        items2.emplace_back(&lce::items::CHAINMAIL_CHESTPLATE, 20);
        items2.emplace_back(&lce::items::CHAINMAIL_HELMET, 20);
        items2.emplace_back(&lce::items::CHAINMAIL_LEGGINGS, 20);
        items2.emplace_back(&lce::items::CHAINMAIL_BOOTS, 20);
        items2.emplace_back(&lce::items::BOOK_AND_QUILL, 5, 1, 2);
        items2.emplace_back(&lce::items::LEAD, 10, 1, 3);
        items2.emplace_back(&lce::items::BOTTLE_O_ENCHANTING, 3);
        items2.emplace_back(&lce::items::POTION_OF_WATER_BREATHING, 15);
        items2.emplace_back(&lce::items::POTION_OF_REGENERATION, 10);
        items2.emplace_back(&lce::items::CAKE, 1);

        lootTables.emplace_back(items2, 5, 12);

        maxItemsPossible = 13;
    }

    template<bool shuffle>
    Container BuriedTreasure::getLoot(const int64_t worldSeed, const int chunkX, const int chunkZ) {
        RNG seed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
        return std::move(Loot::getLootFromLootTableSeed<shuffle>(static_cast<int>(seed.nextLong())));
    }
} // namespace loot
