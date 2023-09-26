#include "stronghold_loot.hpp"

namespace loot_tables {

    template <typename T>
    template<bool checkCaves>
    inline uint64_t StrongholdLoot<T>::getLootSeed(const Generator& g, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator) {
        int chunkX = chestX >> 4;
        int chunkZ = chestZ >> 4;
        uint64_t rng = getPopulationSeed(g.getWorldSeed(), chestX >> 4, chestZ >> 4);

        if constexpr (checkCaves) {
            ChunkPrimer* chunk = Chunk::provideChunk<true, true, false>(g, chunkX, chunkZ);
            // we roll rng equal to the stone bricks in the chunk that generated before the chest corridor
            structure_rolls::StrongholdRolls::generateStructure<true>(chunk, strongholdGenerator, &rng, chestX, chestY,
                                                                      chestZ);
            delete chunk;
        }
        else {
            structure_rolls::StrongholdRolls::generateStructure<true>(nullptr, strongholdGenerator, &rng, chestX,
                                                                      chestY, chestZ);
        }
        return rng;
    }

    template <typename T>
    template<bool checkCaves, bool shuffle>
    Container StrongholdLoot<T>::getLoot(const Generator& g, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator) {
        return T::getLootFromSeed<shuffle>(getLootSeed<checkCaves>(g, chestX, chestY, chestZ, strongholdGenerator));
    }

}