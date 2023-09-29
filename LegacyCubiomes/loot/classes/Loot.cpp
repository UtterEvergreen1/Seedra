#include "Loot.hpp"

namespace loot_tables {


/* combine loot seeding and generation to get the base loot */
    template<typename T>
    template<bool shuffle, bool legacy>
    Container Loot<T>::getLootFromChunk(int64_t worldSeed, int chunkX, int chunkZ) {
        static_assert(!shuffle || !legacy, "Legacy loot does not shuffle: change shuffle to false");
        uint64_t seed = getPopulationSeed(worldSeed, chunkX, chunkZ);
        if constexpr (legacy)
            return getLootLegacyFromSeed(&seed);
        else
            return getLootFromSeed<shuffle>(&seed);
    }


/** loot generation from seed */
    template<typename T>
    template<bool shuffle>
    Container Loot<T>::getLootFromLootTableSeed(uint64_t *lootTableSeed) {
        std::vector<ItemStack> chestContents;
        chestContents.reserve(Loot<T>::maxItemsPossible);
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        int rollCount, rollIndex;
        for (const LootTable &table: lootTables) {
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);
                chestContents.push_back(result);
            }
        }

        if constexpr (!shuffle) {
            return {std::move(chestContents)};
        }

        Container container = Container(Container::CHEST_SIZE);
        container.shuffleIntoContainer(chestContents, *lootTableSeed);
        return container;

    }

    /// loot generation from seed
    template<typename T>
    template<bool shuffle>
    inline Container Loot<T>::getLootFromSeed(uint64_t *seed) {
        return getLootFromLootTableSeed<shuffle>(nextLong(seed));
    }

    template<typename T>
    Container Loot<T>::getLootLegacyFromSeed(uint64_t *seed) {
        Container chestContents = Container();
        int rollCount;

        //generate loot
        for (const LootTable &table: lootTables) {
            rollCount = LootTable::getInt<true>(seed, table.min, table.max);
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<true>(seed);
                chestContents.setMoveInventorySlotContents(
                        nextInt(seed, Container::CHEST_SIZE), std::move(result));
            }
        }
        return chestContents;
    }


    /// other parameter options for loot finding
    template<typename T>
    template<bool shuffle, bool legacy>
    inline Container Loot<T>::getLootFromChunk(int64_t worldSeed, Pos2D chunkPos) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, chunkPos.x, chunkPos.z);
    }

    template<typename T>
    template<bool shuffle, bool legacy>
    inline Container Loot<T>::getLootFromBlock(int64_t worldSeed, int blockX, int blockZ) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, blockX >> 4, blockZ >> 4);
    }

    template<typename T>
    template<bool shuffle, bool legacy>
    inline Container Loot<T>::getLootFromBlock(int64_t worldSeed, Pos2D blockPos) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, blockPos.x >> 4, blockPos.z >> 4);
    }


    /// generate loot chests in a row eg. needed to get all desert temple chests
    template<typename T>
    template<bool shuffle, bool legacy>
    std::vector<Container> Loot<T>::getLootChests(int numChests, int64_t worldSeed, int chunkX, int chunkZ) {
        uint64_t seed = getPopulationSeed(worldSeed, chunkX, chunkZ);
        return getLootChestsFromSeed<shuffle, legacy>(numChests, &seed);
    }

    template<typename T>
    template<bool shuffle, bool legacy>
    std::vector<Container> Loot<T>::getLootChestsFromSeed(int numChests, uint64_t *seed) {
        std::vector<Container> chests(numChests);
        for (int chestIndex = 0; chestIndex < numChests; chestIndex++) {
            if constexpr (legacy)
                chests[chestIndex] = getLootLegacyFromSeed(seed);
            else
                chests[chestIndex] = getLootFromSeed<shuffle>(seed);
        }
        return chests;
    }

}