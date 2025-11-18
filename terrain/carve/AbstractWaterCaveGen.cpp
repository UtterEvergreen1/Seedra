#include "AbstractWaterCaveGen.hpp"


AbstractWaterCaveGen::~AbstractWaterCaveGen() = default;


MU Pos2D AbstractWaterCaveGen::getSeedMultiplier(const Generator* g) {
    RNG rng;
    rng.setSeed(static_cast<u64>(g->getWorldSeed()));
    Pos2D seedMultiplier;
    seedMultiplier.x = rng.nextInt() / 2 * 2 + 1;
    seedMultiplier.z = rng.nextInt() / 2 * 2 + 1;
    return seedMultiplier;
}


MU void AbstractWaterCaveGen::setupRNG(const Pos2D seedMultiplier, const Pos2D chunkPos) {
    c_int adjustedX = chunkPos.x * seedMultiplier.x;
    c_int adjustedZ = chunkPos.z * seedMultiplier.z;

    m_rng.setSeed(static_cast<u64>((adjustedX + adjustedZ) ^ m_g->getWorldSeed()));
}


MU void AbstractWaterCaveGen::setupRNG(const Generator* g, RNG& rng,
    const Pos2D seedMultiplier, const Pos2D chunkPos) {
    c_int adjustedX = chunkPos.x * seedMultiplier.x;
    c_int adjustedZ = chunkPos.z * seedMultiplier.z;

    rng.setSeed(static_cast<u64>((adjustedX + adjustedZ) ^ g->getWorldSeed()));
}

void AbstractWaterCaveGen::generateUnderwater(ChunkPrimer *primer, Pos2D target, bool accurate) {
    m_rng.setSeed(static_cast<u64>(m_g->getWorldSeed()));
    c_int seedMultiplierX = m_rng.nextInt() / 2 * 2 + 1;
    c_int seedMultiplierZ = m_rng.nextInt() / 2 * 2 + 1;

    const Pos2D lower = target - CHUNK_RANGE;
    const Pos2D upper = target + CHUNK_RANGE;
    Pos2D chunkPos;
    for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
        for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {
            Pos2D current(chunkPos.x, chunkPos.z);

            c_int adjustedSeedX = chunkPos.x * seedMultiplierX;
            c_int adjustedSeedZ = chunkPos.z * seedMultiplierZ;
            m_rng.setSeed(static_cast<u64>((adjustedSeedX + adjustedSeedZ) ^ m_g->getWorldSeed()));

            addFeature(primer, current, target, accurate);
        }
    }
}
