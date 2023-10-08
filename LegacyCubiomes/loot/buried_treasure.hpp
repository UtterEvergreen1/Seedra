#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"


namespace loot {
    class BuriedTreasure : public Loot<BuriedTreasure> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLoot(int64_t worldSeed, int chunkX, int chunkZ);

        static int checkFor5Cakes(int seed);
    };

    void BuriedTreasure::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&Items::HEART_OF_THE_SEA,           1);
        lootTables.emplace_back(items1,                 1, 1);

        // table 2
        items2.emplace_back(&Items::PRISMARINE_CRYSTALS,        5, 1, 5);
        items2.emplace_back(&Items::IRON_INGOT,                 20, 3, 5);
        items2.emplace_back(&Items::GOLD_INGOT,                 10, 1, 5);
        items2.emplace_back(&Items::TNT,                        10, 1, 2);
        items2.emplace_back(&Items::DIAMOND,                    15);
        items2.emplace_back(&Items::DISC_WAIT,                  5);
        items2.emplace_back(&Items::DISC_MELLOHI,               5);
        items2.emplace_back(&Items::NAME_TAG,                   10);
        items2.emplace_back(&Items::CHAINMAIL_CHESTPLATE,       20);
        items2.emplace_back(&Items::CHAINMAIL_HELMET,           20);
        items2.emplace_back(&Items::CHAINMAIL_LEGGINGS,         20);
        items2.emplace_back(&Items::CHAINMAIL_BOOTS,            20);
        items2.emplace_back(&Items::BOOK_AND_QUILL,             5, 1, 2);
        items2.emplace_back(&Items::LEAD,                       10, 1, 3);
        items2.emplace_back(&Items::BOTTLE_O_ENCHANTING,        3);
        items2.emplace_back(&Items::POTION_OF_WATER_BREATHING,  15);
        items2.emplace_back(&Items::POTION_OF_REGENERATION,     10);
        items2.emplace_back(&Items::CAKE,                       1);
        lootTables.emplace_back(items2,                 5, 12);

        maxItemsPossible = 13;

    }

    template <bool shuffle>
    Container BuriedTreasure::getLoot(int64_t worldSeed, int chunkX, int chunkZ) {
        int rollCount;
        uint64_t seed = getPopulationSeed(worldSeed, chunkX, chunkZ);
        uint64_t lootTableSeed = (int)nextLong(&seed);
        setSeed(&lootTableSeed, lootTableSeed);

        std::vector<ItemStack> chestContents(maxItemsPossible);

        //generate loot
        int slotIndex = 0;
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(&lootTableSeed, table.min, table.max);
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(&lootTableSeed);
                chestContents[slotIndex++] = result;
            }
        }
        if constexpr (shuffle){
            chestContents.resize(slotIndex);
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return {27, chestContents};
    }

    int BuriedTreasure::checkFor5Cakes(int seed) {
        uint64_t lootSeed = seed ^ 0x5deece66d;
        nextInt(&lootSeed, 1);
        int rollCount = LootTable::getInt<false>(&lootSeed, 5, 12);
        int cakeCount = 0;

        for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
            ItemStack result = lootTables[1].createLootRoll<false>(&lootSeed);
            if (result.item->getID() == Items::ItemID::CAKE_ID) {
                ++cakeCount;
            }
        }
        return cakeCount;
    }
}

