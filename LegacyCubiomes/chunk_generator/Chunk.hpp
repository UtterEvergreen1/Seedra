#pragma once

#include "CaveGenerator.hpp"
#include "ChunkGenerator.hpp"
#include "ChunkPrimer.hpp"
#include "RavineGenerator.hpp"
#include "WaterCaveGenerator.hpp"
#include "WaterRavineGenerator.hpp"

namespace Chunk {

    template<bool checkWaterCaves = false, bool generateCaves = true, bool generateRavines = true,
             bool generateSkyLight = false>
    static ChunkPrimer* provideChunk(const Generator& g, c_int chunkX, c_int chunkZ, bool accurate = true) {
        ChunkGeneratorOverWorld chunk(g);
        ChunkPrimer* chunkPrimer = chunk.provideChunk(chunkX, chunkZ);
        if constexpr (generateCaves && checkWaterCaves) {
            if (g.getLCEVersion() == LCEVERSION::AQUATIC) {
                WaterCaveGenerator waterCaveGenerator(g);
                waterCaveGenerator.generateUnderwater(chunkX, chunkZ, chunkPrimer, accurate);
            }
        }
        if constexpr (generateRavines && checkWaterCaves) {
            if (g.getLCEVersion() == LCEVERSION::AQUATIC) {
                WaterRavineGenerator waterRavineGenerator(g);
                waterRavineGenerator.generateUnderwater(chunkX, chunkZ, chunkPrimer, accurate);
            }
        }

        if constexpr (generateCaves) {
            CaveGenerator caveGenerator(g);
            caveGenerator.generate(chunkX, chunkZ, chunkPrimer, accurate);
        }
        if constexpr (generateRavines) {
            RavineGenerator ravineGenerator(g);
            ravineGenerator.generate(chunkX, chunkZ, chunkPrimer, accurate);
        }

        /*
        structure order after caves:
        mineshaft
        village
        stronghold
        scattered_feature
        ocean ruin
        ocean monument
        woodland mansion
        shipwreck
        buried treasure
         */

        if constexpr (generateSkyLight) { chunkPrimer->generateSkylightMap(); }
        return chunkPrimer;
    }

    MU static void populate(const Generator& g, int chunkX, int chunkZ, ChunkPrimer* chunkData) {
        c_int xStart = chunkX * 16;
        c_int zStart = chunkZ * 16;
        for (int xPos = 0; xPos < 16; ++xPos) {
            for (int zPos = 0; zPos < 16; ++zPos) {
                c_int x = xStart + xPos;
                c_int z = zStart + zPos;

                c_int chunkPosX = x & 15;
                c_int chunkPosZ = z & 15;

                c_int precipitationHeight = chunkData->getPrecipitationHeight(x, z);
                const Pos3D blockPos1 = Pos3D(x, precipitationHeight, z);
                const Pos3D blockPos2 = Pos3D(x, precipitationHeight - 1, z);

                if (chunkData->canBlockFreeze(g, blockPos2, false)) {
                    chunkData->setBlockId(chunkPosX, blockPos2.getY(), chunkPosZ, lce::blocks::ids::ICE_ID);
                }

                if (chunkData->canSnowAt(g, blockPos1, true)) {
                    chunkData->setBlockId(chunkPosX, precipitationHeight, chunkPosZ, lce::blocks::ids::SNOW_ID);
                }
            }
        }
    }
}
