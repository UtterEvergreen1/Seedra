#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class JungleTempleDispenser : public Loot<JungleTempleDispenser> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLoot(int64_t worldSeed, int chunkX, int chunkZ);
        static Container getLootLegacy(int64_t worldSeed, int chunkX, int chunkZ);
    };

    void JungleTempleDispenser::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&ARROW, 30, 2, 7);
        lootTables.emplace_back(items,  1, 2, 30);

        maxItemsPossible = 2;
    }

    template <bool shuffle>
    Container JungleTempleDispenser::getLoot(int64_t worldSeed, int chunkX, int chunkZ) {
        std::vector<ItemStack> chestContents;
        int rollCount;

        uint64_t lootTableSeed = getPopulationSeed(worldSeed, chunkX, chunkZ);
        lootTableSeed = nextLong(&lootTableSeed);
        setSeed(&lootTableSeed, lootTableSeed);

        //generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(&lootTableSeed, table.min, table.max);
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(&lootTableSeed);
                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container(9);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return {9, chestContents};
    }

    Container JungleTempleDispenser::getLootLegacy(int64_t worldSeed, int chunkX, int chunkZ) {
        Container chestContents = Container(9);
        int rollCount;

        uint64_t lootTableSeed = getPopulationSeed(worldSeed, chunkX, chunkZ);

        //generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<true>(&lootTableSeed, table.min, table.max);
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<true>(&lootTableSeed);
                chestContents.setInventorySlotContents(nextInt(&lootTableSeed, 9), result);
            }
        }
        return chestContents;
    }

}

