#include "Chunk.hpp"


namespace Chunk {


    template<bool checkWaterCaves, bool generateCaves, bool generateRavines, bool generateSkyLight>
    ChunkPrimer* provideChunk(const Generator& g, const int chunkX, const int chunkZ, const bool accurate) {
        ChunkGeneratorOverWorld chunk(g);
        ChunkPrimer *chunkPrimer = chunk.provideChunk(chunkX, chunkZ);
        chunkPrimer->decorateRng = RNG::getPopulationSeed(g.getWorldSeed(), chunkX, chunkZ);

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


        chunkPrimer->stage = Stage::STAGE_STRUCTURE;


        if constexpr (generateSkyLight) {
            chunkPrimer->generateSkylightMap();
        }
        return chunkPrimer;
    }



    void populateChunk(World& world, const Generator& g, int chunkX, int chunkZ, World* worldIn) {
        const ChunkPrimer* chunk = worldIn->getChunk({chunkX, chunkZ - 1});
        const ChunkPrimer* chunk1 = worldIn->getChunk({chunkX + 1, chunkZ});
        const ChunkPrimer* chunk2 = worldIn->getChunk({chunkX, chunkZ + 1});
        const ChunkPrimer* chunk3 = worldIn->getChunk({chunkX - 1, chunkZ});

        if (chunk1 && chunk2 && worldIn->getChunk({chunkX + 1, chunkZ + 1}) != nullptr) {
            populateStructures(world, g, chunkX, chunkZ, worldIn);
            populateDecorations(world, g, chunkX, chunkZ, worldIn);
        }

        if (chunk3 && chunk2 && worldIn->getChunk({chunkX - 1, chunkZ + 1}) != nullptr) {
            populateStructures(world, g, chunkX - 1, chunkZ, worldIn);
            populateDecorations(world, g, chunkX - 1, chunkZ, worldIn);
        }

        if (chunk && chunk1 && worldIn->getChunk({chunkX + 1, chunkZ - 1}) != nullptr) {
            populateStructures(world, g, chunkX, chunkZ - 1, worldIn);
            populateDecorations(world, g, chunkX, chunkZ - 1, worldIn);
        }

        if (chunk && chunk3 && worldIn->getChunk({chunkX - 1, chunkZ - 1}) != nullptr) {
            populateStructures(world, g, chunkX - 1, chunkZ - 1, worldIn);
            populateDecorations(world, g, chunkX - 1, chunkZ - 1, worldIn);
        }
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
    void populateStructures(World& world, const Generator& g, int chunkX, int chunkZ, World* worldIn) {
        ChunkPrimer* chunk = worldIn->getChunk({chunkX, chunkZ});
        if (!chunk || chunk->stage != Stage::STAGE_STRUCTURE) {
            return;
        }

        constexpr bool generateMineshafts = true;
        constexpr bool generateVillages = true;
        constexpr bool generateStrongholds = true;

        auto chunkBB = BoundingBox::makeChunkBox(chunkX, chunkZ);

        if constexpr (generateMineshafts) {
            for (auto& mineshaft : world.mineshafts) {
                if (chunkBB.intersects(mineshaft.structureBB)) {
                    for (int ip = 0; ip < mineshaft.pieceArraySize; ip++) {
                        StructureComponent& piece = mineshaft.pieceArray[ip];
                        if (chunkBB.intersects(piece)) {
                            build::mineshaft::addComponentParts(world, chunk->decorateRng, chunkBB, piece);
                        }
                    }
                }
            }
        }

        if constexpr (generateVillages) {
            for (auto& village : world.villages) {
                if (chunkBB.intersects(village.structureBB)) {
                    for (int ip = 0; ip < village.pieceArraySize; ip++) {
                        StructureComponent& piece = village.pieceArray[ip];
                        if (chunkBB.intersects(piece)) {
                            piece.structureType = village.biomeType;
                            build::village::addComponentParts(world, chunk->decorateRng, chunkBB, piece);
                        }
                    }
                }
            }
        }

        if constexpr (generateStrongholds) {
            for (auto& stronghold : world.strongholds) {
                if (chunkBB.intersects(stronghold.structureBB)) {
                    for (int ip = 0; ip < stronghold.pieceArraySize; ip++) {
                        StructureComponent& piece = stronghold.pieceArray[ip];
                        if (chunkBB.intersects(piece)) {
                            build::stronghold::addComponentParts(world, chunk->decorateRng, chunkBB, piece);
                        }
                    }
                }
            }
        }


        chunk->stage = Stage::STAGE_DECORATE;
    }



    void populateDecorations(World& world, const Generator& g, int chunkX, int chunkZ, World* worldIn) {
        ChunkPrimer* chunk = worldIn->getChunk({chunkX, chunkZ});
        if (!chunk || chunk->stage != Stage::STAGE_DECORATE) {
            return;
        }

        // return;
        // std::cout << "Populating chunk " << chunkX << ", " << chunkZ << std::endl;
        // RNG rngPopulate = RNG::getPopulationSeed(g.getWorldSeed(), chunkX, chunkZ);


        if (const Pos3D waterPos = FeaturePositions::waterLake(&g, chunk->decorateRng, chunkX, chunkZ); !waterPos.isNull()) {
            const WorldGenLakes waterGen(&g, &lce::blocks::BlocksInit::STILL_WATER);
            waterGen.generate(worldIn, chunk->decorateRng, waterPos);
        }

        if (const Pos3D lavaPos = FeaturePositions::lavaLake(chunk->decorateRng, chunkX, chunkZ); !lavaPos.isNull()) {
            const WorldGenLakes lavaGen(&g, &lce::blocks::BlocksInit::STILL_LAVA);
            lavaGen.generate(worldIn, chunk->decorateRng, lavaPos);
        }

        for (int i = 0; i < 8; i++) {
            if (Pos3D pos = FeaturePositions::dungeon(chunk->decorateRng, chunkX, chunkZ); !pos.isNull()) {
                WorldGenDungeons dungeonGen;
                dungeonGen.generate(worldIn, chunk->decorateRng, pos);
            }
        }

        Biome::registry[g.getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16)]->decorate(
                worldIn, chunk->decorateRng, {chunkX << 4, chunkZ << 4});


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


    template ChunkPrimer* provideChunk<true, true, true, false>(const Generator &, c_int, c_int, bool);
    template ChunkPrimer* provideChunk<false, true, true, false>(const Generator &, c_int, c_int, bool);


}