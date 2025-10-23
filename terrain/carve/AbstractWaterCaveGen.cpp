#include "AbstractWaterCaveGen.hpp"


MU Pos2D AbstractWaterCaveGen::getSeedMultiplier(const Generator* g) {
    RNG rng;
    rng.setSeed(g->getWorldSeed());
    Pos2D seedMultiplier;
    seedMultiplier.x = rng.nextInt() / 2 * 2 + 1;
    seedMultiplier.z = rng.nextInt() / 2 * 2 + 1;
    return seedMultiplier;
}


MU void AbstractWaterCaveGen::setupRNG(Pos2D seedMultiplier, Pos2D chunkPos) {
    c_int adjustedX = chunkPos.x * seedMultiplier.x;
    c_int adjustedZ = chunkPos.z * seedMultiplier.z;

    rng.setSeed((adjustedX + adjustedZ) ^ g->getWorldSeed());
}


MU void AbstractWaterCaveGen::setupRNG(const Generator* g,
                                       RNG& rng, Pos2D seedMultiplier, Pos2D chunkPos) {
    c_int adjustedX = chunkPos.x * seedMultiplier.x;
    c_int adjustedZ = chunkPos.z * seedMultiplier.z;

    rng.setSeed((adjustedX + adjustedZ) ^ g->getWorldSeed());
}

void AbstractWaterCaveGen::generateUnderwater(ChunkPrimer *primer, Pos2D target, bool accurate) {
    rng.setSeed(g->getWorldSeed());
    c_int seedMultiplierX = rng.nextInt() / 2 * 2 + 1;
    c_int seedMultiplierZ = rng.nextInt() / 2 * 2 + 1;

    const Pos2D lower = target - CHUNK_RANGE;
    const Pos2D upper = target + CHUNK_RANGE;
    Pos2D chunkPos;
    for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
        for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {
            Pos2D current(chunkPos.x, chunkPos.z);

            c_int adjustedSeedX = chunkPos.x * seedMultiplierX;
            c_int adjustedSeedZ = chunkPos.z * seedMultiplierZ;
            rng.setSeed((adjustedSeedX + adjustedSeedZ) ^ g->getWorldSeed());

            addFeature(primer, current, target, accurate);
        }
    }
}