#include "stronghold_loot.hpp"

namespace loot_tables {

    template <typename T>
    template<bool checkCaves>
    inline uint64_t StrongholdLoot<T>::getLootSeed(int64_t worldSeed, BIOMESCALE biomeSize, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator) {
        int chunkX = chestX >> 4;
        int chunkZ = chestZ >> 4;
        uint64_t rng = getPopulationSeed(worldSeed, chestX >> 4, chestZ >> 4);

        if constexpr (checkCaves) {
            ChunkPrimer* chunk = Chunk::provideChunk<true, true>(worldSeed, chunkX, chunkZ, biomeSize);
            // we roll rng equal to the stone bricks in the chunk that generated before the chest corridor
            structure_rolls::StrongholdRolls::generateStructure<true>(&rng, chestX, chestY, chestZ, strongholdGenerator, chunk);
            delete chunk;
        }
        else {
            structure_rolls::StrongholdRolls::generateStructure<true>(&rng, chestX, chestY, chestZ, strongholdGenerator, nullptr);
        }
        return rng;
    }

    template <typename T>
    template<bool checkCaves, bool shuffle>
    Container StrongholdLoot<T>::getLoot(int64_t seed, BIOMESCALE biomeSize, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator) {
        return T::getLootFromSeed<shuffle>(getLootSeed<checkCaves>(
                seed, biomeSize, chestX, chestY, chestZ, strongholdGenerator));
    }

}