#pragma once

#include "Loot.hpp"

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/structures/rolls/stronghold.hpp"

namespace loot {
    template<typename T>
    class StrongholdLoot : public Loot<StrongholdLoot<T>> {
    public:
        /// loot seeding with stronghold stone rolls
        template<bool checkCaves>
        static RNG getLootSeed(const Generator& g, generation::Stronghold* strongholdGenerator, const Piece& piece,
                               int chestChunkX, int chestChunkZ) {
            RNG rng = RNG::getPopulationSeed(g.getWorldSeed(), chestChunkX, chestChunkZ);

            if constexpr (checkCaves) {
                ChunkPrimer* chunk = Chunk::provideChunk<true, true, false>(g, chestChunkX, chestChunkZ);
                // we roll rng equal to the stone bricks in the chunk that generated before the chest corridor
                if (!structure_rolls::Stronghold::generateStructure<true>(chunk, strongholdGenerator, rng, piece,
                                                                          chestChunkX, chestChunkZ)) {
                    delete chunk;
                    return RNG(-1);
                }
                delete chunk;
            } else {
                structure_rolls::Stronghold::generateStructure<true>(nullptr, strongholdGenerator, rng, piece,
                                                                     chestChunkX, chestChunkZ);
            }
            return rng;
        }

        /// combine loot seeding and generation to get the stronghold loot
        template<bool checkCaves, bool shuffle>
        ND static Container getLoot(const Generator& g, generation::Stronghold* strongholdGenerator, const Piece& piece,
                                    int chestChunkX, int chestChunkZ) {
            uint64_t lootSeed = getLootSeed<checkCaves>(g, strongholdGenerator, piece, chestChunkX, chestChunkZ);
            if (lootSeed == -1) return {};
            return T::template getLootFromSeed<shuffle>(lootSeed);
        }
    };
} // namespace loot
