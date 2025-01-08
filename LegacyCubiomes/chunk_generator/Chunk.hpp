#pragma once

#include "World.hpp"
#include "CaveGenerator.hpp"
#include "ChunkGenerator.hpp"
#include "ChunkPrimer.hpp"
#include "RavineGenerator.hpp"
#include "WaterCaveGenerator.hpp"
#include "WaterRavineGenerator.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenDungeons.hpp"

namespace Chunk {
    template<bool checkWaterCaves = false,
        bool generateCaves = true,
        bool generateRavines = true,
        bool generateSkyLight = false>
    static ChunkPrimer *provideChunk(const Generator &g, c_int chunkX, c_int chunkZ,  const bool accurate = true) {
        ChunkGeneratorOverWorld chunk(g);
        ChunkPrimer *chunkPrimer = chunk.provideChunk(chunkX, chunkZ);
        // std::cout << "Providing chunk " << chunkX << ", " << chunkZ << std::endl;
        chunkPrimer->stage = Stage::STAGE_CAVES;
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

        chunkPrimer->stage = Stage::STAGE_DECORATE;

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

    MU static void populate(const Generator &g, int chunkX, int chunkZ, World *worldIn) {
        ChunkPrimer* chunk = worldIn->getChunk({chunkX, chunkZ});
        if (!chunk || chunk->stage != Stage::STAGE_DECORATE) {
            return;
        }
        // std::cout << "Populating chunk " << chunkX << ", " << chunkZ << std::endl;

        RNG rng = RNG::getPopulationSeed(g.getWorldSeed(), chunkX, chunkZ);
        if (const Pos3D waterPos = FeaturePositions::waterLake(&g, rng, chunkX, chunkZ); !waterPos.isNull()) {
            const WorldGenLakes waterGen(&g, &lce::blocks::BlocksInit::STILL_WATER);
            waterGen.generate(worldIn, rng, waterPos);
        }

        if (const Pos3D lavaPos = FeaturePositions::lavaLake(rng, chunkX, chunkZ); !lavaPos.isNull()) {
            const WorldGenLakes lavaGen(&g, &lce::blocks::BlocksInit::STILL_LAVA);
            lavaGen.generate(worldIn, rng, lavaPos);
        }

        for (int i = 0; i < 8; i++) {
            if (Pos3D pos = FeaturePositions::dungeon(rng, chunkX, chunkZ); !pos.isNull()) {
                WorldGenDungeons dungeonGen;
                dungeonGen.generate(worldIn, rng, pos);
            }
        }

        Biome::registry[g.getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16)]->decorate(
            worldIn, rng, {chunkX << 4, chunkZ << 4});


        c_int xStart = chunkX * 16 + 8;
        c_int zStart = chunkZ * 16 + 8;
        for (int xPos = 0; xPos < 16; ++xPos) {
            for (int zPos = 0; zPos < 16; ++zPos) {
                c_int x = xStart + xPos;
                c_int z = zStart + zPos;

                c_int precipitationHeight = worldIn->getPrecipitationHeight(x, z);
                const Pos3D snowPos = Pos3D(x, precipitationHeight, z);
                const Pos3D waterPos = Pos3D(x, precipitationHeight - 1, z);

                if (worldIn->canBlockFreezeWater(waterPos)) {
                    worldIn->setBlock(waterPos, lce::blocks::ids::ICE_ID);
                }

                if (worldIn->canSnowAt(snowPos, false)) { // TODO: check light
                    worldIn->setBlock(snowPos, lce::blocks::ids::SNOW_ID);
                }
            }
        }
        chunk->stage = Stage::STAGE_DONE;
    }

    MU static void populateChunk(const Generator &g, int chunkX, int chunkZ, World *worldIn) {
        const ChunkPrimer* chunk = worldIn->getChunk({chunkX, chunkZ - 1});
        const ChunkPrimer* chunk1 = worldIn->getChunk({chunkX + 1, chunkZ});
        const ChunkPrimer* chunk2 = worldIn->getChunk({chunkX, chunkZ + 1});
        const ChunkPrimer* chunk3 = worldIn->getChunk({chunkX - 1, chunkZ});
        if (chunk1 && chunk2 && worldIn->getChunk({chunkX + 1, chunkZ + 1}) != nullptr) {
            populate(g, chunkX, chunkZ, worldIn);
        }

        if (chunk3 && chunk2 && worldIn->getChunk({chunkX - 1, chunkZ + 1}) != nullptr) {
            populate(g, chunkX - 1, chunkZ, worldIn);
        }

        if (chunk && chunk1 && worldIn->getChunk({chunkX + 1, chunkZ - 1}) != nullptr) {
            populate(g, chunkX, chunkZ - 1, worldIn);
        }

        if (chunk && chunk3 && worldIn->getChunk({chunkX - 1, chunkZ - 1}) != nullptr) {
            populate(g, chunkX - 1, chunkZ - 1, worldIn);
        }
    }
}
