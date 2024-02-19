#pragma once

#include "MapGenBase.hpp"

class WaterCaveGeneratorBase : public MapGenBase {

public:
    explicit WaterCaveGeneratorBase(const Generator& generator) : MapGenBase(generator) {}

    WaterCaveGeneratorBase(const CONSOLE console, const LCEVERSION version,
        const int64_t seed, const WORLDSIZE size, const BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}

    ~WaterCaveGeneratorBase() override = default;

    void generateUnderwater(const int targetX, int const targetZ, ChunkPrimer* primer, bool accurate = true) {
        rng.setSeed(g.getWorldSeed());
        const int seedMultiplierX = rng.nextInt() / 2 * 2 + 1;
        const int seedMultiplierZ = rng.nextInt() / 2 * 2 + 1;

        for (int currentX = targetX - range; currentX <= targetX + range; ++currentX) {
            for (int currentZ = targetZ - range; currentZ <= targetZ + range; ++currentZ) {
                const int adjustedSeedX = currentX * seedMultiplierX;
                const int adjustedSeedZ = currentZ * seedMultiplierZ;
                rng.setSeed(adjustedSeedX + adjustedSeedZ ^ g.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, primer, accurate);
            }
        }
    }
};