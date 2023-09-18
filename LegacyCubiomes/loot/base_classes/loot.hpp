#pragma once

#include <vector>

#include "LegacyCubiomes/utils/pos2D.hpp"

#include "loot_classes.hpp"

namespace loot_tables {
    template <typename T>
    class Loot {
    public:
        static std::vector<LootTable> lootTables;
        static int maxItemsPossible;

        /*combine base loot seeding and generation to get the base loot*/
        template<bool shuffle, bool legacy>
        ND static Container getLootFromChunk(int64_t worldSeed, int chunkX, int chunkZ);

        /*loot generation from seed, don't use in a search unless you know the exact seed*/
        template<bool shuffle>
        static Container getLootFromSeed(uint64_t* seed);
        template<bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t lootTableSeed);

        static Container getLootLegacyFromSeed(uint64_t* seed);

        /*other parameter options for loot finding*/
        template<bool shuffle, bool legacy>
        static Container getLootFromChunk(int64_t worldSeed, Pos2D chunkPos);
        template<bool shuffle, bool legacy>
        static Container getLootFromBlock(int64_t worldSeed, int blockX, int blockZ);
        template<bool shuffle, bool legacy>
        static Container getLootFromBlock(int64_t worldSeed, Pos2D blockPos);

        /*generate loot chests in a row eg. needed to get all desert temple chests*/
        template<bool shuffle, bool legacy>
        [[nodiscard]] static std::vector<Container> getLootChests(int numChests, int64_t worldSeed, int chunkX, int chunkZ);

        template<bool shuffle, bool legacy>
        static std::vector<Container> getLootChestsFromSeed(int numChests, uint64_t* seed);
    };

    template <typename T>
    std::vector<LootTable> Loot<T>::lootTables;

    template <typename T>
    int Loot<T>::maxItemsPossible;


    /* combine loot seeding and generation to get the base loot */
    template <typename T>
    template<bool shuffle, bool legacy>
    Container Loot<T>::getLootFromChunk(int64_t worldSeed, int chunkX, int chunkZ){
        uint64_t seed = getPopulationSeed(worldSeed, chunkX, chunkZ);
        if constexpr (legacy)
            return getLootLegacyFromSeed(&seed);
        else
            return getLootFromSeed<shuffle>(&seed);
    }

    /* loot generation from seed */
    template <typename T>
    template <bool shuffle>
    Container Loot<T>::getLootFromLootTableSeed(uint64_t lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(&lootTableSeed, lootTableSeed);

        // generate loot
        for (const LootTable& table : lootTables) {
            rollCount = LootTable::getInt<false>(&lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(&lootTableSeed);
                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle) {
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return  { 27, chestContents };
    }

    /* loot generation from seed */
    template <typename T>
    template <bool shuffle>
    inline Container Loot<T>::getLootFromSeed(uint64_t* seed) {
        return getLootFromLootTableSeed<shuffle>(nextLong(seed));
    }

    template <typename T>
    Container Loot<T>::getLootLegacyFromSeed(uint64_t* seed) {
        Container chestContents = Container(27);
        int rollCount;

        //generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<true>(seed, table.min, table.max);
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<true>(seed);
                chestContents.setInventorySlotContents(nextInt(seed, 27), result);
            }
        }
        return chestContents;
    }


    /*other parameter options for loot finding*/
    template <typename T>
    template<bool shuffle, bool legacy>
    inline Container Loot<T>::getLootFromChunk(int64_t worldSeed, Pos2D chunkPos) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, chunkPos.x, chunkPos.z);
    }

    template <typename T>
    template<bool shuffle, bool legacy>
    inline Container Loot<T>::getLootFromBlock(int64_t worldSeed, int blockX, int blockZ) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, blockX >> 4, blockZ >> 4);
    }

    template <typename T>
    template<bool shuffle, bool legacy>
    inline Container Loot<T>::getLootFromBlock(int64_t worldSeed, Pos2D blockPos) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, blockPos.x >> 4, blockPos.z >> 4);
    }


    /*generate loot chests in a row eg. needed to get all desert temple chests*/
    template <typename T>
    template<bool shuffle, bool legacy>
    std::vector<Container> Loot<T>::getLootChests(int numChests, int64_t worldSeed, int chunkX, int chunkZ){
        uint64_t seed = getPopulationSeed(worldSeed, chunkX, chunkZ);
        return getLootChestsFromSeed<shuffle, legacy>(numChests, &seed);
    }

    template <typename T>
    template <bool shuffle, bool legacy>
    std::vector<Container> Loot<T>::getLootChestsFromSeed(int numChests, uint64_t* seed) {
        std::vector<Container> chests(numChests);
        for(int chestIndex = 0; chestIndex < numChests; chestIndex++){
            if constexpr (legacy)
                chests[chestIndex] = getLootLegacyFromSeed(seed);
            else
                chests[chestIndex] = getLootFromSeed<shuffle>(seed);
        }
        return chests;
    }
}

