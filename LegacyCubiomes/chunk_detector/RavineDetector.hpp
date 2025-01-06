#pragma once

#include <cmath>
#include <iomanip>
#include <set>
#include <string>
#include <vector>

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/timer.hpp"
#include "lce/blocks/block.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/registry/blockRegistry.hpp"


class RavineDetector {
public:
    static constexpr int RANGE = 8;
    Generator myG;
    RNG myRNG;

    std::vector<float> rs = std::vector(128, 0.0F);

    std::set<Pos2D> myRavineChunks;
    int myMinChunkX = 0;
    int myMaxChunkX = 0;
    int myMinChunkZ = 0;
    int myMaxChunkZ = 0;
    float myTimeTaken = 0;

    explicit RavineDetector(const Generator& generator) : myG(generator), myRNG(0) {}

    ~RavineDetector() = default;

    std::set<Pos2D> getAllRavinesInRange(int minChunkX, int maxChunkX,
                                         int minChunkZ, int maxChunkZ,
                                         bool accurate) {
        myRavineChunks.clear();

        myMinChunkX = minChunkX;
        myMaxChunkX = maxChunkX;
        myMinChunkZ = minChunkZ;
        myMaxChunkZ = maxChunkZ;

        Timer timer;
        for (int chunkX = myMinChunkX; chunkX < myMaxChunkX; chunkX++) {
            for (int chunkZ = myMinChunkZ; chunkZ < myMaxChunkZ; chunkZ++) {
                generate(chunkX, chunkZ, accurate);
            }
        }
        myTimeTaken = timer.getSeconds();

        return myRavineChunks;
    }

private:
    void generate(c_int targetX, c_int targetZ, bool accurate = true) {
        myRNG.setSeed(myG.getWorldSeed());
        c_auto seedMultiplierX = myRNG.nextLongI();
        c_auto seedMultiplierZ = myRNG.nextLongI();

        for (int currentX = targetX - RANGE; currentX <= targetX + RANGE; ++currentX) {
            for (int currentZ = targetZ - RANGE; currentZ <= targetZ + RANGE; ++currentZ) {
                c_auto adjustedX = (i64) currentX * seedMultiplierX;
                c_auto adjustedZ = (i64) currentZ * seedMultiplierZ;
                myRNG.setSeed(adjustedX ^ adjustedZ ^ myG.getWorldSeed());

                addFeature(currentX, currentZ, targetX, targetZ, accurate);
            }
        }
    }

    void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, bool accurate);

    void addTunnel(i64 randomSeed, Pos2D chunk, DoublePos3D tunnel, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier, bool accurate);
};

