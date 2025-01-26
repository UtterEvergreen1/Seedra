#include "RavineGenerator.hpp"

#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biomeID.hpp"


using namespace lce::blocks;


MU Pos2DVec_t RavineGenerator::getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper) {

    RNG rng;
    Pos2DTemplate<i64> seedMultiplier = getSeedMultiplier(g);

    lower = lower - CHUNK_RANGE;
    upper = upper + CHUNK_RANGE;

    Pos2DVec_t chunkPositions;
    c_int sizeChecked = std::abs(upper.x - lower.x) * std::abs(upper.z - lower.z);
    c_int reserveSize = (int)((float)(sizeChecked) * RESERVE_MULTIPLIER / 50.0F + 4);
    chunkPositions.reserve(reserveSize);

    Pos2D chunkPos;
    for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
        for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {
            RavineGenerator::setupRNG(g, rng, seedMultiplier, chunkPos);

            if EXPECT_FALSE (rng.nextInt(50) == 0) {
                chunkPositions.emplace_back(chunkPos.asType<int>());
            }
        }
    }

    return chunkPositions;
}






void RavineGenerator::addFeature(World& worldIn, Pos2D baseChunk, bool accurate) {
    if EXPECT_FALSE (rng.nextInt(50) == 0) {
        DoublePos3D tunnelStart;
        tunnelStart.x = (double) (baseChunk.x * 16 + rng.nextInt(16));
        tunnelStart.y = (double) (rng.nextInt(rng.nextInt(40) + 8) + 20);
        tunnelStart.z = (double) (baseChunk.z * 16 + rng.nextInt(16));

        c_float tunnelDirection = rng.nextFloat() * (PI_FLOAT * 2.0F);
        c_float tunnelSlope = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
        c_float tunnelLengthMultiplier = (rng.nextFloat() * 2.0F + rng.nextFloat()) * 2.0F;

        addTunnel(worldIn, rng.nextLongI(), baseChunk, tunnelStart, tunnelLengthMultiplier, tunnelDirection,
                  tunnelSlope, 0, 0, 3.0, accurate);
    }
}


void RavineGenerator::addTunnel(World& worldIn, i64 randomSeed, Pos2D baseChunk, DoublePos3D tunnel, float angle,
                                float slope, float curvature, int theStartSegment, int theEndSegment,
                                double theWidthMultiplier, bool accurate) {

    if (accurate &&
        g.getLCEVersion() == LCEVERSION::AQUATIC &&
        isOceanic(g.getBiomeAt(1, tunnel.asType<int>()))) { return; }

    RNG rng;
    rng.setSeed(randomSeed);
    Pos2D baseChunkX16 = baseChunk * 16;

    float curvatureChangeRate = 0.0F;
    float slopeChangeRate = 0.0F;

    if (theEndSegment < 1) {
        constexpr int rangeBoundary = CHUNK_RANGE * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        theEndSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isSegmentAtCenter = false;
    if (theStartSegment == -1) {
        theStartSegment = theEndSegment / 2;
        isSegmentAtCenter = true;
    }

    float tunnelRadiusMultiplier = 1.0F;

    for (int segment = 0; segment < 128; ++segment) {
        if (segment == 0 || rng.nextInt(3) == 0) {
            tunnelRadiusMultiplier = 1.0F + rng.nextFloat() * rng.nextFloat();
        }
        rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
    }

    float theEndSegmentFDivPI = PI_FLOAT / (float)(theEndSegment);

SEGMENT_FOR_LOOP_START:
    for (; theStartSegment < theEndSegment; theStartSegment++) {

        double adjustedWidth = 1.5 + (double) (MathHelper::sin(
                                                       (float) theStartSegment * theEndSegmentFDivPI) * angle);
        double adjustedHeight = adjustedWidth * theWidthMultiplier;

        adjustedWidth = adjustedWidth * ((double) rng.nextFloat() * 0.25 + 0.75);
        adjustedHeight = adjustedHeight * ((double) rng.nextFloat() * 0.25 + 0.75);

        c_float cosCurvature = MathHelper::cos(curvature);
        tunnel.x += (double) (MathHelper::cos(slope) * cosCurvature);
        tunnel.y += (double) (MathHelper::sin(curvature));
        tunnel.z += (double) (MathHelper::sin(slope) * cosCurvature);

        // setup tunnel slope
        c_float f1_1 = rng.nextFloat();
        c_float f1_2 = rng.nextFloat();
        c_float f1_3 = rng.nextFloat();
        curvature = curvature * 0.7F;
        curvature = curvature + slopeChangeRate * 0.05F;
        slopeChangeRate = slopeChangeRate * 0.8F;
        slopeChangeRate = slopeChangeRate + (f1_1 - f1_2) * f1_3 * 2.0F;

        // setup tunnel curvature
        c_float f2_1 = rng.nextFloat();
        c_float f2_2 = rng.nextFloat();
        c_float f2_3 = rng.nextFloat();
        slope += curvatureChangeRate * 0.05F;
        curvatureChangeRate = curvatureChangeRate * 0.5F;
        curvatureChangeRate = curvatureChangeRate + (f2_1 - f2_2) * f2_3 * 4.0F;

        if (!isSegmentAtCenter && rng.nextInt(4) == 0) { continue; }

        /*
        DoublePos2D offset = (currentChunkX16 + 8).asType<double>();
        DoublePos2D distance = tunnel.asPos2D() - offset;
        c_auto remainingSegments = (double) (theEndSegment - theStartSegment);
        c_auto maxDistance = (double) (angle + 18.0F);
        if (distance.distanceSq() - remainingSegments * remainingSegments > maxDistance * maxDistance) { return; }
        if (tunnel.x < offset.x - 16.0 - adjustedWidth * 2.0 ||
            tunnel.z < offset.z - 16.0 - adjustedWidth * 2.0 ||
            tunnel.x > offset.x + 16.0 + adjustedWidth * 2.0 ||
            tunnel.z > offset.z + 16.0 + adjustedWidth * 2.0) {
            continue;
        }
         */
        
        Pos3D min;
        Pos3D max;
        min.x = (int) floor(tunnel.x - adjustedWidth) - baseChunkX16.x - 1;
        max.x = (int) floor(tunnel.x + adjustedWidth) - baseChunkX16.x + 1;
        min.y = (int) floor(tunnel.y - adjustedHeight) - 1;
        max.y = (int) floor(tunnel.y + adjustedHeight) + 1;
        min.z = (int) floor(tunnel.z - adjustedWidth) - baseChunkX16.z - 1;
        max.z = (int) floor(tunnel.z + adjustedWidth) - baseChunkX16.z + 1;
        if (min.y < 1) min.y = 1;
        if (max.y > 120) max.y = 120;
        min += baseChunkX16;
        max += baseChunkX16;
        if (!worldIn.bounds.isVecInside(min) && !worldIn.bounds.isVecInside(max)) {
            continue;
        }
        Pos3D pos;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                for (pos.y = max.y + 1; pos.y >= min.y - 1; --pos.y) {

                    c_u16 blockId = worldIn.getBlockId(pos);
                    if (pos.y != min.y - 1 &&
                        pos.x != min.x &&
                        pos.x != max.x - 1
                        && pos.z != min.z
                        && pos.z != max.z - 1) {
                        pos.y = min.y;
                    }
                    c_bool hasLiquid = blockId == STILL_WATER_ID ||
                                       blockId == FLOWING_WATER_ID;
                    if (hasLiquid) {
                        goto FOUND_LIQUID;
                    }
                }
            }
        }
        min -= baseChunkX16;
        max -= baseChunkX16;

        // used to make the cpu not have to
        // cache the later code when it finds water
        goto JUMP_PAST_FOUND_LIQUID;
    FOUND_LIQUID:
        if (isSegmentAtCenter) { return; }
        goto SEGMENT_FOR_LOOP_START;
    JUMP_PAST_FOUND_LIQUID:

        pos.setPos(0, 0, 0);
        DoublePos3D dXYZ;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            dXYZ.x = ((double) (pos.x + baseChunkX16.x) + 0.5 - tunnel.x) / adjustedWidth;
            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                dXYZ.z = ((double) (pos.z + baseChunkX16.z) + 0.5 - tunnel.z) / adjustedWidth;
                bool replaceableBlockDetected = false;

                c_double sqXZ = dXYZ.distanceSqXZ();
                if (sqXZ >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    dXYZ.y = ((double) pos.y + 0.5 - tunnel.y) / adjustedHeight;

                    if ((sqXZ) * (double) rs[pos.y] + dXYZ.y * dXYZ.y / 6.0 >= 1.0) { continue; }

                    Pos3D blockPos = pos;
                    blockPos += baseChunkX16;

                    c_u16 currentBlock = worldIn.getBlockId(blockPos);
                    c_u16 blockAbove = worldIn.getBlockId(blockPos.up());

                    if (currentBlock == GRASS_ID) {
                        replaceableBlockDetected = true;
                    }

                    if (canReplaceBlock(currentBlock, blockAbove)) {
                        if (blockPos.y < 11) {
                            worldIn.setBlockId(blockPos, FLOWING_LAVA_ID);
                        } else {
                            worldIn.setBlockId(blockPos, AIR_ID);

                            if (replaceableBlockDetected &&
                                worldIn.getBlockId(blockPos.down()) == DIRT_ID) {
                                worldIn.setBlockId(blockPos.down(),
                                    topBlock(blockPos.x, blockPos.z));
                            }
                        }
                    }
                }
            }
        }
        if (isSegmentAtCenter) { break; }
    }
}













unsigned char RavineGenerator::topBlock(c_int x, c_int z) const {
    switch (g.getBiomeAt(1, x, z)) {
        case beach:
        case desert:
        case mesa:
            return lce::blocks::SAND_ID;
        case mushroom_field_shore:
        case mushroom_fields:
            return lce::blocks::MYCELIUM_ID;
        case snowy_beach:
        case snowy_mountains:
        case snowy_taiga:
        case snowy_taiga_hills:
        case snowy_tundra:
            return lce::blocks::SNOW_ID;
        case stone_beach:
            return lce::blocks::STONE_ID;
        default:
            return lce::blocks::GRASS_ID;
    }
}


bool RavineGenerator::canReplaceBlock(c_u16 blockAt, c_u16 blockAbove) {
    switch (blockAt) {
        case lce::blocks::STONE_ID:
        case lce::blocks::GRASS_ID:
        case lce::blocks::DIRT_ID:
        case lce::blocks::SANDSTONE_ID:
        case lce::blocks::SNOW_ID:
        case lce::blocks::MYCELIUM_ID:
        case lce::blocks::STAINED_HARDENED_CLAY_ID:
        case lce::blocks::HARDENED_CLAY_ID:
        case lce::blocks::RED_SANDSTONE_ID:
            return true;
        case lce::blocks::SAND_ID:
        case lce::blocks::GRAVEL_ID:
            return blockAbove != lce::blocks::AIR_ID
                   && blockAbove != lce::blocks::STILL_WATER_ID; // wii u?
            //return blockAbove != lce::items::STILL_WATER_ID;
        default:
            return false;
    }
}




















void RavineGenerator::addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) {
    if EXPECT_FALSE (rng.nextInt(50) == 0) {
        DoublePos3D tunnelStart;
        tunnelStart.x = (double) (baseChunk.x * 16 + rng.nextInt(16));
        tunnelStart.y = (double) (rng.nextInt(rng.nextInt(40) + 8) + 20);
        tunnelStart.z = (double) (baseChunk.z * 16 + rng.nextInt(16));

        c_float tunnelDirection = rng.nextFloat() * (PI_FLOAT * 2.0F);
        c_float tunnelSlope = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
        c_float tunnelLengthMultiplier = (rng.nextFloat() * 2.0F + rng.nextFloat()) * 2.0F;

        addTunnel(chunkPrimer, rng.nextLongI(), currentChunk, tunnelStart, tunnelLengthMultiplier, tunnelDirection,
                  tunnelSlope, 0, 0, 3.0, accurate);
    }
}


void RavineGenerator::addTunnel(ChunkPrimer* chunkPrimer, i64 randomSeed, Pos2D currentChunk, DoublePos3D tunnel, float angle,
                                float slope, float curvature, int theStartSegment, int theEndSegment,
                                double theWidthMultiplier, bool accurate) {

    if (accurate &&
        g.getLCEVersion() == LCEVERSION::AQUATIC &&
        isOceanic(g.getBiomeAt(1, tunnel.asType<int>()))) { return; }

    RNG rng;
    rng.setSeed(randomSeed);
    Pos2D currentChunkX16 = currentChunk * 16;
    DoublePos2D offset = (currentChunkX16 + 8).asType<double>();

    float curvatureChangeRate = 0.0F;
    float slopeChangeRate = 0.0F;

    if (theEndSegment < 1) {
        constexpr int rangeBoundary = CHUNK_RANGE * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        theEndSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isSegmentAtCenter = false;
    if (theStartSegment == -1) {
        theStartSegment = theEndSegment / 2;
        isSegmentAtCenter = true;
    }

    float tunnelRadiusMultiplier = 1.0F;

    for (int segment = 0; segment < 128; ++segment) {
        if (segment == 0 || rng.nextInt(3) == 0) {
            tunnelRadiusMultiplier = 1.0F + rng.nextFloat() * rng.nextFloat();
        }
        rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
    }

    float endSegmentFDivPI = PI_FLOAT / (float)(theEndSegment);

SEGMENT_FOR_LOOP_START:
    for (; theStartSegment < theEndSegment; theStartSegment++) {

        double adjustedWidth = 1.5 + (double) (MathHelper::sin(
            (float) theStartSegment * endSegmentFDivPI) * angle);
        double adjustedHeight = adjustedWidth * theWidthMultiplier;

        adjustedWidth = adjustedWidth * ((double) rng.nextFloat() * 0.25 + 0.75);
        adjustedHeight = adjustedHeight * ((double) rng.nextFloat() * 0.25 + 0.75);

        c_float cosCurvature = MathHelper::cos(curvature);
        tunnel.x += (double) (MathHelper::cos(slope) * cosCurvature);
        tunnel.y += (double) (MathHelper::sin(curvature));
        tunnel.z += (double) (MathHelper::sin(slope) * cosCurvature);

        // setup tunnel slope
        c_float f1_1 = rng.nextFloat();
        c_float f1_2 = rng.nextFloat();
        c_float f1_3 = rng.nextFloat();
        curvature = curvature * 0.7F;
        curvature = curvature + slopeChangeRate * 0.05F;
        slopeChangeRate = slopeChangeRate * 0.8F;
        slopeChangeRate = slopeChangeRate + (f1_1 - f1_2) * f1_3 * 2.0F;

        // setup tunnel curvature
        c_float f2_1 = rng.nextFloat();
        c_float f2_2 = rng.nextFloat();
        c_float f2_3 = rng.nextFloat();
        slope += curvatureChangeRate * 0.05F;
        curvatureChangeRate = curvatureChangeRate * 0.5F;
        curvatureChangeRate = curvatureChangeRate + (f2_1 - f2_2) * f2_3 * 4.0F;


        DoublePos2D distance = tunnel.asPos2D() - offset;
        c_auto remainingSegments = (double) (theEndSegment - theStartSegment);
        c_auto maxDistance = (double) (angle + 18.0F);

        if (!isSegmentAtCenter && rng.nextInt(4) == 0) { continue; }

        if (distance.distanceSq() - remainingSegments * remainingSegments > maxDistance * maxDistance) {
            return;
        }

        if (tunnel.x < offset.x - 16.0 - adjustedWidth * 2.0 ||
            tunnel.z < offset.z - 16.0 - adjustedWidth * 2.0 ||
            tunnel.x > offset.x + 16.0 + adjustedWidth * 2.0 ||
            tunnel.z > offset.z + 16.0 + adjustedWidth * 2.0) {
            continue;
        }
        
        Pos3D min;
        Pos3D max;
        min.x = (int) floor(tunnel.x - adjustedWidth) - currentChunkX16.x - 1;
        max.x = (int) floor(tunnel.x + adjustedWidth) - currentChunkX16.x + 1;
        min.y = (int) floor(tunnel.y - adjustedHeight) - 1;
        max.y = (int) floor(tunnel.y + adjustedHeight) + 1;
        min.z = (int) floor(tunnel.z - adjustedWidth) - currentChunkX16.z - 1;
        max.z = (int) floor(tunnel.z + adjustedWidth) - currentChunkX16.z + 1;

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
                    c_u16 blockId = chunkPrimer->getBlockId(pos);
                    if (pos.y != min.y - 1 &&
                        pos.x != min.x &&
                        pos.x != max.x - 1
                        && pos.z != min.z
                        && pos.z != max.z - 1) {
                        pos.y = min.y;
                    }

                    c_bool hasLiquid = blockId == STILL_WATER_ID ||
                                       blockId == FLOWING_WATER_ID;
                    if (hasLiquid) {
                        goto FOUND_LIQUID;
                    }
                }
            }
        }

        // used to make the cpu not have to
        // cache the later code when it finds water
        goto JUMP_PAST_FOUND_LIQUID;
    FOUND_LIQUID:
        if (isSegmentAtCenter) { return; }
        goto SEGMENT_FOR_LOOP_START;
    JUMP_PAST_FOUND_LIQUID:


        pos.setPos(0, 0, 0);
        DoublePos3D dXYZ;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            dXYZ.x = ((double) (pos.x + currentChunkX16.x) + 0.5 - tunnel.x) / adjustedWidth;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                dXYZ.z = ((double) (pos.z + currentChunkX16.z) + 0.5 - tunnel.z) / adjustedWidth;
                bool replaceableBlockDetected = false;

                c_double scaleDXDZSq = dXYZ.distanceSqXZ();

                if (scaleDXDZSq >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    dXYZ.y = ((double) pos.y + 0.5 - tunnel.y) / adjustedHeight;

                    if ((scaleDXDZSq) * (double) rs[pos.y] + dXYZ.y * dXYZ.y / 6.0 >= 1.0) { continue; }

                    c_u16 currentBlock = chunkPrimer->getBlockId(pos);
                    c_u16 blockAbove = chunkPrimer->getBlockId(pos.up());

                    if (currentBlock == GRASS_ID) {
                        replaceableBlockDetected = true;
                    }

                    if (canReplaceBlock(currentBlock, blockAbove)) {
                        if (pos.y < 11) {
                            chunkPrimer->setBlockId(pos, FLOWING_LAVA_ID);
                        } else {
                            chunkPrimer->setBlockId(pos, AIR_ID);

                            if (replaceableBlockDetected &&
                                chunkPrimer->getBlockId(pos.down()) == DIRT_ID) {
                                chunkPrimer->setBlockId(pos.down(),
                                    topBlock(pos.x + currentChunkX16.x, pos.z + currentChunkX16.z));
                            }
                        }
                    }
                }
            }
        }
        if (isSegmentAtCenter) { break; }
    }
}
