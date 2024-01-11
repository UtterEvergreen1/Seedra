#pragma once

#include "ChunkPrimer.hpp"

#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

class MapGenBase {
public:
    const int range = 8;
    Generator g;
    RNG rng;

    explicit MapGenBase(const Generator& generator) : g(generator), rng(0) {}

    MapGenBase(CONSOLE console, LCEVERSION version, int64_t seed, WORLDSIZE size, BIOMESCALE scale)
        : g(console, version, seed, size, scale), rng(0) {}

    void generate(int chunkX, int chunkZ, ChunkPrimer* primer) {
        rng.setSeed(g.getWorldSeed());
        auto seedMultiplierX = (int64_t) rng.nextLong();
        auto seedMultiplierZ = (int64_t) rng.nextLong();

        int lower_x = chunkX - this->range;
        int upper_x = chunkX + this->range;
        int lower_z = chunkZ - this->range;
        int upper_z = chunkZ + this->range;

        for (int currentX = lower_x; currentX <= upper_x; ++currentX) {
            for (int currentZ = lower_z; currentZ <= upper_z; ++currentZ) {
                auto adjustedX = (int64_t) currentX * seedMultiplierX;
                auto adjustedZ = (int64_t) currentZ * seedMultiplierZ;
                rng.setSeed(adjustedX ^ adjustedZ ^ g.getWorldSeed());

                recursiveGenerate(currentX, currentZ, chunkX, chunkZ, primer);
            }
        }
    }

    virtual void recursiveGenerate(int baseChunkX, int baseChunkZ,
                                   int targetX, int targetZ,
                                   ChunkPrimer* chunkPrimer) = 0;
};
