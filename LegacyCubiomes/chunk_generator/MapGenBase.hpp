#pragma once

#include "ChunkPrimer.hpp"


class MapGenBase {
public:
    static constexpr int range = 8;
    Generator g;
    RNG rng;

    explicit MapGenBase(const Generator& generator) : g(generator), rng(0) {}

    MapGenBase(const CONSOLE console, const LCEVERSION version,
        const int64_t seed, const WORLDSIZE size, const BIOMESCALE scale)
        : g(console, version, seed, size, scale), rng(0) {}

    virtual ~MapGenBase() = default;

    void generate(const int targetX, const int targetZ, ChunkPrimer* primer, bool accurate = true) {
        rng.setSeed(g.getWorldSeed());
        const auto seedMultiplierX = rng.nextLongI();
        const auto seedMultiplierZ = rng.nextLongI();

        for (int currentX = targetX - range; currentX <= targetX + range; ++currentX) {
            for (int currentZ = targetZ - range; currentZ <= targetZ + range; ++currentZ) {
                const auto adjustedX = (int64_t) currentX * seedMultiplierX;
                const auto adjustedZ = (int64_t) currentZ * seedMultiplierZ;
                rng.setSeed(adjustedX ^ adjustedZ ^ g.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, primer, accurate);
            }
        }
    }

    virtual void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer, bool accurate) = 0;
};
