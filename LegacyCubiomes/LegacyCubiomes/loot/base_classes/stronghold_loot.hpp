#pragma once

#include "loot.hpp"
#include "loot_classes.hpp"

#include "LegacyCubiomes/structures/structure_rolls/stronghold_rolls.hpp"
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"

using stronghold_generator::StrongholdGenerator;

namespace loot_tables {
    class StrongholdLoot : public Loot<StrongholdLoot> {
    public:
        /*combine loot seeding and generation to get the stronghold loot*/
        template<bool checkCaves, bool shuffle>
        [[nodiscard]] static Container getLoot(int64_t seed, BIOMESCALE biomeSize,
                                               int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator);

        /*loot seeding with stronghold stone rolls*/
        template<bool checkCaves>
        static uint64_t getLootSeed(int64_t worldSeed, BIOMESCALE biomeSize,
                                    int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator);
    };

    template<bool checkCaves, bool shuffle>
    Container StrongholdLoot::getLoot(int64_t seed, BIOMESCALE biomeSize,
                                      int chestX, int chestY, int chestZ,
                                      StrongholdGenerator* strongholdGenerator) {
        return getLootFromSeed<shuffle>(getLootSeed<checkCaves>(seed, biomeSize, chestX, chestY, chestZ, strongholdGenerator));
    }

    template<bool checkCaves>
    inline uint64_t StrongholdLoot::getLootSeed(int64_t worldSeed, BIOMESCALE biomeSize,
                                                int chestX, int chestY, int chestZ,
                                         StrongholdGenerator* strongholdGenerator) {
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
}
