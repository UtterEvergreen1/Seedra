#pragma once

#include <vector>

#include "LegacyCubiomes/loot/classes/Container.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"


namespace loot {
    template<typename T>
    class Loot {
    public:
        static std::vector<LootTable> lootTables;
        static uint8_t maxItemsPossible;

        /// combine base loot seeding and generation to get the base loot
        template<bool shuffle, bool legacy>
        ND static Container getLootFromChunk(int64_t worldSeed, int chunkX, int chunkZ);

        /// loot generation from seed, don't use in a search unless you know the exact seed
        template<bool shuffle>
        static Container getLootFromSeed(RNG& seed);
        template<bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t lootTableSeed);

        static Container getLootLegacyFromSeed(RNG& seed);

        /// other parameter options for loot finding
        template<bool shuffle, bool legacy>
        static Container getLootFromChunk(int64_t worldSeed, Pos2D chunkPos);
        template<bool shuffle, bool legacy>
        static Container getLootFromBlock(int64_t worldSeed, int blockX, int blockZ);
        template<bool shuffle, bool legacy>
        static Container getLootFromBlock(int64_t worldSeed, Pos2D blockPos);

        /// generate loot chests in a row eg. needed to get all desert temple chests
        template<bool shuffle, bool legacy>
        ND static std::vector<Container> getLootChests(int numChests, int64_t worldSeed, int chunkX, int chunkZ);

        template<bool shuffle, bool legacy>
        static std::vector<Container> getLootChestsFromSeed(int numChests, RNG& seed);
    };


    template<typename T>
    std::vector<LootTable> Loot<T>::lootTables;

    template<typename T>
    uint8_t Loot<T>::maxItemsPossible;

    /// combine loot seeding and generation to get the base loot
    template<typename T>
    template<bool shuffle, bool legacy>
    Container Loot<T>::getLootFromChunk(const int64_t worldSeed, const int chunkX, const int chunkZ) {
        static_assert(!shuffle || !legacy, "Legacy loot does not shuffle: change shuffle to false");
        RNG seed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
        if constexpr (legacy) return getLootLegacyFromSeed(seed);
        else
            return getLootFromSeed<shuffle>(seed);
    }


    /// loot generation from seed
    template<typename T>
    template<bool shuffle>
    Container Loot<T>::getLootFromLootTableSeed(const uint64_t lootTableSeed) {
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        RNG random;
        random.setSeed(lootTableSeed);

        // generate loot
        for (const LootTable& table: lootTables) {
            const int rollCount = random.nextInt(table.getMin(), table.getMax());
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(random);
                chestContents.push_back(result);
            }
        }

        if constexpr (!shuffle) { return {std::move(chestContents)}; }

        auto container = Container(Container::CHEST_SIZE);
        container.shuffleIntoContainer(chestContents, random);
        return container;
    }

    /// loot generation from seed
    template<typename T>
    template<bool shuffle>
    Container Loot<T>::getLootFromSeed(RNG& seed) {
        return getLootFromLootTableSeed<shuffle>(seed.nextLong());
    }

    template<typename T>
    Container Loot<T>::getLootLegacyFromSeed(RNG& seed) {
        auto chestContents = Container();

        //generate loot
        for (const LootTable& table: lootTables) {
            const int rollCount = seed.nextIntLegacy(table.getMin(), table.getMax());
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<true>(seed);
                chestContents.setInventorySlotContents(seed.nextInt(Container::CHEST_SIZE), std::move(result));
            }
        }
        return chestContents;
    }


    /// other parameter options for loot finding
    template<typename T>
    template<bool shuffle, bool legacy>
    Container Loot<T>::getLootFromChunk(const int64_t worldSeed, const Pos2D chunkPos) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, chunkPos.x, chunkPos.z);
    }

    template<typename T>
    template<bool shuffle, bool legacy>
    Container Loot<T>::getLootFromBlock(const int64_t worldSeed, const int blockX, const int blockZ) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, blockX >> 4, blockZ >> 4);
    }

    template<typename T>
    template<bool shuffle, bool legacy>
    Container Loot<T>::getLootFromBlock(const int64_t worldSeed, const Pos2D blockPos) {
        return getLootFromChunk<shuffle, legacy>(worldSeed, blockPos.x >> 4, blockPos.z >> 4);
    }


    /// generate loot chests in a row eg. needed to get all desert temple chests
    template<typename T>
    template<bool shuffle, bool legacy>
    std::vector<Container> Loot<T>::getLootChests(const int numChests, const int64_t worldSeed, const int chunkX,
                                                  const int chunkZ) {
        RNG seed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
        return getLootChestsFromSeed<shuffle, legacy>(numChests, seed);
    }

    template<typename T>
    template<bool shuffle, bool legacy>
    std::vector<Container> Loot<T>::getLootChestsFromSeed(const int numChests, RNG& seed) {
        std::vector<Container> chests(numChests);
        for (int chestIndex = 0; chestIndex < numChests; chestIndex++) {
            if constexpr (legacy) chests[chestIndex] = getLootLegacyFromSeed(seed);
            else
                chests[chestIndex] = getLootFromSeed<shuffle>(seed);
        }
        return chests;
    }

}
