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

    rng.setSeed((adjustedX + adjustedZ) ^ g.getWorldSeed());
}


MU void AbstractWaterCaveGen::setupRNG(const Generator* g,
                                       RNG& rng, Pos2D seedMultiplier, Pos2D chunkPos) {
    c_int adjustedX = chunkPos.x * seedMultiplier.x;
    c_int adjustedZ = chunkPos.z * seedMultiplier.z;

    rng.setSeed((adjustedX + adjustedZ) ^ g->getWorldSeed());
}