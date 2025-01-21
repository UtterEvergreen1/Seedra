#pragma once

#include "AbstractMapGen.hpp"

class AbstractWaterCaveGen : public AbstractMapGen {

public:
    explicit AbstractWaterCaveGen(const Generator& generator) : AbstractMapGen(generator) {}

    AbstractWaterCaveGen(const lce::CONSOLE console, const LCEVERSION version,
        c_i64 seed, const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
        : AbstractMapGen(console, version, seed, size, scale) {}

    ~AbstractWaterCaveGen() override = default;

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