#include "Chunk.hpp"
#include "World.hpp"

#include "structures/gen/village/village.hpp"
#include "structures/placement/mineshaft.hpp"
#include "structures/placement/stronghold.hpp"

#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/carve/ChunkGenerator.hpp"
#include "terrain/carve/RavineGenerator.hpp"
#include "terrain/carve/WaterCaveGenerator.hpp"
#include "terrain/carve/WaterRavineGenerator.hpp"

#include "terrain/decorators/WorldGenDungeons.hpp"
#include "terrain/decorators/WorldGenLakes.hpp"

namespace Chunk {
    void provideChunk(ChunkPrimer *chunkPrimer, const Generator &g, const Pos2D& chunkPos) {
        ChunkGeneratorOverWorld chunk(g);
        chunkPrimer->isModifying.store(true);
        chunk.provideChunk(chunkPrimer, chunkPos);
        chunkPrimer->isModifying.store(false);
    }

    ChunkPrimer* provideNewChunk(const Generator& g, const Pos2D& chunkPos) {
        auto* chunkPrimer = new ChunkPrimer();
        provideChunk(chunkPrimer, g, chunkPos);
        return chunkPrimer;
    }



    MU void populateCaves(World& world, Pos2D chunkPos) {
        ChunkPrimer* chunkPrimer = world.getChunk(chunkPos);
        if (chunkPrimer->isModifying.load() || chunkPrimer->stage != Stage::STAGE_WATER_CAVES) {
            return;
        }

        chunkPrimer->isModifying.store(true);
        bool accurate = true;



        
        
        if (world.getGenerator()->getLCEVersion() != LCEVERSION::AQUATIC &&
            (chunkPrimer->stage == Stage::STAGE_TERRAIN ||
            chunkPrimer->stage == Stage::STAGE_WATER_CAVES || 
            chunkPrimer->stage == Stage::STAGE_WATER_RAVINES)) {
            chunkPrimer->stage = Stage::STAGE_CAVES;
        }
        
        const Pos2D seedMultiplierWaterCaves = AbstractWaterCaveGen::getSeedMultiplier(world.getGenerator());
        if (chunkPrimer->stage == Stage::STAGE_WATER_CAVES) {
            chunkPrimer->stage = Stage::STAGE_WATER_RAVINES;
            WaterCaveGenerator waterCaveGenerator(world);
            waterCaveGenerator.setupRNG(seedMultiplierWaterCaves, chunkPos);
            waterCaveGenerator.generateUnderwater(chunkPrimer, chunkPos, accurate);
        }
        

        if (chunkPrimer->stage == Stage::STAGE_WATER_RAVINES) {
            chunkPrimer->stage = Stage::STAGE_CAVES;
            WaterRavineGenerator waterRavineGenerator(world);
            waterRavineGenerator.setupRNG(seedMultiplierWaterCaves, chunkPos);
            waterRavineGenerator.generateUnderwater(chunkPrimer, chunkPos, accurate);
        }
        
        const Pos2DTemplate<i64> seedMultiplierCaves = AbstractMapGen::getSeedMultiplier(world.getGenerator());
        if (chunkPrimer->stage == Stage::STAGE_CAVES) {
            chunkPrimer->stage = Stage::STAGE_RAVINES;
            CaveGenerator caveGenerator(world);
            caveGenerator.setupRNG(seedMultiplierCaves, chunkPos);
            caveGenerator.generate(chunkPrimer, chunkPos, accurate);
        }
        
        
        if (chunkPrimer->stage == Stage::STAGE_RAVINES) {
            chunkPrimer->stage = Stage::STAGE_STRUCTURE;
            RavineGenerator ravineGenerator(world);
            ravineGenerator.setupRNG(seedMultiplierCaves, chunkPos);
            ravineGenerator.generate(chunkPrimer, chunkPos, accurate);
        }
        chunkPrimer->isModifying.store(false);

        // chunkPrimer->generateSkylightMap();
    }


    MU void populateLight(World& world, Pos2D chunkPos) {
        ChunkPrimer* chunkPrimer = world.getChunk({chunkPos.x, chunkPos.z});

        chunkPrimer->generateSkylightMap();
        chunkPrimer->stage = Stage::STAGE_STRUCTURE;
    }



    void populateChunk(World& world, Pos2D chunkPos) {
        const ChunkPrimer* chunk = world.getChunk({chunkPos.x, chunkPos.z - 1});
        const ChunkPrimer* chunk1 = world.getChunk({chunkPos.x + 1, chunkPos.z});
        const ChunkPrimer* chunk2 = world.getChunk({chunkPos.x, chunkPos.z + 1});
        const ChunkPrimer* chunk3 = world.getChunk({chunkPos.x - 1, chunkPos.z});
        // Checks if caves are done on adj chunks before decorating

        //south-east
        if (chunk1 && chunk2 && chunk1->stage >= Stage::STAGE_STRUCTURE && chunk2->stage >= Stage::STAGE_STRUCTURE) {
            ChunkPrimer* chunk4 = world.getChunk({chunkPos.x + 1, chunkPos.z + 1});
            if (chunk4 != nullptr && chunk4->stage >= Stage::STAGE_STRUCTURE) {
                // populateCaves(world, chunkPos);
                // populateLight(world, g, chunkPos.x, chunkPos.z);
                populateStructures(world, chunkPos);
                populateDecorations(world, chunkPos);
            }
        }

        //south-west
        if (chunk3 && chunk2 && chunk3->stage >= Stage::STAGE_STRUCTURE && chunk2->stage >= Stage::STAGE_STRUCTURE) {
            ChunkPrimer* chunk4 = world.getChunk({chunkPos.x - 1, chunkPos.z + 1});
            if (chunk4 != nullptr && chunk4->stage >= Stage::STAGE_STRUCTURE) {
                // populateCaves(world, chunkPos);
                // populateLight(world, g, chunkPos.x, chunkPos.z);
                Pos2D newPos(chunkPos.x - 1, chunkPos.z);
                populateStructures(world, newPos);
                populateDecorations(world, newPos);
            }
        }

        //north-east
        if (chunk && chunk1 && chunk->stage >= Stage::STAGE_STRUCTURE && chunk1->stage >= Stage::STAGE_STRUCTURE) {
            ChunkPrimer* chunk4 = world.getChunk({chunkPos.x + 1, chunkPos.z - 1});
            if (chunk4 != nullptr && chunk4->stage >= Stage::STAGE_STRUCTURE) {
                // populateCaves(world, chunkPos);
                // populateLight(world, g, chunkPos.x, chunkPos.z);
                Pos2D newPos(chunkPos.x, chunkPos.z - 1);
                populateStructures(world, newPos);
                populateDecorations(world, newPos);
            }
        }

        //north-west
        if (chunk && chunk3 && chunk->stage >= Stage::STAGE_STRUCTURE && chunk3->stage >= Stage::STAGE_STRUCTURE) {
            ChunkPrimer* chunk4 = world.getChunk({chunkPos.x - 1, chunkPos.z - 1});
            if (chunk4 != nullptr && chunk4->stage >= Stage::STAGE_STRUCTURE) {
                // populateCaves(world, chunkPos);
                // populateLight(world, g, chunkPos.x, chunkPos.z);
                Pos2D newPos(chunkPos.x - 1, chunkPos.z - 1);
                populateStructures(world, newPos);
                populateDecorations(world, newPos);
            }
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
    void populateStructures(World& world, Pos2D chunkPos) {
        ChunkPrimer* chunk = world.getChunk(chunkPos);
        if (!chunk || chunk->isModifying.load() || chunk->stage != Stage::STAGE_STRUCTURE) {
            return;
        }
        chunk->isModifying.store(true);

        chunk->decorateRng = RNG::getPopulationSeed(world.getGenerator()->getWorldSeed(), chunkPos.x, chunkPos.z);

        auto chunkBB = BoundingBox::makeChunkBox(chunkPos.x, chunkPos.z);

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
        chunk->isModifying.store(false);
    }



    void populateDecorations(World& world, Pos2D chunkPos) {

        ChunkPrimer* chunk = world.getChunk(chunkPos);
        if (!chunk || chunk->isModifying.load() || chunk->stage != Stage::STAGE_DECORATE) {
            return;
        }
        chunk->isModifying.store(true);

        Generator* g = world.getGenerator();
        if (const Pos3D waterPos = FeaturePositions::waterLake(
                    g, chunk->decorateRng, chunkPos.x, chunkPos.z); !waterPos.isNull()) {
            const WorldGenLakes waterGen(g, lce::BlocksInit::STILL_WATER.getState());
            waterGen.generate(&world, chunk->decorateRng, waterPos);
        }

        if (const Pos3D lavaPos = FeaturePositions::lavaLake(
                    chunk->decorateRng, chunkPos.x, chunkPos.z); !lavaPos.isNull()) {
            const WorldGenLakes lavaGen(g, lce::BlocksInit::STILL_LAVA.getState());
            lavaGen.generate(&world, chunk->decorateRng, lavaPos);
        }

        for (int i = 0; i < 8; i++) {
            if (Pos3D pos = FeaturePositions::dungeon(
                        chunk->decorateRng, chunkPos.x, chunkPos.z); !pos.isNull()) {
                WorldGenDungeons dungeonGen;
                dungeonGen.generate(&world, chunk->decorateRng, pos);
            }
        }

        world.getBiomeAt((chunkPos.x << 4) + 16, (chunkPos.z << 4) + 16)->decorate(
                &world, chunk->decorateRng, {chunkPos.x << 4, chunkPos.z << 4});


        c_int xStart = chunkPos.x * 16 + 8;
        c_int zStart = chunkPos.z * 16 + 8;
        for (int xPos = 0; xPos < 16; ++xPos) {
            for (int zPos = 0; zPos < 16; ++zPos) {
                c_int x = xStart + xPos;
                c_int z = zStart + zPos;
                // Skip snow and ice generation if the biome is never cold enough
                if (!world.isSnowyAt(x, z)) continue;

                c_int precipitationHeight = world.getPrecipitationHeight(x, z);
                const Pos3D snowPos = Pos3D(x, precipitationHeight, z);
                const Pos3D waterPos = Pos3D(x, precipitationHeight - 1, z);

                if (world.canBlockFreezeWater(waterPos)) { world.setBlockId(waterPos, lce::blocks::ICE_ID);
                }

                if (world.canSnowAt(snowPos, true)) { // TODO: check light
                    world.setBlockId(snowPos, lce::blocks::SNOW_ID);
                    if (world.getBlockId(snowPos.down()) == lce::blocks::FARMLAND_ID) {
                        world.setBlock(snowPos.down(), lce::BlocksInit::DIRT.getState());
                    }
                }
            }
        }
        chunk->stage = Stage::STAGE_DONE;
        chunk->isModifying.store(false);
    }

}
