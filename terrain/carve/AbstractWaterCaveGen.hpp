#pragma once

#include "AbstractMapGen.hpp"

class AbstractWaterCaveGen : public AbstractMapGen {

public:
    explicit AbstractWaterCaveGen(World& world) : AbstractMapGen(world) {}

    ~AbstractWaterCaveGen() override = default;
    
    /// used for water caves and water ravines
    MU static Pos2D getSeedMultiplier(const Generator* g);
    /// used for water caves and water ravines
    MU void setupRNG(Pos2D seedMultiplier, Pos2D chunkPos);
    /// used for water caves and water ravines
    MU static void setupRNG(const Generator* g, RNG& rng, Pos2D seedMultiplier, Pos2D chunkPos);


    void generateUnderwater(ChunkPrimer* primer, Pos2D target, bool accurate = true) {
        rng.setSeed(g->getWorldSeed());
        c_int seedMultiplierX = rng.nextInt() / 2 * 2 + 1;
        c_int seedMultiplierZ = rng.nextInt() / 2 * 2 + 1;
        
        const Pos2D lower = target - CHUNK_RANGE;
        const Pos2D upper = target + CHUNK_RANGE;
        Pos2D chunkPos;
        for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
            for (chunkPos.z = lower.x; chunkPos.z <= upper.x; ++chunkPos.z) {
                Pos2D current(chunkPos.x, chunkPos.z);

                c_int adjustedSeedX = chunkPos.x * seedMultiplierX;
                c_int adjustedSeedZ = chunkPos.z * seedMultiplierZ;
                rng.setSeed((adjustedSeedX + adjustedSeedZ) ^ g->getWorldSeed());

                addFeature(primer, current, target, accurate);
            }
        }
    }
};