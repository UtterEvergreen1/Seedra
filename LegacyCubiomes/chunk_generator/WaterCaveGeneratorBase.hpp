#pragma once

#include "MapGenBase.hpp"

class WaterCaveGeneratorBase : public MapGenBase {

public:
    explicit WaterCaveGeneratorBase(const Generator& generator) : MapGenBase(generator) {}

    WaterCaveGeneratorBase(const lce::CONSOLE console, const LCEVERSION version,
        c_i64 seed, const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}

    ~WaterCaveGeneratorBase() override = default;

    void generateUnderwater(c_int targetX, int const targetZ, ChunkPrimer* primer, bool accurate = true) {
        rng.setSeed(g.getWorldSeed());
        c_int seedMultiplierX = rng.nextInt() / 2 * 2 + 1;
        c_int seedMultiplierZ = rng.nextInt() / 2 * 2 + 1;

        for (int currentX = targetX - range; currentX <= targetX + range; ++currentX) {
            for (int currentZ = targetZ - range; currentZ <= targetZ + range; ++currentZ) {
                c_int adjustedSeedX = currentX * seedMultiplierX;
                c_int adjustedSeedZ = currentZ * seedMultiplierZ;
                rng.setSeed(adjustedSeedX + adjustedSeedZ ^ g.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, primer, accurate);
            }
        }
    }
};