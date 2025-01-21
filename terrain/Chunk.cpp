#include "Chunk.hpp"

#include "structures/gen/village/village.hpp"
#include "structures/placement/mineshaft.hpp"
#include "structures/placement/stronghold.hpp"

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


    void populateChunk(World& world, const Generator& g, int chunkX, int chunkZ) {
        const ChunkPrimer* chunk = world.getChunk({chunkX, chunkZ - 1});
        const ChunkPrimer* chunk1 = world.getChunk({chunkX + 1, chunkZ});
        const ChunkPrimer* chunk2 = world.getChunk({chunkX, chunkZ + 1});
        const ChunkPrimer* chunk3 = world.getChunk({chunkX - 1, chunkZ});

        //south east
        if (chunk1 && chunk2 && world.getChunk({chunkX + 1, chunkZ + 1}) != nullptr) {
            populateStructures(world, g, chunkX, chunkZ);
            populateDecorations(world, g, chunkX, chunkZ);
        }

        //south west
        if (chunk3 && chunk2 && world.getChunk({chunkX - 1, chunkZ + 1}) != nullptr) {
            populateStructures(world, g, chunkX - 1, chunkZ);
            populateDecorations(world, g, chunkX - 1, chunkZ);
        }

        //north east
        if (chunk && chunk1 && world.getChunk({chunkX + 1, chunkZ - 1}) != nullptr) {
            populateStructures(world, g, chunkX, chunkZ - 1);
            populateDecorations(world, g, chunkX, chunkZ - 1);
        }

        //north west
        if (chunk && chunk3 && world.getChunk({chunkX - 1, chunkZ - 1}) != nullptr) {
            populateStructures(world, g, chunkX - 1, chunkZ - 1);
            populateDecorations(world, g, chunkX - 1, chunkZ - 1);
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
    void populateStructures(World& world, MU const Generator& g, int chunkX, int chunkZ) {
        ChunkPrimer* chunk = world.getChunk({chunkX, chunkZ});
        if (!chunk || chunk->stage != Stage::STAGE_STRUCTURE) {
            return;
        }

        auto chunkBB = BoundingBox::makeChunkBox(chunkX, chunkZ);

        if (GENERATE_MINESHAFTS) {
            for (auto& mineshaft : world.mineshafts) {
                if (chunkBB.intersects(mineshaft.getStructureBB())) {
                    for (int ip = 0; ip < mineshaft.getPieceCount(); ip++) {
                        StructureComponent& piece = mineshaft.getPiece(ip);
                        if (chunkBB.intersects(piece)) {
                            build::mineshaft::addComponentParts(world, chunk->decorateRng, chunkBB, piece);
                        }
                    }
                }
            }
        }

        if (GENERATE_VILLAGES) {
            for (auto& village : world.villages) {
                if (chunkBB.intersects(village.getStructureBB())) {
                    for (int ip = 0; ip < village.getPieceCount(); ip++) {
                        auto& piece = village.getPiece(ip);
                        if (chunkBB.intersects(piece)) {
                            build::village::addComponentParts(world, chunk->decorateRng, chunkBB, piece);
                        }
                    }
                }
            }
        }

        if (GENERATE_STRONGHOLDS) {
            for (auto& stronghold : world.strongholds) {
                if (chunkBB.intersects(stronghold.getStructureBB())) {
                    for (int ip = 0; ip < stronghold.getPieceCount(); ip++) {
                        StructureComponent& piece = stronghold.getPiece(ip);
                        if (chunkBB.intersects(piece)) {
                            build::stronghold::addComponentParts(world, chunk->decorateRng, chunkBB, piece);
                        }
                    }
                }
            }
        }


        chunk->stage = Stage::STAGE_DECORATE;
    }



    void populateDecorations(World& world, const Generator& g, int chunkX, int chunkZ) {
        ChunkPrimer* chunk = world.getChunk({chunkX, chunkZ});
        if (!chunk || chunk->stage != Stage::STAGE_DECORATE) {
            return;
        }

        if (const Pos3D waterPos = FeaturePositions::waterLake(&g, chunk->decorateRng, chunkX, chunkZ); !waterPos.isNull()) {
            const WorldGenLakes waterGen(&g, &lce::BlocksInit::STILL_WATER);
            waterGen.generate(&world, chunk->decorateRng, waterPos);
        }

        if (const Pos3D lavaPos = FeaturePositions::lavaLake(chunk->decorateRng, chunkX, chunkZ); !lavaPos.isNull()) {
            const WorldGenLakes lavaGen(&g, &lce::BlocksInit::STILL_LAVA);
            lavaGen.generate(&world, chunk->decorateRng, lavaPos);
        }

        for (int i = 0; i < 8; i++) {
            if (Pos3D pos = FeaturePositions::dungeon(chunk->decorateRng, chunkX, chunkZ); !pos.isNull()) {
                WorldGenDungeons dungeonGen;
                dungeonGen.generate(&world, chunk->decorateRng, pos);
            }
        }

        Biome::registry[g.getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16)]->decorate(
                &world, chunk->decorateRng, {chunkX << 4, chunkZ << 4});


        c_int xStart = chunkX * 16 + 8;
        c_int zStart = chunkZ * 16 + 8;
        for (int xPos = 0; xPos < 16; ++xPos) {
            for (int zPos = 0; zPos < 16; ++zPos) {
                c_int x = xStart + xPos;
                c_int z = zStart + zPos;

                c_int precipitationHeight = world.getPrecipitationHeight(x, z);
                const Pos3D snowPos = Pos3D(x, precipitationHeight, z);
                const Pos3D waterPos = Pos3D(x, precipitationHeight - 1, z);

                if (world.canBlockFreezeWater(waterPos)) {
                    world.setBlock(waterPos, lce::blocks::ICE_ID);
                }

                if (world.canSnowAt(snowPos, false)) { // TODO: check light
                    world.setBlock(snowPos, lce::blocks::SNOW_ID);
                }
            }
        }
        chunk->stage = Stage::STAGE_DONE;
    }

}


template ChunkPrimer* Chunk::provideChunk<true, true, true, false>(const Generator &, c_int, c_int, bool);
template ChunkPrimer* Chunk::provideChunk<false, true, true, false>(const Generator &, c_int, c_int, bool);
