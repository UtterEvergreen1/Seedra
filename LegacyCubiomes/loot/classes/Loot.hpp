#pragma once

#include <vector>

#include "LegacyCubiomes/utils/pos2D.hpp"

#include "LegacyCubiomes/loot/classes/Container.hpp"

namespace loot_tables {
    template <typename T>
    class Loot {
    public:
        static std::vector<LootTable> lootTables;
        static int maxItemsPossible;

        /// combine base loot seeding and generation to get the base loot
        template<bool shuffle, bool legacy>
        ND static Container getLootFromChunk(int64_t worldSeed, int chunkX, int chunkZ);

        /// loot generation from seed, don't use in a search unless you know the exact seed
        template<bool shuffle>
        static Container getLootFromSeed(uint64_t* seed);
        template<bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);

        static Container getLootLegacyFromSeed(uint64_t* seed);

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
        static std::vector<Container> getLootChestsFromSeed(int numChests, uint64_t* seed);
    };


}

