#include "CaveGenerator.hpp"

#include "scanBoxHull.hpp"

#include <set>

#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "terrain/biomes/biome_t.hpp"

#include "lce/blocks/__include.hpp"

using namespace lce::blocks;


std::map<Pos2D, int> sectionsInChunk;


MU Pos2DVec_t CaveGenerator::getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper) {

    RNG rng;
    Pos2DTemplate<i64> seedMultiplier = getSeedMultiplier(g);

    Pos2DVec_t chunkPositions;
    c_int sizeChecked = std::abs(upper.x - lower.x) * std::abs(upper.z - lower.z);
    c_int reserveSize = (int)((float)(sizeChecked) * RESERVE_MULTIPLIER / 15.0F + 4);
    chunkPositions.reserve(reserveSize);

    lower = lower - CHUNK_RANGE;
    upper = upper + CHUNK_RANGE;

    Pos2D chunkPos;
    for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
        for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {
            CaveGenerator::setupRNG(g, rng, seedMultiplier, chunkPos);

            rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

            if EXPECT_FALSE (rng.nextInt(15) == 0) {
                chunkPositions.emplace_back(chunkPos.asType<int>());
            }
        }
    }

    return chunkPositions;
}





void CaveGenerator::addFeature(World& worldIn, Pos2D baseChunk, bool accurate) {

    c_int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

    if EXPECT_TRUE (rng.nextInt(15) != 0) { return; }

    for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
        DoublePos3D tunnelStart;
        tunnelStart.x = (double)(baseChunk.x * 16 + rng.nextInt(16));
        tunnelStart.y = (double)(rng.nextInt(rng.nextInt(120) + 8));
        tunnelStart.z = (double)(baseChunk.z * 16 + rng.nextInt(16));

        int segmentCount = 1;

        if (rng.nextInt(4) == 0) {
            addRoom(worldIn, rng.nextLongI(), baseChunk, tunnelStart, rng, accurate);
            segmentCount = rng.nextInt(4) + 1;
        }

        for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
            c_float yaw = rng.nextFloat() * (PI_FLOAT * 2.0F);
            c_float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
            float tunnelLength = rng.nextFloat() * 2.0F + rng.nextFloat();

            if (rng.nextInt(10) == 0) { tunnelLength *= rng.nextFloat() * rng.nextFloat() * 3.0F + 1.0F; }

            addTunnel(worldIn, rng.nextLongI(), baseChunk, tunnelStart, tunnelLength, yaw, pitch, 0, 0, 1.0, accurate);

        }
    }
}



void CaveGenerator::addTunnel(World& worldIn, i64 theSeedModifier, Pos2D currentChunk, DoublePos3D startPos,
                              float theWidth, float theDirection, float theSlope, int theCurrentSegment,
                              int theMaxSegment, double theHeightMultiplier, bool accurate) {

    Pos2D currentChunkX16 = currentChunk * 16;
    DoublePos2D currentChunkCenter = (currentChunkX16 + 8).asType<double>();

    float directionModifier = 0.0F;
    float slopeModifier = 0.0F;
    RNG rng;
    rng.setSeed(theSeedModifier);

    if (theMaxSegment <= 0) {
        constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
        constexpr int RANDOM_RANGE = RANGE_BOUNDARY / 4;
        theMaxSegment = RANGE_BOUNDARY - rng.nextInt(RANDOM_RANGE);
    }

    bool isMainTunnel = false;
    if (theCurrentSegment == -1) {
        theCurrentSegment = theMaxSegment / 2;
        isMainTunnel = true;
    }

    int splitPoint = rng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
    float theMaxSegmentFDivPI = PI_FLOAT / (float)(theMaxSegment);
    bool isTunnelWide = rng.nextInt(6) == 0;

SEGMENT_FOR_LOOP_START:
    for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {

        // setup tunnelWidth + tunnelHeight
        c_double adjustedWidth = 1.5 + (double) (MathHelper::sin(
            (float) theCurrentSegment * theMaxSegmentFDivPI) * theWidth);
        c_double adjustedHeight = adjustedWidth * theHeightMultiplier;

        // setup tunnel start
        {
            c_float directionCos = (MathHelper::cos(theSlope));
            startPos.x += (double) (MathHelper::cos(theDirection) * directionCos);
            startPos.y += (double) (MathHelper::sin(theSlope));
            startPos.z += (double) (MathHelper::sin(theDirection) * directionCos);
        }

        // setup tunnel slope
        if (isTunnelWide) {
            theSlope = theSlope * 0.92F;
        } else {
            theSlope = theSlope * 0.7F;
        }
        theSlope = theSlope + slopeModifier * 0.1F;
        slopeModifier = slopeModifier * 0.9F;
        {
            c_float f1_1 = rng.nextFloat();
            c_float f1_2 = rng.nextFloat();
            c_float f1_3 = rng.nextFloat();
            slopeModifier = slopeModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
        }

        // setup tunnel direction
        theDirection += directionModifier * 0.1F;
        directionModifier = directionModifier * 0.75F;
        {
            c_float f2_1 = rng.nextFloat();
            c_float f2_2 = rng.nextFloat();
            c_float f2_3 = rng.nextFloat();
            directionModifier = directionModifier + (f2_1 - f2_2) * f2_3 * 4.0F;
        }

        // split the tunnel (if not main)
        if (!isMainTunnel && theCurrentSegment == splitPoint && theWidth > 1.0F && theMaxSegment > 0) {
            float tunnelWidth1, tunnelWidth2;
            i64 seedModifier1, seedModifier2;

            if (lce::isXbox(g->getConsole())) {
                tunnelWidth1 = rng.nextFloat();
                seedModifier1 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
                seedModifier2 = rng.nextLongI();
            } else {
                seedModifier1 = rng.nextLongI();
                tunnelWidth1 = rng.nextFloat();
                seedModifier2 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
            }
            addTunnel(worldIn, seedModifier1, currentChunk, startPos, tunnelWidth1 * 0.5F + 0.5F,
                      theDirection - HALF_PI_FLOAT, theSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0, accurate);
            addTunnel(worldIn, seedModifier2, currentChunk, startPos, tunnelWidth2 * 0.5F + 0.5F,
                      theDirection + HALF_PI_FLOAT, theSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0, accurate);
            return;
        }

        // DoublePos2D distance = startPos.asPos2D() - currentChunkCenter;
        // double segmentsRemaining = theMaxSegment - theCurrentSegment;
        // double maxDistance = theWidth + 18.0F;

        // if (accurate &&
        //     g->getLCEVersion() == LCEVERSION::AQUATIC &&
        //     isOceanic(world.getBiomeIdAt((int)startPos.getX(), (int)startPos.getZ()))) { return; }

        if (!isMainTunnel && rng.nextInt(4) == 0) { continue; }

        // if (distance.distanceSq() - segmentsRemaining * segmentsRemaining > maxDistance * maxDistance) { return; }

        // if (startPos.x < currentChunkCenter.x - 16.0 - adjustedWidth * 2.0 ||
        //     startPos.z < currentChunkCenter.z - 16.0 - adjustedWidth * 2.0 ||
        //     startPos.x > currentChunkCenter.x + 16.0 + adjustedWidth * 2.0 ||
        //     startPos.z > currentChunkCenter.z + 16.0 + adjustedWidth * 2.0) { continue; }

        Pos3D min;
        Pos3D max;
        min.x = static_cast<int>(floor(startPos.x - adjustedWidth)) - currentChunkX16.x - 1;
        max.x = static_cast<int>(floor(startPos.x + adjustedWidth)) - currentChunkX16.x + 1;
        min.y = static_cast<int>(floor(startPos.y - adjustedHeight)) - 1;
        max.y = static_cast<int>(floor(startPos.y + adjustedHeight)) + 1;
        min.z = static_cast<int>(floor(startPos.z - adjustedWidth)) - currentChunkX16.z - 1;
        max.z = static_cast<int>(floor(startPos.z + adjustedWidth)) - currentChunkX16.z + 1;

        if (min.y < 1) min.y = 1;
        if (max.y > 120) max.y = 120;

        min += currentChunkX16;
        max += currentChunkX16;
        if (!genBounds.isVecInside(min) || !genBounds.isVecInside(max)) {
            continue;
        }
        Pos3D pos;

        auto foundLiquid = scanBoxHull(worldIn, min, max,
                                       [](const Pos3D& p, World& w) -> bool {
                                           const u16 id = w.getBlockId(p);
                                           return id == STILL_WATER_ID || id == FLOWING_WATER_ID;
                                       });
        if (foundLiquid) {
            goto FOUND_WATER;
        }

        min -= currentChunkX16;
        max -= currentChunkX16;

        // used to make the cpu not have to
        // cache the later code when it finds water
        goto JUMP_PAST_FOUND_WATER;
    FOUND_WATER:
        if (isMainTunnel) { return; }
        goto SEGMENT_FOR_LOOP_START;
    JUMP_PAST_FOUND_WATER:



        Pos2D center(startPos.x / 16 + (max.x - min.x) / 2,
                     startPos.z / 16 + (max.z - min.z) / 2
        );
        if (!sectionsInChunk.contains(center)) {
            sectionsInChunk[center] = 1;
        } else {
            sectionsInChunk[center]++;
        }

        /*
         * Pos2DTemplate<int>    currentChunkX16, 	4*3
         * Pos2DTemplate<double> startPos, 	8*3
         * double                adjustedWidth, 	8
         * double                adjustedHeight, 	8
         * Pos3DTemplate<int>    min, 		4*3
         * Pos3DTemplate<int>    max		4*3
         */

        c_double invW = 1.0 / adjustedWidth;
        c_double invH = 1.0 / adjustedHeight;

        // std::cout << "Range: " << max - min << " Pos: " << startPos << ", Adjusted: " << DoublePos2D (invW, invH) << "\n";

        pos.setPos(0, 0, 0);
        DoublePos3D scale;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            scale.x = ((double) (pos.x + currentChunkX16.x) + 0.5 - startPos.x) * invW;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                scale.z = ((double) (pos.z + currentChunkX16.z) + 0.5 - startPos.z) * invW;
                bool isTopBlock = false;

                if (scale.distanceSqXZ() >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    scale.y = ((double) (pos.y - 1) + 0.5 - startPos.y) * invH;

                    // 0.7 makes the floor and ceiling flatter
                    if (scale.y <= -0.7 || scale.distanceSq() >= 1.0) { continue; }

                    Pos3D blockPos = pos;
                    blockPos += currentChunkX16;

                    u16 currentBlock = worldIn.getBlockId(blockPos);
                    u16 blockAbove = worldIn.getBlockId(blockPos.up());

                    if (currentBlock == GRASS_ID || currentBlock == MYCELIUM_ID) { isTopBlock = true; }

                    if (canReplaceBlock(currentBlock, blockAbove)) {
                        if (blockPos.y < 11) {
                            worldIn.setBlockId(blockPos, STILL_LAVA_ID);
                        } else {
                            worldIn.setBlockId(blockPos, AIR_ID);
                            if (isTopBlock && worldIn.getBlockId(blockPos.down()) == DIRT_ID) {
                                worldIn.setBlockId(
                                        blockPos.down(),
                                        topBlock(blockPos.x, blockPos.z));
                            }
                        }
                    }
                }
            }
        }
        if (isMainTunnel) { return; }
    } // end of "theCurrentSegment < theMaxSegment" for loop
}


void CaveGenerator::addRoom(World& worldIn, i64 seedModifier, Pos2D currentChunk,
                            const DoublePos3D& roomStart, RNG& rng, bool accurate) {
    addTunnel(worldIn, seedModifier, currentChunk, roomStart, 1.0F + rng.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1,
              0.5, accurate);
}















unsigned char CaveGenerator::topBlock(c_int x, c_int z) const {
    switch (world.getBiomeIdAt(x, z)) {
        case biome_t::beach:
        case biome_t::desert:
        case biome_t::mesa:
            return SAND_ID;
        case biome_t::mushroom_field_shore:
        case biome_t::mushroom_fields:
            return MYCELIUM_ID;
        case biome_t::snowy_beach:
        case biome_t::snowy_mountains:
        case biome_t::snowy_taiga:
        case biome_t::snowy_taiga_hills:
        case biome_t::snowy_tundra:
            return SNOW_BLOCK_ID;
        case biome_t::stone_beach:
            return STONE_ID;
        default:
            return 2;
    }
}


bool CaveGenerator::canReplaceBlock(c_u16 blockAt, c_u16 blockAbove) {
    switch (blockAt) {
        default:
            return false;
        case STONE_ID:
        case GRASS_ID:
        case DIRT_ID:
        case SANDSTONE_ID:
        case SNOW_ID:
        case MYCELIUM_ID:
        case STAINED_HARDENED_CLAY_ID:
        case HARDENED_CLAY_ID:
        case RED_SANDSTONE_ID:
            return true;
        case SAND_ID:
        case GRAVEL_ID:
            return blockAbove != AIR_ID
                   && blockAbove != STILL_WATER_ID; // wii u?
                                                    // return blockAbove != lce::items::STILL_WATER_ID;
    }
}


void CaveGenerator::addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) {

    c_int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

    if EXPECT_TRUE (rng.nextInt(15) != 0) { return; }

    for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
        DoublePos3D tunnelStart;
        tunnelStart.x = (double)(baseChunk.x * 16 + rng.nextInt(16));
        tunnelStart.y = (double)(rng.nextInt(rng.nextInt(120) + 8));
        tunnelStart.z = (double)(baseChunk.z * 16 + rng.nextInt(16));

        int segmentCount = 1;

        if (rng.nextInt(4) == 0) {
            addRoom(chunkPrimer, rng.nextLongI(), currentChunk, tunnelStart, rng, accurate);
            segmentCount = rng.nextInt(4) + 1;
        }

        for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
            c_float yaw = rng.nextFloat() * (PI_FLOAT * 2.0F);
            c_float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
            float tunnelLength = rng.nextFloat() * 2.0F + rng.nextFloat();

            if (rng.nextInt(10) == 0) { tunnelLength *= rng.nextFloat() * rng.nextFloat() * 3.0F + 1.0F; }

            addTunnel(chunkPrimer, rng.nextLongI(), currentChunk, tunnelStart, tunnelLength, yaw, pitch, 0, 0, 1.0,
                      accurate);

        }

        // auto blockPos = tunnelStart.asPos2D().asType<int>().toChunkPos();
        // Pos3D pos(blockPos.x, (int)tunnelStart.y, blockPos.z);
        // chunkPrimer->setBlockId(pos, lce::blocks::REDSTONE_BLOCK_ID);
    }
}


void CaveGenerator::addTunnel(ChunkPrimer* chunkPrimer, i64 theSeedModifier, Pos2D currentChunk, DoublePos3D startPos,
                              float theWidth, float theDirection, float theSlope, int theCurrentSegment,
                              int theMaxSegment, double theHeightMultiplier, bool accurate) {

    Pos2D currentChunkX16 = currentChunk * 16;
    DoublePos2D currentChunkCenter = (currentChunkX16 + 8).asType<double>();

    float directionModifier = 0.0F;
    float slopeModifier = 0.0F;
    RNG rng;
    rng.setSeed(theSeedModifier);

    if (theMaxSegment <= 0) {
        constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
        constexpr int RANDOM_RANGE = RANGE_BOUNDARY / 4;
        theMaxSegment = RANGE_BOUNDARY - rng.nextInt(RANDOM_RANGE);
    }

    bool isMainTunnel = false;
    if (theCurrentSegment == -1) {
        theCurrentSegment = theMaxSegment / 2;
        isMainTunnel = true;
    }

    int splitPoint = rng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
    float maxSegmentFDivPI = PI_FLOAT / (float)(theMaxSegment);
    bool isTunnelWide = rng.nextInt(6) == 0;

SEGMENT_FOR_LOOP_START:
    for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {

        // setup tunnelWidth + tunnelHeight
        c_double adjustedWidth = 1.5 + (double) (MathHelper::sin(
                                                         (float) theCurrentSegment * maxSegmentFDivPI) * theWidth);
        c_double adjustedHeight = adjustedWidth * theHeightMultiplier;

        // setup tunnel start
        {
            c_float directionCos = (MathHelper::cos(theSlope));
            startPos.x += (double) (MathHelper::cos(theDirection) * directionCos);
            startPos.y += (double) (MathHelper::sin(theSlope));
            startPos.z += (double) (MathHelper::sin(theDirection) * directionCos);
        }

        // setup tunnel slope
        if (isTunnelWide) {
            theSlope = theSlope * 0.92F;
        } else {
            theSlope = theSlope * 0.7F;
        }
        theSlope = theSlope + slopeModifier * 0.1F;
        slopeModifier = slopeModifier * 0.9F;
        {
            c_float f1_1 = rng.nextFloat();
            c_float f1_2 = rng.nextFloat();
            c_float f1_3 = rng.nextFloat();
            slopeModifier = slopeModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
        }

        // setup tunnel direction
        theDirection += directionModifier * 0.1F;
        directionModifier = directionModifier * 0.75F;
        {
            c_float f2_1 = rng.nextFloat();
            c_float f2_2 = rng.nextFloat();
            c_float f2_3 = rng.nextFloat();
            directionModifier = directionModifier + (f2_1 - f2_2) * f2_3 * 4.0F;
        }

        // split the tunnel (if not main)
        if (!isMainTunnel && theCurrentSegment == splitPoint && theWidth > 1.0F && theMaxSegment > 0) {
            float tunnelWidth1, tunnelWidth2;
            i64 seedModifier1, seedModifier2;

            if (lce::isXbox(g->getConsole())) {
                tunnelWidth1 = rng.nextFloat();
                seedModifier1 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
                seedModifier2 = rng.nextLongI();
            } else {
                seedModifier1 = rng.nextLongI();
                tunnelWidth1 = rng.nextFloat();
                seedModifier2 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
            }
            addTunnel(chunkPrimer, seedModifier1, currentChunk, startPos, tunnelWidth1 * 0.5F + 0.5F,
                      theDirection - HALF_PI_FLOAT, theSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0, accurate);
            addTunnel(chunkPrimer, seedModifier2, currentChunk, startPos, tunnelWidth2 * 0.5F + 0.5F,
                      theDirection + HALF_PI_FLOAT, theSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0, accurate);
            return;
        }

        DoublePos2D distance = startPos.asPos2D() - currentChunkCenter;
        double segmentsRemaining = theMaxSegment - theCurrentSegment;
        double maxDistance = theWidth + 18.0F;

        if (accurate &&
            g->getLCEVersion() == LCEVERSION::AQUATIC &&
            isOceanic(world.getBiomeIdAt((int)startPos.getX(), (int)startPos.getZ()))) { return; }

        if (!isMainTunnel && rng.nextInt(4) == 0) { continue; }

        if (distance.distanceSq() - segmentsRemaining * segmentsRemaining > maxDistance * maxDistance) { return; }

        if (startPos.x < currentChunkCenter.x - 16.0 - adjustedWidth * 2.0 ||
            startPos.z < currentChunkCenter.z - 16.0 - adjustedWidth * 2.0 ||
            startPos.x > currentChunkCenter.x + 16.0 + adjustedWidth * 2.0 ||
            startPos.z > currentChunkCenter.z + 16.0 + adjustedWidth * 2.0) { continue; }

        Pos3D min;
        Pos3D max;
        min.x = static_cast<int>(floor(startPos.x - adjustedWidth)) - currentChunkX16.x - 1;
        max.x = static_cast<int>(floor(startPos.x + adjustedWidth)) - currentChunkX16.x + 1;
        min.y = static_cast<int>(floor(startPos.y - adjustedHeight)) - 1;
        max.y = static_cast<int>(floor(startPos.y + adjustedHeight)) + 1;
        min.z = static_cast<int>(floor(startPos.z - adjustedWidth)) - currentChunkX16.z - 1;
        max.z = static_cast<int>(floor(startPos.z + adjustedWidth)) - currentChunkX16.z + 1;

        if (min.x < 0) min.x = 0;
        if (max.x > 16) max.x = 16;
        if (min.y < 1) min.y = 1;
        if (max.y > 120) max.y = 120;
        if (min.z < 0) min.z = 0;
        if (max.z > 16) max.z = 16;

        Pos3D pos;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                for (pos.y = max.y + 1; pos.y >= min.y - 1; --pos.y) {
                    if (pos.y < 0 || pos.y >= 128) { continue; }
                    u16 blockType = chunkPrimer->getBlockId(pos);
                    if (pos.y != min.y - 1 &&
                        pos.x != min.x &&
                        pos.x != max.x - 1 &&
                        pos.z != min.z &&
                        pos.z != max.z - 1) {
                        pos.y = min.y;
                    }
                    c_bool hasWater = blockType == STILL_WATER_ID ||
                                      blockType == FLOWING_WATER_ID;
                    if (hasWater) {
                        goto FOUND_WATER;
                    }
                }
            }
        }

        // used to make the cpu not have to
        // cache the later code when it finds water
        goto JUMP_PAST_FOUND_WATER;
    FOUND_WATER:
        if (isMainTunnel) { return; }
        goto SEGMENT_FOR_LOOP_START;
    JUMP_PAST_FOUND_WATER:

        // ((dVar27 = (double)getDepth__5BiomeFv(uVar9), dVar27 < -0.9 && (iVar10 = isSnowCovered__5BiomeFv(uVar9), iVar10 != 0))
        pos.setPos(0, 0, 0);
        DoublePos3D scale;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            scale.x = ((double) (pos.x + currentChunkX16.x) + 0.5 - startPos.x) / adjustedWidth;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                scale.z = ((double) (pos.z + currentChunkX16.z) + 0.5 - startPos.z) / adjustedWidth;
                bool isTopBlock = false;

                if (scale.distanceSqXZ() >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    scale.y = ((double) (pos.y - 1) + 0.5 - startPos.y) / adjustedHeight;

                    if (scale.y <= -0.7 || scale.distanceSq() >= 1.0) { continue; }

                    u16 currentBlock = chunkPrimer->getBlockId(pos);
                    u16 blockAbove = chunkPrimer->getBlockId(pos.up());

                    if (currentBlock == GRASS_ID || currentBlock == MYCELIUM_ID) { isTopBlock = true; }

                    if (canReplaceBlock(currentBlock, blockAbove)) {
                        if (pos.y < 11) {
                            chunkPrimer->setBlockId(pos, STILL_LAVA_ID);
                        } else {
                            chunkPrimer->setBlockId(pos, AIR_ID);
                            if (isTopBlock && chunkPrimer->getBlockId(pos.down()) == DIRT_ID) {
                                chunkPrimer->setBlockId(
                                        pos.down(),
                                        topBlock(pos.x + currentChunkX16.x,
                                                 pos.z + currentChunkX16.z));
                            }
                        }
                    }
                }
            }
        }
        if (isMainTunnel) { return; }
    } // end of "theCurrentSegment < theMaxSegment" for loop
}


void CaveGenerator::addRoom(ChunkPrimer* chunkPrimer, i64 seedModifier, Pos2D currentChunk,
                            const DoublePos3D& roomStart, RNG& rng, bool accurate) {
    addTunnel(chunkPrimer, seedModifier, currentChunk, roomStart, 1.0F + rng.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1,
              0.5, accurate);
}
