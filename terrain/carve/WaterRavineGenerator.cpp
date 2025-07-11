#include "WaterRavineGenerator.hpp"
#include "WaterCaveGenerator.hpp"

#include "common/MathHelper.hpp"
#include "common/constants.hpp"


MU Pos2DVec_t WaterRavineGenerator::getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper) {

    Pos2DVec_t chunkPositions;
    c_int sizeChecked = std::abs(upper.x - lower.x) * std::abs(upper.z - lower.z);
    c_int reserveSize = (int)((float)(sizeChecked) * RESERVE_MULTIPLIER / 20.0F + 4);
    chunkPositions.reserve(reserveSize);

    lower = lower - CHUNK_RANGE;
    upper = upper + CHUNK_RANGE;

    RNG rng;
    Pos2D seedMultiplier = getSeedMultiplier(g);

    Pos2D chunkPos;
    for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
        for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {

            setupRNG(g, rng, seedMultiplier, chunkPos);

            if EXPECT_FALSE (rng.nextInt(20) == 0) {
                chunkPositions.emplace_back(chunkPos);
            }
        }
    }

    return chunkPositions;
}



void WaterRavineGenerator::addFeature(
        World& worldIn, Pos2D baseChunk, bool accurate) {
    if EXPECT_FALSE (rng.nextInt(20) == 0) {
        auto tunnelStartX = (double) (baseChunk.x * 16 + rng.nextInt(16));
        auto tunnelStartY = (double) (rng.nextInt(rng.nextInt(40) + 8) + 20);
        if (rng.nextInt() < 0) { tunnelStartY += 15.0; }
        auto tunnelStartZ = (double) (baseChunk.z * 16 + rng.nextInt(16));
        c_float tunnelDirection = rng.nextFloat() * (PI_FLOAT * 2.0F);
        c_float tunnelSlope = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
        float tunnelLengthMultiplier = (rng.nextFloat() * 3.0F) + (rng.nextFloat() * 3.0F);
        if (rng.nextFloat() < 0.05) tunnelLengthMultiplier *= 2;
        addTunnel(worldIn, rng.nextLongI(), baseChunk, {tunnelStartX, tunnelStartY, tunnelStartZ},
                  tunnelLengthMultiplier, tunnelDirection, tunnelSlope, 0, 0, 3.0);
    }
}


void WaterRavineGenerator::addTunnel(World& worldIn, c_i64 randomSeed, const Pos2D baseChunk, DoublePos3D tunnel,
                                     c_float angle, float slope, float curvature, int tunnelStartSegment,
                                     int tunnelEndSegment, c_double widthMultiplier) {

    if (!isOceanic(world.getBiomeIdAt((int)tunnel.getX(), (int)tunnel.getZ()))) return;


    RNG rng;
    rng.setSeed(randomSeed);

    Pos2D baseChunkX16 = baseChunk * 16;
    float curvatureChangeRate = 0.0F;
    float slopeChangeRate = 0.0F;

    if (tunnelEndSegment < 1) {
        constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
        constexpr int RANDOM_RANGE = RANGE_BOUNDARY / 4;
        tunnelEndSegment = RANGE_BOUNDARY - rng.nextInt(RANDOM_RANGE);
    }

    bool isSegmentAtCenter = false;

    if (tunnelStartSegment == -1) {
        tunnelStartSegment = tunnelEndSegment / 2;
        isSegmentAtCenter = true;
    }

    float tunnelRadiusMultiplier = 1.0F;

    for (int segment = 0; segment < 128; ++segment) {
        if (segment == 0 || rng.nextInt(3) == 0) {
            tunnelRadiusMultiplier = 1.0F + rng.nextFloat() * rng.nextFloat(); }
        rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
    }

    c_float loopCache_endSegmentFDivPI = PI_FLOAT / (float)(tunnelEndSegment);

    for (; tunnelStartSegment < tunnelEndSegment; tunnelStartSegment++) {
        double adjustedWidth = 1.5 + (double) (sinf((float) tunnelStartSegment * loopCache_endSegmentFDivPI) * angle);
        double adjustedHeight = adjustedWidth * widthMultiplier;
        adjustedWidth = adjustedWidth * ((double) rng.nextFloat() * 0.25 + 0.75);
        adjustedHeight = adjustedHeight * ((double) rng.nextFloat() * 0.25 + 0.75);

        {
            c_float cosCurvature = cosf(curvature);
            tunnel.x += (double) (cosf(slope) * cosCurvature);
            tunnel.y += (double) (sinf(curvature));
            tunnel.z += (double) (sinf(slope) * cosCurvature);
        }

        curvature = curvature * 0.7F;
        curvature = curvature + slopeChangeRate * 0.05F;

        c_float f1_1 = rng.nextFloat();
        c_float f1_2 = rng.nextFloat();
        c_float f1_3 = rng.nextFloat();
        slopeChangeRate = slopeChangeRate * 0.8F;
        slopeChangeRate = slopeChangeRate + (f1_1 - f1_2) * f1_3 * 2.0F;

        c_float f2_1 = rng.nextFloat();
        c_float f2_2 = rng.nextFloat();
        c_float f2_3 = rng.nextFloat();
        slope += curvatureChangeRate * 0.05F;
        curvatureChangeRate = curvatureChangeRate * 0.5F;
        curvatureChangeRate = curvatureChangeRate + (f2_1 - f2_2) * f2_3 * 4.0F;

        // Hi Utter comment
        // good up to here
        if (!isSegmentAtCenter && rng.nextInt(4) == 0) { continue; }

        Pos3D min;
        Pos3D max;

        min.x = (int) floor(tunnel.x - adjustedWidth) - baseChunkX16.x - 1;
        max.x = (int) floor(tunnel.x + adjustedWidth) - baseChunkX16.x + 1;
        min.y = (int) floor(tunnel.y - adjustedHeight) - 1;
        max.y = (int) floor(tunnel.y + adjustedHeight) + 1;
        min.z = (int) floor(tunnel.z - adjustedWidth) - baseChunkX16.z - 1;
        max.z = (int) floor(tunnel.z + adjustedWidth) - baseChunkX16.z + 1;

        max.y = MathHelper::clamp(max.y, 1, 120);
        min.y = MathHelper::clamp(min.y, 1, max.y);

        min += baseChunkX16;
        max += baseChunkX16;
        if (!genBounds.isVecInside(min) || !genBounds.isVecInside(max)) {
            continue;
        }
        min -= baseChunkX16;
        max -= baseChunkX16;

        DoublePos3D scale;
        Pos3D pos;

        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            scale.x = ((double) (pos.x + baseChunkX16.x) + 0.5 - tunnel.x) / adjustedWidth;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                scale.z = ((double) (pos.z + baseChunkX16.z) + 0.5 - tunnel.z) / adjustedWidth;

                c_double scaleDXDZSq = scale.distanceSqXZ();

                if (scaleDXDZSq >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    scale.y = ((double) pos.y + 0.5 - tunnel.y) / adjustedHeight;

                    if (scaleDXDZSq * (double) rs[pos.y - 1] + (scale.y * scale.y) / 6.0 >= 1.0 || pos.y >= 62) { continue; }

                    Pos3D blockPos = pos;
                    blockPos += baseChunkX16;

                    if (!WaterCaveGenerator::canReplaceBlock(worldIn.getBlockId(blockPos))) { continue; }

                    if (blockPos.y < 10) {
                        if (rng.nextFloat() >= 0.25) {
                            worldIn.setBlockId(blockPos, lce::blocks::OBSIDIAN_ID);
                        } else {
                            worldIn.setBlockId(blockPos, lce::blocks::MAGMA_BLOCK_ID);
                        }
                    } else {
                        bool placedWater = false;


                        if (blockPos.x < 15) {
                            if (worldIn.isAirBlock(blockPos.east())) {
                                placedWater = true;
                                worldIn.setBlockId(blockPos.east(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.x > 0) {
                            if (worldIn.isAirBlock(blockPos.west())) {
                                placedWater = true;
                                worldIn.setBlockId(blockPos.west(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.z < 15) {
                            if (worldIn.isAirBlock(blockPos.south())) {
                                placedWater = true;
                                worldIn.setBlockId(blockPos.south(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.z > 0) {
                            if (worldIn.isAirBlock(blockPos.north())) {
                                placedWater = true;
                                worldIn.setBlockId(blockPos.north(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }

                        if (!placedWater) {
                            worldIn.setBlockId(blockPos, lce::blocks::STILL_WATER_ID);
                        }
                    }
                }
            }
        }

        if (isSegmentAtCenter) { break; }

    }
}























void WaterRavineGenerator::addFeature(
    ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) {
    if EXPECT_FALSE (rng.nextInt(20) == 0) {
        auto tunnelStartX = (double) (baseChunk.x * 16 + rng.nextInt(16));
        auto tunnelStartY = (double) (rng.nextInt(rng.nextInt(40) + 8) + 20);
        if (rng.nextInt() < 0) { tunnelStartY += 15.0; }
        auto tunnelStartZ = (double) (baseChunk.z * 16 + rng.nextInt(16));
        c_float tunnelDirection = rng.nextFloat() * (PI_FLOAT * 2.0F);
        c_float tunnelSlope = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
        float tunnelLengthMultiplier = (rng.nextFloat() * 3.0F) + (rng.nextFloat() * 3.0F);
        if (rng.nextFloat() < 0.05) tunnelLengthMultiplier *= 2;
        addTunnel(chunkPrimer, rng.nextLongI(), currentChunk, {tunnelStartX, tunnelStartY, tunnelStartZ},
                  tunnelLengthMultiplier, tunnelDirection, tunnelSlope, 0, 0, 3.0);

        /*Pos3D blockPos = DoublePos3D(tunnelStartX, tunnelStartY, tunnelStartZ).asType<int>();

        blockPos.x - baseChunk.x * 16;
        blockPos.z - baseChunk.z * 16;
        chunkPrimer->setBlockId(blockPos, lce::blocks::REDSTONE_BLOCK_ID);*/
    }
}


void WaterRavineGenerator::addTunnel(ChunkPrimer* chunkPrimer, c_i64 randomSeed, const Pos2D currentChunk, DoublePos3D tunnel,
                                     c_float angle, float slope, float curvature, int tunnelStartSegment,
                                     int tunnelEndSegment, c_double widthMultiplier) {

    if (!isOceanic(world.getBiomeIdAt((int)tunnel.getX(), (int)tunnel.getZ()))) return;

    RNG rng;
    rng.setSeed(randomSeed);

    Pos2D currentChunkX16 = currentChunk * 16;
    DoublePos2D offset = (currentChunkX16 + 8).asType<double>();
    float curvatureChangeRate = 0.0F;
    float slopeChangeRate = 0.0F;

    if (tunnelEndSegment < 1) {
        constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
        constexpr int RANDOM_RANGE = RANGE_BOUNDARY / 4;
        tunnelEndSegment = RANGE_BOUNDARY - rng.nextInt(RANDOM_RANGE);
    }

    bool isSegmentAtCenter = false;

    if (tunnelStartSegment == -1) {
        tunnelStartSegment = tunnelEndSegment / 2;
        isSegmentAtCenter = true;
    }

    float tunnelRadiusMultiplier = 1.0F;

    for (int segment = 0; segment < 128; ++segment) {
        if (segment == 0 || rng.nextInt(3) == 0) {
            tunnelRadiusMultiplier = 1.0F + rng.nextFloat() * rng.nextFloat(); }
        rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
    }

    c_float loopCache_endSegmentFDivPI = PI_FLOAT / (float)(tunnelEndSegment);
    c_double loopCache_maxDistanceSq = (double) (angle + 18.0F);

    for (; tunnelStartSegment < tunnelEndSegment; tunnelStartSegment++) {
        double adjustedWidth = 1.5 + (double) (sinf((float) tunnelStartSegment * loopCache_endSegmentFDivPI) * angle);
        double adjustedHeight = adjustedWidth * widthMultiplier;
        adjustedWidth = adjustedWidth * ((double) rng.nextFloat() * 0.25 + 0.75);
        adjustedHeight = adjustedHeight * ((double) rng.nextFloat() * 0.25 + 0.75);

        {
        c_float cosCurvature = cosf(curvature);
        tunnel.x += (double) (cosf(slope) * cosCurvature);
        tunnel.y += (double) (sinf(curvature));
        tunnel.z += (double) (sinf(slope) * cosCurvature);
        }

        curvature = curvature * 0.7F;
        curvature = curvature + slopeChangeRate * 0.05F;

        c_float f1_1 = rng.nextFloat();
        c_float f1_2 = rng.nextFloat();
        c_float f1_3 = rng.nextFloat();
        slopeChangeRate = slopeChangeRate * 0.8F;
        slopeChangeRate = slopeChangeRate + (f1_1 - f1_2) * f1_3 * 2.0F;

        c_float f2_1 = rng.nextFloat();
        c_float f2_2 = rng.nextFloat();
        c_float f2_3 = rng.nextFloat();
        slope += curvatureChangeRate * 0.05F;
        curvatureChangeRate = curvatureChangeRate * 0.5F;
        curvatureChangeRate = curvatureChangeRate + (f2_1 - f2_2) * f2_3 * 4.0F;

        // Hi Utter comment
        // good up to here
        if (!isSegmentAtCenter && rng.nextInt(4) == 0) { continue; }


        DoublePos2D distance = tunnel.asPos2D() - offset;
        c_auto remainingSegments = (double) (tunnelEndSegment - tunnelStartSegment);
        if (distance.distanceSq() - remainingSegments * remainingSegments > loopCache_maxDistanceSq) { return; }
        if (tunnel.x < offset.x - 16.0 - adjustedWidth * 2.0 ||
            tunnel.z < offset.z - 16.0 - adjustedWidth * 2.0 ||
            tunnel.x > offset.z + 16.0 + adjustedWidth * 2.0 ||
            tunnel.z > offset.z + 16.0 + adjustedWidth * 2.0) { continue; }

        Pos3D min;
        Pos3D max;

        min.x = (int) floor(tunnel.x - adjustedWidth) - currentChunk.x * 16 - 1;
        max.x = (int) floor(tunnel.x + adjustedWidth) - currentChunk.x * 16 + 1;
        min.y = (int) floor(tunnel.y - adjustedHeight) - 1;
        max.y = (int) floor(tunnel.y + adjustedHeight) + 1;
        min.z = (int) floor(tunnel.z - adjustedWidth) - currentChunk.z * 16 - 1;
        max.z = (int) floor(tunnel.z + adjustedWidth) - currentChunk.z * 16 + 1;

        if (min.x < 0) min.x = 0;
        if (max.x > 16) max.x = 16;
        max.y = MathHelper::clamp(max.y, 1, 120);
        min.y = MathHelper::clamp(min.y, 1, max.y);
        if (min.z < 0) min.z = 0;
        if (max.z > 16) max.z = 16;

        // added by jerrin
        max.y = std::min(max.y, 62);

        DoublePos3D scale;

        Pos3D pos;
        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            scale.x = ((double) (pos.x + currentChunkX16.x) + 0.5 - tunnel.x) / adjustedWidth;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                scale.z = ((double) (pos.z + currentChunkX16.z) + 0.5 - tunnel.z) / adjustedWidth;

                c_double scaleDXDZSq = scale.distanceSqXZ();

                if (scaleDXDZSq >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    scale.y = ((double) pos.y + 0.5 - tunnel.y) / adjustedHeight;

                    if (scaleDXDZSq * (double) rs[pos.y - 1] + (scale.y * scale.y) / 6.0 >= 1.0) { continue; }

                    if (!WaterCaveGenerator::canReplaceBlock(chunkPrimer->getBlockId(pos.x, pos.y, pos.z))) { continue; }

                    if (pos.y < 10) {
                        if (rng.nextFloat() >= 0.25) {
                            chunkPrimer->setBlockId(pos, lce::blocks::OBSIDIAN_ID);
                        } else {
                            chunkPrimer->setBlockId(pos, lce::blocks::MAGMA_BLOCK_ID);
                        }
                    } else {
                        bool placedWater = false;
                        if (pos.x < 15) {
                            if (chunkPrimer->isAirBlock(pos.east())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(pos.east(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }

                        if (pos.x > 0) {
                            if (chunkPrimer->isAirBlock(pos.west())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(pos.west(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }

                        if (pos.z < 15) {
                            if (chunkPrimer->isAirBlock(pos.south())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(pos.south(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }

                        if (pos.z > 0) {
                            if (chunkPrimer->isAirBlock(pos.north())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(pos.north(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }

                        if (!placedWater) {
                            chunkPrimer->setBlockId(pos.x, pos.y, pos.z, lce::blocks::STILL_WATER_ID);
                        }
                    }
                }
            }
        }

        if (isSegmentAtCenter) { break; }

    }
}
