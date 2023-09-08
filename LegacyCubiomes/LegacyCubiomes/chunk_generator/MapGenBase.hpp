#pragma once

#include "ChunkPrimer.hpp"

class MapGenBase
{
public:
    const int range = 8;
    Generator g;
    uint64_t rng;
    explicit MapGenBase(const Generator& generator) : g(generator), rng(0) {}
    MapGenBase(int64_t worldSeed, BIOMESCALE biomeScale) : g(WIIU_LATEST, biomeScale), rng(0) {
        g.applySeed(DIMENSIONS::OVERWORLD, worldSeed);
    }

    void generate(int targetX, int targetZ, ChunkPrimer* primer) {
        setSeed(&rng, g.seed);
        int64_t seedMultiplierX = (int64_t)nextLong(&rng);
        int64_t seedMultiplierZ = (int64_t)nextLong(&rng);

        for (int currentX = targetX - this->range; currentX <= targetX + this->range; ++currentX) {
            for (int currentZ = targetZ - this->range; currentZ <= targetZ + this->range; ++currentZ) {
                int64_t adjustedX = (int64_t)currentX * seedMultiplierX;
                int64_t adjustedZ = (int64_t)currentZ * seedMultiplierZ;
                setSeed(&rng, adjustedX ^ adjustedZ ^ g.seed);

                recursiveGenerate(currentX, currentZ, targetX, targetZ, primer);
            }
        }
    }

    virtual void recursiveGenerate(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) = 0;
};
