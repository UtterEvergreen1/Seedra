#include "WaterCaveGenerator.hpp"

#include "common/MathHelper.hpp"
#include "common/constants.hpp"


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

void WaterCaveGenerator::addTunnel(c_i64 seedModifier, const Pos2D chunk, ChunkPrimer* chunkPrimer,
                                   DoublePos3D start, c_float tunnelWidth, float tunnelDirection, float tunnelSlope,
                                   int currentTunnelSegment, int maxTunnelSegment,
                                   c_double tunnelHeightMultiplier) {

    const Pos2D startPos((int) start.x, (int) start.z);

    c_double targetCenterX = chunk.x * 16 + 8;
    c_double targetCenterZ = chunk.z * 16 + 8;
    float directionModifier = 0.0F;
    float slopeModifier = 0.0F;
    RNG rng;
    rng.setSeed(seedModifier);

    if (maxTunnelSegment <= 0) {
        constexpr int rangeBoundary = range * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        maxTunnelSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isMainTunnel = false;

    if (currentTunnelSegment == -1) {
        currentTunnelSegment = maxTunnelSegment / 2;
        isMainTunnel = true;
    }

    c_int splitPoint = rng.nextInt(maxTunnelSegment / 2) + maxTunnelSegment / 4;

    for (c_bool isTunnelWide = rng.nextInt(6) == 0; currentTunnelSegment < maxTunnelSegment;
         ++currentTunnelSegment) {
        c_double tunnelWidthScaled =
                1.5 + (double) (sinf((float) currentTunnelSegment * PI_FLOAT / (float) maxTunnelSegment) * tunnelWidth);
        c_double tunnelHeight = tunnelWidthScaled * tunnelHeightMultiplier;
        c_float directionCosine = cosf(tunnelSlope);
        c_float directionSine = sinf(tunnelSlope);
        start.x += (double) (cosf(tunnelDirection) * directionCosine);
        start.y += (double) directionSine;
        start.z += (double) (sinf(tunnelDirection) * directionCosine);

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
        slopeModifier = slopeModifier + (f1_1 - f1_2) * f1_3 * 2.0F;   // correct
        directionModifier = directionModifier + (f1 - f2) * f3 * 4.0F; // correct

        if (!isMainTunnel && currentTunnelSegment == splitPoint && tunnelWidth > 1.0F && maxTunnelSegment > 0) {
            c_bool isXbox = g.getConsole() == lce::CONSOLE::XBOX360 ||
                                g.getConsole() == lce::CONSOLE::XBOX1;
            float tunnelWidth1;
            i64 seed1;
            if (isXbox) {
                tunnelWidth1 = rng.nextFloat();
                seed1 = rng.nextLongI();
            } else {
                seed1 = rng.nextLongI();
                tunnelWidth1 = rng.nextFloat();
            }
            addTunnel(seed1, chunk, chunkPrimer, start, tunnelWidth1 * 0.5F + 0.5F, tunnelDirection - HALF_PI,
                      tunnelSlope / 3.0F, currentTunnelSegment, maxTunnelSegment, 1.0);
            float tunnelWidth2;
            i64 seed2;
            if (isXbox) {
                tunnelWidth2 = rng.nextFloat();
                seed2 = rng.nextLongI();
            } else {
                seed2 = rng.nextLongI();
                tunnelWidth2 = rng.nextFloat();
            }
            addTunnel(seed2, chunk, chunkPrimer, start, tunnelWidth2 * 0.5F + 0.5F, tunnelDirection + HALF_PI,
                      tunnelSlope / 3.0F, currentTunnelSegment, maxTunnelSegment, 1.0);
            return;
        }

        if (!isOceanic(g.getBiomeAt(1, startPos))) return;

        if (isMainTunnel || rng.nextInt(4) != 0) {
            c_double distanceX = start.x - targetCenterX;
            c_double distanceZ = start.z - targetCenterZ;
            c_double segmentsRemaining = maxTunnelSegment - currentTunnelSegment;
            c_double maxDistance = tunnelWidth + 18.0F;

            if (distanceX * distanceX + distanceZ * distanceZ - segmentsRemaining * segmentsRemaining >
                maxDistance * maxDistance) {
                return;
            }

            if (start.x >= targetCenterX - 16.0 - tunnelWidthScaled * 2.0 &&
                start.z >= targetCenterZ - 16.0 - tunnelWidthScaled * 2.0 &&
                start.x <= targetCenterX + 16.0 + tunnelWidthScaled * 2.0 &&
                start.z <= targetCenterZ + 16.0 + tunnelWidthScaled * 2.0) {
                int minX = static_cast<int>(floor(start.x - tunnelWidthScaled)) - chunk.x * 16 - 1;
                int maxX = static_cast<int>(floor(start.x + tunnelWidthScaled)) - chunk.x * 16 + 1;
                int minY = static_cast<int>(floor(start.y - tunnelHeight)) - 1;
                int maxY = static_cast<int>(floor(start.y + tunnelHeight)) + 1;
                int minZ = static_cast<int>(floor(start.z - tunnelWidthScaled)) - chunk.z * 16 - 1;
                int maxZ = static_cast<int>(floor(start.z + tunnelWidthScaled)) - chunk.z * 16 + 1;

                if (minX < 0) minX = 0;
                if (maxX > 16) maxX = 16;
                maxY = MathHelper::clamp(maxY, 1, 120);
                minY = MathHelper::clamp(minY, 1, maxY);
                if (maxY > 120) maxY = 120;
                if (minZ < 0) minZ = 0;
                if (maxZ > 16) maxZ = 16;

                for (int currentX = minX; currentX < maxX; ++currentX) {
                    c_double scaleX = ((double) (currentX + chunk.x * 16) + 0.5 - start.x) / tunnelWidthScaled;

                    for (int currentZ = minZ; currentZ < maxZ; ++currentZ) {
                        c_double scaleZ = ((double) (currentZ + chunk.z * 16) + 0.5 - start.z) / tunnelWidthScaled;

                        if (scaleX * scaleX + scaleZ * scaleZ < 1.0) {
                            //for (int currentY = maxY; currentY > minY; --currentY)
                            for (int currentY = maxY - 1; currentY >= minY; --currentY) {
                                c_double scaleY = ((double) currentY + 0.5 - start.y) / tunnelHeight;

                                if (scaleY > -0.7 && scaleX * scaleX + scaleY * scaleY + scaleZ * scaleZ < 1.0 &&
                                    currentY < 62) {

                                    if (canReplaceBlock(chunkPrimer->getBlockId(currentX, currentY, currentZ))) {
                                        if (currentY < 9) {
                                            chunkPrimer->setBlockId(currentX, currentY, currentZ, lce::blocks::STILL_LAVA_ID);
                                        } else if (currentY < 10) {
                                            if (rng.nextFloat() >= 0.25) {
                                                chunkPrimer->setBlockId(currentX, currentY, currentZ,
                                                                        lce::blocks::OBSIDIAN_ID);
                                            } else {
                                                chunkPrimer->setBlockId(currentX, currentY, currentZ,
                                                                        lce::blocks::MAGMA_BLOCK_ID);
                                            }
                                        } else {
                                            bool placedWater = false;
                                            if (currentX < 15) {
                                                if (chunkPrimer->getBlockId(currentX + 1, currentY, currentZ) ==
                                                    lce::blocks::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(currentX + 1, currentY, currentZ,
                                                                            lce::blocks::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (currentX > 0) {
                                                if (chunkPrimer->getBlockId(currentX - 1, currentY, currentZ) ==
                                                    lce::blocks::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(currentX - 1, currentY, currentZ,
                                                                            lce::blocks::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (currentZ < 15) {
                                                if (chunkPrimer->getBlockId(currentX, currentY, currentZ + 1) ==
                                                    lce::blocks::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(currentX, currentY, currentZ + 1,
                                                                            lce::blocks::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (currentZ > 0) {
                                                if (chunkPrimer->getBlockId(currentX, currentY, currentZ - 1) ==
                                                    lce::blocks::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(currentX, currentY, currentZ - 1,
                                                                            lce::blocks::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (!placedWater) {
                                                chunkPrimer->setBlockId(currentX, currentY, currentZ,
                                                                        lce::blocks::STILL_WATER_ID);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (isMainTunnel) { break; }
            }
        }
    }
}

void WaterCaveGenerator::addRoom(c_i64 seedModifier, const Pos2D target, ChunkPrimer* chunkPrimer,
                                 const DoublePos3D& roomStart, RNG& rng) {
    addTunnel(seedModifier, target, chunkPrimer, roomStart, 1.0F + rng.nextFloat() * 6.0F,
              0.0F, 0.0F,
              -1, -1, 0.5);
}


void WaterCaveGenerator::addFeature(c_int baseChunkX, c_int baseChunkZ, int targetX, int targetZ,
                                    ChunkPrimer* chunkPrimer, MU bool accurate) {
    c_int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt(40) + 1) + 1);

    if EXPECT_TRUE (rng.nextInt(15) != 0) { return; }

    for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
        auto tunnelStartZ = (double) (baseChunkZ * 16 + rng.nextInt(16));
        auto tunnelStartY = (double) rng.nextInt(rng.nextInt(120) + 8);
        auto tunnelStartX = (double) (baseChunkX * 16 + rng.nextInt(16));
        int segmentCount = 1;

        if (rng.nextInt(4) == 0) {
            addRoom(rng.nextLongI(), {targetX, targetZ}, chunkPrimer,
                    {tunnelStartX, tunnelStartY, tunnelStartZ}, rng);
            segmentCount = rng.nextInt(4) + 1;
        }

        for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
            c_float yaw = rng.nextFloat() * (PI_FLOAT * 2.0F);
            c_float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
            c_float tunnelLength = rng.nextFloat() * 2.0F + rng.nextFloat();

            addTunnel(rng.nextLongI(), {targetX, targetZ}, chunkPrimer,
                      {tunnelStartX, tunnelStartY, tunnelStartZ},
                      tunnelLength, yaw, pitch,
                      0, 0, 1.0);
        }
    }
}