#pragma once

#include "ChunkPrimer.hpp"
#include "ChunkGenerator.hpp"
#include "CaveGenerator.hpp"
#include "RavineGenerator.hpp"

namespace Chunk {
    template<bool generateCaves, bool generateRavines, bool generateSkyLight>
    static ChunkPrimer* provideChunk(const Generator& g, int chunkX, int chunkZ) {
        ChunkGeneratorOverWorld chunk(g);
        ChunkPrimer* chunkPrimer = chunk.provideChunk(chunkX, chunkZ);
        if constexpr (generateCaves) {
            CaveGenerator caveGenerator = CaveGenerator(g);
            caveGenerator.generate(chunkX, chunkZ, chunkPrimer);
        }
        if constexpr (generateRavines) {
            RavineGenerator ravineGenerator = RavineGenerator(g);
            ravineGenerator.generate(chunkX, chunkZ, chunkPrimer);
        }
        if constexpr (generateSkyLight) {
            chunkPrimer->generateSkylightMap();
        }
        return chunkPrimer;
    }

    MU static void populate(const Generator& g, int chunkX, int chunkZ, ChunkPrimer *chunkData) {
        int xStart = chunkX * 16;
        int zStart = chunkZ * 16;
        for (int xPos = 0; xPos < 16; ++xPos) {
            for (int zPos = 0; zPos < 16; ++zPos) {
                int x = xStart + xPos;
                int z = zStart + zPos;

                int chunkPosX = x & 15;
                int chunkPosZ = z & 15;

                int precipitationHeight = chunkData->getPrecipitationHeight(x, z);
                Pos3D blockPos1 = Pos3D(x, precipitationHeight, z);
                Pos3D blockPos2 = Pos3D(x, precipitationHeight - 1, z);

                if (chunkData->canBlockFreeze(g, blockPos2, false)) {
                    chunkData->setBlock(chunkPosX, blockPos2.getY(), chunkPosZ, Items::ICE_ID);
                }

                if (chunkData->canSnowAt(g, blockPos1, true)) {
                    chunkData->setBlock(chunkPosX, precipitationHeight, chunkPosZ, Items::SNOW_ID);
                }
            }
        }
    }
}
