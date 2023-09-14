#pragma once

#include "ChunkPrimer.hpp"
#include "ChunkGenerator.hpp"
#include "CaveGenerator.hpp"
#include "RavineGenerator.hpp"

namespace Chunk {
    template<bool generateCaves, bool generateRavines>
    static ChunkPrimer* provideChunk(int64_t seed, int chunkX, int chunkZ, BIOMESCALE biome_scale) {
        ChunkGeneratorOverWorld chunk = ChunkGeneratorOverWorld(seed, biome_scale);
        ChunkPrimer* chunkPrimer = chunk.provideChunk(chunkX, chunkZ);
        if constexpr (generateCaves) {
            CaveGenerator caveGenerator = CaveGenerator(seed, biome_scale);
            caveGenerator.generate(chunkX, chunkZ, chunkPrimer);
        }
        if constexpr (generateRavines) {
            RavineGenerator ravineGenerator = RavineGenerator(seed, biome_scale);
            ravineGenerator.generate(chunkX, chunkZ, chunkPrimer);
        }
        return chunkPrimer;
    }

    [[maybe_unused]] static void populate(Generator* g, int64_t seed, int chunkX, int chunkZ, BIOMESCALE biome_scale, ChunkPrimer *chunkData) {
        int xStart = chunkX * 16;
        int zStart = chunkZ * 16;
        for (int xPos = 0; xPos < 16; ++xPos) {
            for (int zPos = 0; zPos < 16; ++zPos) {
                int x1 = (xStart + xPos) & 15;
                int z1 = (zStart + zPos) & 15;
                Pos3D blockPos1 = chunkData->getPrecipitationHeight(Pos2D(xStart + xPos, zStart + zPos));
                Pos3D blockPos2 = Pos3D(blockPos1.x, blockPos1.y - 1, blockPos1.z);

                if (chunkData->canBlockFreeze(g, blockPos2, false)) {
                    chunkData->setBlock(x1, blockPos2.y, z1, Items::ICE_ID);
                }

                if (chunkData->canSnowAt(g, blockPos1, true)) {
                    chunkData->setBlock(x1, blockPos1.y, z1, Items::SNOW_ID);
                }
            }
        }
    }
}
