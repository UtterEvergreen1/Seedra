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
        items1.emplace_back(&Items::HEART_OF_THE_SEA, 1);
        lootTables.emplace_back(items1, 1, 1);

        // table 2
        items2.emplace_back(&Items::PRISMARINE_CRYSTALS, 5, 1, 5);
        items2.emplace_back(&Items::IRON_INGOT, 20, 3, 5);
        items2.emplace_back(&Items::GOLD_INGOT, 10, 1, 5);
        items2.emplace_back(&Items::TNT, 10, 1, 2);
        items2.emplace_back(&Items::DIAMOND, 15);
        items2.emplace_back(&Items::DISC_WAIT, 5);
        items2.emplace_back(&Items::DISC_MELLOHI, 5);
        items2.emplace_back(&Items::NAME_TAG, 10);
        items2.emplace_back(&Items::CHAINMAIL_CHESTPLATE, 20);
        items2.emplace_back(&Items::CHAINMAIL_HELMET, 20);
        items2.emplace_back(&Items::CHAINMAIL_LEGGINGS, 20);
        items2.emplace_back(&Items::CHAINMAIL_BOOTS, 20);
        items2.emplace_back(&Items::BOOK_AND_QUILL, 5, 1, 2);
        items2.emplace_back(&Items::LEAD, 10, 1, 3);
        items2.emplace_back(&Items::BOTTLE_O_ENCHANTING, 3);
        items2.emplace_back(&Items::POTION_OF_WATER_BREATHING, 15);
        items2.emplace_back(&Items::POTION_OF_REGENERATION, 10);
        items2.emplace_back(&Items::CAKE, 1);

        lootTables.emplace_back(items2, 5, 12);

        maxItemsPossible = 13;
    }

    template<bool shuffle>
    Container BuriedTreasure::getLoot(const int64_t worldSeed, const int chunkX, const int chunkZ) {
        RNG seed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
        return std::move(Loot::getLootFromLootTableSeed<shuffle>(static_cast<int>(seed.nextLong())));
    }
} // namespace loot
