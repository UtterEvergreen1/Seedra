#pragma once

#include "ChunkPrimer.hpp"

#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

class MapGenBase {
public:
    static constexpr int range = 8;
    Generator g;
    RNG rng;

    explicit MapGenBase(const Generator& generator) : g(generator), rng(0) {}

    MapGenBase(CONSOLE console, LCEVERSION version, int64_t seed, WORLDSIZE size, BIOMESCALE scale)
        : g(console, version, seed, size, scale), rng(0) {}

    void generate(int targetX, int targetZ, ChunkPrimer* primer) {
        rng.setSeed(g.getWorldSeed());
        auto seedMultiplierX = (int64_t) rng.nextLong();
        auto seedMultiplierZ = (int64_t) rng.nextLong();

        for (int currentX = targetX - MapGenBase::range; currentX <= targetX + MapGenBase::range; ++currentX) {
            for (int currentZ = targetZ - MapGenBase::range; currentZ <= targetZ + MapGenBase::range; ++currentZ) {
                auto adjustedX = (int64_t) currentX * seedMultiplierX;
                auto adjustedZ = (int64_t) currentZ * seedMultiplierZ;
                rng.setSeed(adjustedX ^ adjustedZ ^ g.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, primer);
            }
        }
    }

    virtual void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ,
                                   ChunkPrimer* chunkPrimer) = 0;
};
