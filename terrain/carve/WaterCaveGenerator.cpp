#include "WaterCaveGenerator.hpp"

#include "common/MathHelper.hpp"
#include "common/constants.hpp"



MU Pos2DVec_t WaterCaveGenerator::getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper) {

    Pos2DVec_t chunkPositions;
    c_int sizeChecked = std::abs(upper.x - lower.x) * std::abs(upper.z - lower.z);
    c_int reserveSize = (int) ((float) (sizeChecked) *RESERVE_MULTIPLIER / 15.0F + 4);
    chunkPositions.reserve(reserveSize);

    lower = lower - CHUNK_RANGE;
    upper = upper + CHUNK_RANGE;

    RNG rng;
    Pos2D seedMultiplier = getSeedMultiplier(g);

    Pos2D chunkPos;
    for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
        for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {
            setupRNG(g, rng, seedMultiplier, chunkPos);

            int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

            if EXPECT_TRUE (rng.nextInt(15) == 0 && tunnelCount != 0) {
                chunkPositions.emplace_back(chunkPos.asType<int>());
            }
        }
    }

    return chunkPositions;
}


void WaterCaveGenerator::addTunnel(World& worldIn, c_i64 seedModifier, Pos2D baseChunk, DoublePos3D start,
                                   c_float theTunnelWidth, float theTunnelDirection, float theTunnelSlope,
                                   int theCurrentSegment, int theMaxSegment, c_double theHeightMultiplier) {
    Pos2D baseChunkX16 = baseChunk * 16;

    float directionModifier = 0.0F;
    float slopeModifier = 0.0F;
    RNG rng;
    rng.setSeed(seedModifier);

    if (theMaxSegment <= 0) {
        constexpr int rangeBoundary = CHUNK_RANGE * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        theMaxSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isMainTunnel = false;

    if (theCurrentSegment == -1) {
        theCurrentSegment = theMaxSegment / 2;
        isMainTunnel = true;
    }

    c_int splitPoint = rng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
    const Pos2D startPos((int) start.x, (int) start.z);
    // DoublePos2D targetCenter = (baseChunk * 16 + 8).asType<double>();
    float maxSegmentFDivPI = PI_FLOAT / (float)(theMaxSegment);
    // c_double maxDistanceSq = (theTunnelWidth + 18.0F) * (theTunnelWidth + 18.0F);
    c_bool isTunnelWide = rng.nextInt(6) == 0;

    for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {
        c_double tunnelWidthScaled =
                1.5 + (double) (MathHelper::sin((float) theCurrentSegment * maxSegmentFDivPI) * theTunnelWidth);
        c_double tunnelHeight = tunnelWidthScaled * theHeightMultiplier;

        {
            c_float directionCosine = MathHelper::cos(theTunnelSlope);
            start.x += (double) (MathHelper::cos(theTunnelDirection) * directionCosine);
            start.y += (double) (MathHelper::sin(theTunnelSlope));
            start.z += (double) (MathHelper::sin(theTunnelDirection) * directionCosine);
        }

        if (isTunnelWide) {
            theTunnelSlope = theTunnelSlope * 0.92F;
        } else {
            theTunnelSlope = theTunnelSlope * 0.7F;
        }

        theTunnelSlope = theTunnelSlope + slopeModifier * 0.1F;
        theTunnelDirection += directionModifier * 0.1F;
        slopeModifier = slopeModifier * 0.9F;
        directionModifier = directionModifier * 0.75F;

        c_float f1_1 = rng.nextFloat();
        c_float f1_2 = rng.nextFloat();
        c_float f1_3 = rng.nextFloat();
        c_float f1 = rng.nextFloat();
        c_float f2 = rng.nextFloat();
        c_float f3 = rng.nextFloat();
        slopeModifier = slopeModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
        directionModifier = directionModifier + (f1 - f2) * f3 * 4.0F;

        if (!isMainTunnel && theCurrentSegment == splitPoint && theTunnelWidth > 1.0F && theMaxSegment > 0) {
            float tunnelWidth1, tunnelWidth2;
            i64 seed1, seed2;

            if (lce::isXbox(g->getConsole())) {
                tunnelWidth1 = rng.nextFloat();
                seed1 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
                seed2 = rng.nextLongI();
            } else {
                seed1 = rng.nextLongI();
                tunnelWidth1 = rng.nextFloat();
                seed2 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
            }
            addTunnel(worldIn, seed1, baseChunk, start, tunnelWidth1 * 0.5F + 0.5F, theTunnelDirection - HALF_PI,
                      theTunnelSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0);

            addTunnel(worldIn, seed2, baseChunk, start, tunnelWidth2 * 0.5F + 0.5F, theTunnelDirection + HALF_PI,
                      theTunnelSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0);
            return;
        }

        if (!isOceanic(world.getBiomeIdAt(startPos.x, startPos.z))) return;

        if (!isMainTunnel && rng.nextInt(4) == 0) { continue; }
        // DoublePos2D distance = start.asPos2D() - targetCenter;

        // c_double segmentsRemaining = theMaxSegment - theCurrentSegment;

        Pos3D min;
        Pos3D max;

        min.x = static_cast<int>(floor(start.x - tunnelWidthScaled)) - baseChunk.x * 16 - 1;
        max.x = static_cast<int>(floor(start.x + tunnelWidthScaled)) - baseChunk.x * 16 + 1;
        min.y = static_cast<int>(floor(start.y - tunnelHeight)) - 1;
        max.y = static_cast<int>(floor(start.y + tunnelHeight)) + 1;
        min.z = static_cast<int>(floor(start.z - tunnelWidthScaled)) - baseChunk.z * 16 - 1;
        max.z = static_cast<int>(floor(start.z + tunnelWidthScaled)) - baseChunk.z * 16 + 1;

        max.y = MathHelper::clamp(max.y, 1, 120);
        min.y = MathHelper::clamp(min.y, 1, max.y);
        if (max.y > 120) max.y = 120;

        Pos3D pos;
        DoublePos3D scale;

        min += baseChunkX16;
        max += baseChunkX16;
        if (!genBounds.isVecInside(min) || !genBounds.isVecInside(max)) {
            continue;
        }
        min -= baseChunkX16;
        max -= baseChunkX16;

        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            scale.x = ((double) (pos.x + baseChunk.x * 16) + 0.5 - start.x) / tunnelWidthScaled;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                scale.z = ((double) (pos.z + baseChunk.z * 16) + 0.5 - start.z) / tunnelWidthScaled;

                double scaleXZSq = scale.distanceSqXZ();

                if (scaleXZSq >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    scale.y = ((double) pos.y + 0.5 - start.y) / tunnelHeight;

                    if (scale.y <= -0.7 || scaleXZSq + scale.y * scale.y >= 1.0 || pos.y >= 62) { continue; }

                    Pos3D blockPos;
                    blockPos = pos + baseChunk * 16;
                    
                    if (!canReplaceBlock(worldIn.getBlockId(blockPos))) { continue; }

                    if (blockPos.y < 9) {
                        worldIn.setBlockId(blockPos, lce::blocks::STILL_LAVA_ID);
                    } else if (blockPos.y < 10) {
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
                                worldIn.setBlockId(blockPos.east(),
                                                        lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.x > 0) {
                            if (worldIn.isAirBlock(blockPos.west())) {
                                placedWater = true;
                                worldIn.setBlockId(blockPos.west(),
                                                        lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.z < 15) {
                            if (worldIn.isAirBlock(blockPos.south())) {
                                placedWater = true;
                                worldIn.setBlockId(blockPos.south(),
                                                        lce::blocks::FLOWING_WATER_ID);
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
                    // }
                }
            }
        }

        if (isMainTunnel) { break; }
    }
}

void WaterCaveGenerator::addRoom(World& worldIn, i64 seedModifier, Pos2D target, const DoublePos3D& roomStart,
                                 RNG& rng) {
    addTunnel(worldIn, seedModifier, target, roomStart, 1.0F + rng.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
}


void WaterCaveGenerator::addFeature(World& worldIn, Pos2D baseChunk, bool accurate) {
    c_int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

    if EXPECT_TRUE (rng.nextInt(15) != 0) { return; }

    for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
        auto tunnelStartZ = (double) (baseChunk.z * 16 + rng.nextInt(16));
        auto tunnelStartY = (double) rng.nextInt(rng.nextInt(120) + 8);
        auto tunnelStartX = (double) (baseChunk.x * 16 + rng.nextInt(16));
        int segmentCount = 1;

        if (rng.nextInt(4) == 0) {
            addRoom(worldIn, rng.nextLongI(), baseChunk, {tunnelStartX, tunnelStartY, tunnelStartZ}, rng);
            segmentCount = rng.nextInt(4) + 1;
        }

        for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
            c_float yaw = rng.nextFloat() * (PI_FLOAT * 2.0F);
            c_float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
            c_float tunnelLength = rng.nextFloat() * 2.0F + rng.nextFloat();

            addTunnel(worldIn, rng.nextLongI(), baseChunk, {tunnelStartX, tunnelStartY, tunnelStartZ},
                      tunnelLength, yaw, pitch, 0, 0, 1.0);
        }
    }
}


bool WaterCaveGenerator::canReplaceBlock(u16 blockAt) {
    switch (blockAt) {
        case lce::blocks::AIR_ID:
        case lce::blocks::STONE_ID:
        case lce::blocks::GRASS_ID:
        case lce::blocks::DIRT_ID:
        case lce::blocks::FLOWING_WATER_ID:
        case lce::blocks::STILL_WATER_ID:
        case lce::blocks::FLOWING_LAVA_ID:
        case lce::blocks::STILL_LAVA_ID:
        case lce::blocks::SAND_ID:
        case lce::blocks::GRAVEL_ID:
        case lce::blocks::SANDSTONE_ID:
        case lce::blocks::OBSIDIAN_ID:
        case lce::blocks::MYCELIUM_ID:
        case lce::blocks::STAINED_HARDENED_CLAY_ID:
        case lce::blocks::HARDENED_CLAY_ID:
        case lce::blocks::RED_SANDSTONE_ID:
            return true;
        default:
            return false;
    }
}


void WaterCaveGenerator::addTunnel(ChunkPrimer* chunkPrimer, c_i64 seedModifier, Pos2D chunk, DoublePos3D start,
                                   c_float tunnelWidth, float tunnelDirection, float tunnelSlope,
                                   int currentTunnelSegment, int maxTunnelSegment, c_double tunnelHeightMultiplier) {


    float directionModifier = 0.0F;
    float slopeModifier = 0.0F;
    RNG rng;
    rng.setSeed(seedModifier);

    if (maxTunnelSegment <= 0) {
        constexpr int rangeBoundary = CHUNK_RANGE * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        maxTunnelSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isMainTunnel = false;

    if (currentTunnelSegment == -1) {
        currentTunnelSegment = maxTunnelSegment / 2;
        isMainTunnel = true;
    }

    c_int splitPoint = rng.nextInt(maxTunnelSegment / 2) + maxTunnelSegment / 4;
    const Pos2D startPos((int) start.x, (int) start.z);
    DoublePos2D targetCenter = (chunk * 16 + 8).asType<double>();
    float maxSegmentFDivPI = PI_FLOAT / (float)(maxTunnelSegment);
    c_double maxDistanceSq = (tunnelWidth + 18.0F) * (tunnelWidth + 18.0F);
    c_bool isTunnelWide = rng.nextInt(6) == 0;

    for (; currentTunnelSegment < maxTunnelSegment; ++currentTunnelSegment) {
        c_double tunnelWidthScaled =
                1.5 + (double) (MathHelper::sin((float) currentTunnelSegment * maxSegmentFDivPI) * tunnelWidth);
        c_double tunnelHeight = tunnelWidthScaled * tunnelHeightMultiplier;

        {
            c_float directionCosine = MathHelper::cos(tunnelSlope);
            start.x += (double) (MathHelper::cos(tunnelDirection) * directionCosine);
            start.y += (double) (MathHelper::sin(tunnelSlope));
            start.z += (double) (MathHelper::sin(tunnelDirection) * directionCosine);
        }

        if (isTunnelWide) {
            tunnelSlope = tunnelSlope * 0.92F;
        } else {
            tunnelSlope = tunnelSlope * 0.7F;
        }

        tunnelSlope = tunnelSlope + slopeModifier * 0.1F;
        tunnelDirection += directionModifier * 0.1F;
        slopeModifier = slopeModifier * 0.9F;
        directionModifier = directionModifier * 0.75F;

        c_float f1_1 = rng.nextFloat();
        c_float f1_2 = rng.nextFloat();
        c_float f1_3 = rng.nextFloat();
        c_float f1 = rng.nextFloat();
        c_float f2 = rng.nextFloat();
        c_float f3 = rng.nextFloat();
        slopeModifier = slopeModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
        directionModifier = directionModifier + (f1 - f2) * f3 * 4.0F;

        if (!isMainTunnel && currentTunnelSegment == splitPoint && tunnelWidth > 1.0F && maxTunnelSegment > 0) {
            float tunnelWidth1, tunnelWidth2;
            i64 seed1, seed2;

            if (lce::isXbox(g->getConsole())) {
                tunnelWidth1 = rng.nextFloat();
                seed1 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
                seed2 = rng.nextLongI();
            } else {
                seed1 = rng.nextLongI();
                tunnelWidth1 = rng.nextFloat();
                seed2 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
            }
            addTunnel(chunkPrimer, seed1, chunk, start, tunnelWidth1 * 0.5F + 0.5F, tunnelDirection - HALF_PI,
                      tunnelSlope / 3.0F, currentTunnelSegment, maxTunnelSegment, 1.0);

            addTunnel(chunkPrimer, seed2, chunk, start, tunnelWidth2 * 0.5F + 0.5F, tunnelDirection + HALF_PI,
                      tunnelSlope / 3.0F, currentTunnelSegment, maxTunnelSegment, 1.0);
            return;
        }

        if (!isOceanic(world.getBiomeIdAt(startPos.x, startPos.z))) return;

        if (!isMainTunnel && rng.nextInt(4) == 0) { continue; }
        DoublePos2D distance = start.asPos2D() - targetCenter;

        c_double segmentsRemaining = maxTunnelSegment - currentTunnelSegment;

        if (distance.distanceSq() - segmentsRemaining * segmentsRemaining > maxDistanceSq) { return; }

        if (start.x < targetCenter.x - 16.0 - tunnelWidth * 2.0 ||
            start.z < targetCenter.z - 16.0 - tunnelWidth * 2.0 ||
            start.x > targetCenter.x + 16.0 + tunnelWidth * 2.0 ||
            start.z > targetCenter.z + 16.0 + tunnelWidth * 2.0) { continue; }

        Pos3D min;
        Pos3D max;

        min.x = static_cast<int>(floor(start.x - tunnelWidthScaled)) - chunk.x * 16 - 1;
        max.x = static_cast<int>(floor(start.x + tunnelWidthScaled)) - chunk.x * 16 + 1;
        min.y = static_cast<int>(floor(start.y - tunnelHeight)) - 1;
        max.y = static_cast<int>(floor(start.y + tunnelHeight)) + 1;
        min.z = static_cast<int>(floor(start.z - tunnelWidthScaled)) - chunk.z * 16 - 1;
        max.z = static_cast<int>(floor(start.z + tunnelWidthScaled)) - chunk.z * 16 + 1;

        if (min.x < 0) min.x = 0;
        if (max.x > 16) max.x = 16;
        if (min.z < 0) min.z = 0;
        if (max.z > 16) max.z = 16;

        max.y = MathHelper::clamp(max.y, 1, 120);
        min.y = MathHelper::clamp(min.y, 1, max.y);
        if (max.y > 120) max.y = 120;

        Pos3D pos;
        DoublePos3D scale;


        for (pos.x = min.x; pos.x < max.x; ++pos.x) {
            scale.x = ((double) (pos.x + chunk.x * 16) + 0.5 - start.x) / tunnelWidthScaled;

            for (pos.z = min.z; pos.z < max.z; ++pos.z) {
                scale.z = ((double) (pos.z + chunk.z * 16) + 0.5 - start.z) / tunnelWidthScaled;

                double scaleXZSq = scale.distanceSqXZ();

                if (scaleXZSq >= 1.0) { continue; }

                for (pos.y = max.y - 1; pos.y >= min.y; --pos.y) {
                    scale.y = ((double) pos.y + 0.5 - start.y) / tunnelHeight;

                    if (scale.y <= -0.7 || scaleXZSq + scale.y * scale.y >= 1.0 || pos.y >= 62) { continue; }

                    Pos3D blockPos;
                    blockPos = pos;
                    
                    if (!canReplaceBlock(chunkPrimer->getBlockId(blockPos))) { continue; }

                    if (blockPos.y < 9) {
                        chunkPrimer->setBlockId(blockPos, lce::blocks::STILL_LAVA_ID);
                    } else if (blockPos.y < 10) {
                        if (rng.nextFloat() >= 0.25) {
                            chunkPrimer->setBlockId(blockPos, lce::blocks::OBSIDIAN_ID);
                        } else {
                            chunkPrimer->setBlockId(blockPos, lce::blocks::MAGMA_BLOCK_ID);
                        }
                    } else {
                        bool placedWater = false;
                        
                        if (blockPos.x < 15) {
                            if (chunkPrimer->isAirBlock(blockPos.east())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(blockPos.east(),
                                                        lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.x > 0) {
                            if (chunkPrimer->isAirBlock(blockPos.west())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(blockPos.west(),
                                                        lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.z < 15) {
                            if (chunkPrimer->isAirBlock(blockPos.south())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(blockPos.south(),
                                                        lce::blocks::FLOWING_WATER_ID);
                            }
                        }
                        if (blockPos.z > 0) {
                            if (chunkPrimer->isAirBlock(blockPos.north())) {
                                placedWater = true;
                                chunkPrimer->setBlockId(blockPos.north(), lce::blocks::FLOWING_WATER_ID);
                            }
                        }

                        if (!placedWater) {
                            chunkPrimer->setBlockId(blockPos.x, blockPos.y, blockPos.z,
                                                    lce::blocks::STILL_WATER_ID);
                        }
                    }
                    // }
                }
            }
        }

        if (isMainTunnel) { break; }
    }
}

void WaterCaveGenerator::addRoom(i64 seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, const DoublePos3D& roomStart,
                                 RNG& rng) {
    addTunnel(chunkPrimer, seedModifier, target, roomStart, 1.0F + rng.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
}


void WaterCaveGenerator::addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) {
    c_int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

    if EXPECT_TRUE (rng.nextInt(15) != 0) { return; }

    for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
        auto tunnelStartZ = (double) (baseChunk.z * 16 + rng.nextInt(16));
        auto tunnelStartY = (double) rng.nextInt(rng.nextInt(120) + 8);
        auto tunnelStartX = (double) (baseChunk.x * 16 + rng.nextInt(16));
        int segmentCount = 1;

        if (rng.nextInt(4) == 0) {
            addRoom(rng.nextLongI(), currentChunk, chunkPrimer, {tunnelStartX, tunnelStartY, tunnelStartZ}, rng);
            segmentCount = rng.nextInt(4) + 1;
        }

        for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
            c_float yaw = rng.nextFloat() * (PI_FLOAT * 2.0F);
            c_float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
            c_float tunnelLength = rng.nextFloat() * 2.0F + rng.nextFloat();

            addTunnel(chunkPrimer, rng.nextLongI(), currentChunk, {tunnelStartX, tunnelStartY, tunnelStartZ},
                      tunnelLength, yaw, pitch, 0, 0, 1.0);
        }
    }
}