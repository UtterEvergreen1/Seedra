#include "AbstractMapGen.hpp"


AbstractMapGen::~AbstractMapGen() = default;


MU Pos2DTemplate<i64> AbstractMapGen::getSeedMultiplier(const Generator* g) {
    RNG rng;
    rng.setSeed(static_cast<u64>(g->getWorldSeed()));
    Pos2DTemplate<i64> seedMultiplier;
    seedMultiplier.x = rng.nextLongI();
    seedMultiplier.z = rng.nextLongI();

    return seedMultiplier;
}


MU void AbstractMapGen::setupRNG(const Pos2DTemplate<i64> seedMultiplier, const Pos2D chunkPos) {
    c_auto adjustedX = static_cast<i64>(chunkPos.x) * seedMultiplier.x;
    c_auto adjustedZ = static_cast<i64>(chunkPos.z) * seedMultiplier.z;
    m_rng.setSeed(static_cast<u64>(adjustedX ^ adjustedZ ^ m_g->getWorldSeed()));
}


MU void AbstractMapGen::setupRNG(const Generator* g, RNG& rng, const Pos2DTemplate<i64> seedMultiplier, const Pos2D chunkPos) {
    c_auto adjustedX = static_cast<i64>(chunkPos.x) * seedMultiplier.x;
    c_auto adjustedZ = static_cast<i64>(chunkPos.z) * seedMultiplier.z;
    rng.setSeed(static_cast<u64>(adjustedX ^ adjustedZ ^ g->getWorldSeed()));
}
