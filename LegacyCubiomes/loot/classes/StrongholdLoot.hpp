#pragma once

#include "Loot.hpp"


#include "LegacyCubiomes/structures/rolls/stronghold.hpp"
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"



namespace loot {
    template <typename T>
    class StrongholdLoot : public Loot<StrongholdLoot<T>> {
    public:

        /// loot seeding with stronghold stone rolls
        template<bool checkCaves>
        static uint64_t getLootSeed(const Generator& g, int chestX, int chestY, int chestZ, generation::Stronghold* strongholdGenerator) {
            int chunkX = chestX >> 4;
            int chunkZ = chestZ >> 4;
            uint64_t rng = getPopulationSeed(g.getWorldSeed(), chestX >> 4, chestZ >> 4);

            if constexpr (checkCaves) {
                ChunkPrimer* chunk = Chunk::provideChunk<true, true, false>(g, chunkX, chunkZ);
                // we roll rng equal to the stone bricks in the chunk that generated before the chest corridor
                structure_rolls::Stronghold::generateStructure<true>(chunk, strongholdGenerator, &rng, chestX, chestY, chestZ);
                delete chunk;
            }
            else {
                structure_rolls::Stronghold::generateStructure<true>(nullptr, strongholdGenerator, &rng, chestX, chestY, chestZ);
            }
            return rng;
        }

        /// combine loot seeding and generation to get the stronghold loot
        template<bool checkCaves, bool shuffle>
        ND static Container getLoot(const Generator& g, int chestX, int chestY, int chestZ, generation::Stronghold* strongholdGenerator) {
            return T::template getLootFromSeed<shuffle>(getLootSeed<checkCaves>(g, chestX, chestY, chestZ, strongholdGenerator));
        }
    };

}
