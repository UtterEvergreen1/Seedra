#pragma once

#include "Loot.hpp"

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/structures/rolls/stronghold.hpp"

namespace loot {
    template<typename T>
    class StrongholdLoot : public Loot<StrongholdLoot<T>> {
    public:
        /// loot seeding with stronghold stone rolls
        template<bool checkCaves, bool checkWaterCaves = false>
        ND static RNG getLootSeed(const Generator& g, gen::Stronghold* strongholdGenerator, const Piece& piece,
                                  c_int chestChunkX, c_int chestChunkZ, bool accurate);

        /// combine loot seeding and generation to get the stronghold loot
        template<bool checkCaves, bool shuffle>
        MU ND static Container getLoot(const Generator& g, gen::Stronghold* strongholdGenerator,
                                       const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ,
                                       bool accurate);
    };

    /// loot seeding with stronghold stone rolls
    template<typename T>
    template<bool checkCaves, bool checkWaterCaves>
    RNG StrongholdLoot<T>::getLootSeed(const Generator& g, gen::Stronghold* strongholdGenerator,
                                       const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ,
                                       bool accurate) {
        RNG rng = RNG::getPopulationSeed(g.getWorldSeed(), chestChunkX, chestChunkZ);

        if constexpr (checkCaves) {
            ChunkPrimer* chunk = Chunk::provideChunk<checkWaterCaves>(g, chestChunkX, chestChunkZ, accurate);
            // we roll rng equal to the stone bricks in the chunk that generated before the chest corridor
            if (!rolls::Stronghold::generateStructure<true>(chunk, strongholdGenerator, rng, chestChunkX, chestChunkZ,
                                                            piece)) {
                delete chunk;
                return RNG::ConstructWithoutSetSeed(-1);
            }
            delete chunk;
        } else {
            rolls::Stronghold::generateStructure<true>(nullptr, strongholdGenerator, rng, chestChunkX, chestChunkZ,
                                                       piece);
        }
        return rng;
    }

    /// combine loot seeding and generation to get the stronghold loot
    template<typename T>
    template<bool checkCaves, bool shuffle>
    Container StrongholdLoot<T>::getLoot(const Generator& g, gen::Stronghold* strongholdGenerator,
                                         const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ,
                                         const bool accurate) {
        u64 lootSeed = StrongholdLoot<T>::template getLootSeed<checkCaves>(g, strongholdGenerator, piece, chestChunkX,
                                                                           chestChunkZ, accurate);
        if (lootSeed == -1) return {};
        return T::template getLootFromSeed<shuffle>(lootSeed);
    }
} // namespace loot
