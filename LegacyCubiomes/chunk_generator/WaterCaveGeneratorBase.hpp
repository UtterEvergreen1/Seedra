#pragma once
#include "MapGenBase.hpp"
class WaterCaveGeneratorBase : public MapGenBase {
public:
    explicit WaterCaveGeneratorBase(const Generator& generator) : MapGenBase(generator) {}

    WaterCaveGeneratorBase(CONSOLE console, LCEVERSION version, int64_t seed, WORLDSIZE size, BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}

    void generateUnderwater(int targetX, int targetZ, ChunkPrimer* primer) {
        rng.setSeed(g.getWorldSeed());
        int seedMultiplierX = (rng.nextInt() / 2) * 2 + 1;
        int seedMultiplierZ = (rng.nextInt() / 2) * 2 + 1;
        //int seedMultiplierX = rng.nextInt() | 1;
        //int seedMultiplierZ = rng.nextInt() | 1;

        for (int currentX = targetX - MapGenBase::range; currentX <= targetX + MapGenBase::range; ++currentX) {
            for (int currentZ = targetZ - MapGenBase::range; currentZ <= targetZ + MapGenBase::range; ++currentZ) {
                int adjustedSeedX = currentX * seedMultiplierX;
                int adjustedSeedZ = currentZ * seedMultiplierZ;
                rng.setSeed((adjustedSeedX + adjustedSeedZ) ^ g.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, primer);
            }
        }
    }
};