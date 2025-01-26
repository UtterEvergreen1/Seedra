#include "AbstractMapGen.hpp"



MU Pos2DTemplate<i64> AbstractMapGen::getSeedMultiplier(const Generator* g) {
    RNG rng;
    rng.setSeed(g->getWorldSeed());
    Pos2DTemplate<i64> seedMultiplier;
    seedMultiplier.x = rng.nextLongI();
    seedMultiplier.z = rng.nextLongI();

    return seedMultiplier;
}


MU void AbstractMapGen::setupRNG(Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos) {
    c_auto adjustedX = (i64) chunkPos.x * seedMultiplier.x;
    c_auto adjustedZ = (i64) chunkPos.z * seedMultiplier.z;
    rng.setSeed(adjustedX ^ adjustedZ ^ g.getWorldSeed());
}


MU void AbstractMapGen::setupRNG(const Generator* g, RNG& rng, Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos) {
    c_auto adjustedX = (i64) chunkPos.x * seedMultiplier.x;
    c_auto adjustedZ = (i64) chunkPos.z * seedMultiplier.z;
    rng.setSeed(adjustedX ^ adjustedZ ^ g->getWorldSeed());
}