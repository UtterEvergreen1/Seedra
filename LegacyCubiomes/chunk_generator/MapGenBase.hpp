#pragma once

#include "ChunkPrimer.hpp"


class MapGenBase {
public:
    static constexpr int range = 8;
    Generator g;
    RNG rng;

    explicit MapGenBase(const Generator& generator) : g(generator), rng(0) {}

    MapGenBase(const lce::CONSOLE console, const LCEVERSION version,
        c_i64 seed, const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
        : g(console, version, seed, size, scale), rng(0) {}

    virtual ~MapGenBase() = default;

    void generate(c_int targetX, c_int targetZ, ChunkPrimer* primer, bool accurate = true) {
        rng.setSeed(g.getWorldSeed());
        c_auto seedMultiplierX = rng.nextLongI();
        c_auto seedMultiplierZ = rng.nextLongI();

        for (int currentX = targetX - range; currentX <= targetX + range; ++currentX) {
            for (int currentZ = targetZ - range; currentZ <= targetZ + range; ++currentZ) {
                c_auto adjustedX = (i64) currentX * seedMultiplierX;
                c_auto adjustedZ = (i64) currentZ * seedMultiplierZ;
                rng.setSeed(adjustedX ^ adjustedZ ^ g.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, primer, accurate);
            }
        }
    }

    virtual void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ,
                            ChunkPrimer* chunkPrimer, bool accurate) = 0;
};
