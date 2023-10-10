#pragma once

#include "ChunkPrimer.hpp"

#include "LegacyCubiomes/utils/pos2D.hpp"
#include "LegacyCubiomes/utils/pos3D.hpp"
#include "LegacyCubiomes/utils/DoublePos2D.hpp"
#include "LegacyCubiomes/utils/DoublePos3D.hpp"

class MapGenBase
{
public:
    const int range = 8;
    Generator g;
    uint64_t rng;

    explicit MapGenBase(const Generator& generator) : g(generator), rng(0) {}

    MapGenBase(CONSOLE console, LCEVERSION version, int64_t seed, WORLDSIZE size, BIOMESCALE scale) :
            g(console, version, seed, size, scale), rng(0) {}

    void generate(int targetX, int targetZ, ChunkPrimer* primer) {
        setSeed(&rng, g.getWorldSeed());
        auto seedMultiplierX = (int64_t)nextLong(&rng);
        auto seedMultiplierZ = (int64_t)nextLong(&rng);

        for (int currentX = targetX - this->range; currentX <= targetX + this->range; ++currentX) {
            for (int currentZ = targetZ - this->range; currentZ <= targetZ + this->range; ++currentZ) {
                auto adjustedX = (int64_t)currentX * seedMultiplierX;
                auto adjustedZ = (int64_t)currentZ * seedMultiplierZ;
                setSeed(&rng, adjustedX ^ adjustedZ ^ g.getWorldSeed());

                recursiveGenerate(currentX, currentZ, targetX, targetZ, primer);
            }
        }
    }

    virtual void recursiveGenerate(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) = 0;
};
