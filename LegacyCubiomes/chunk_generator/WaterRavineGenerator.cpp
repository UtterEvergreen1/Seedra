#include "WaterRavineGenerator.hpp"
#include "WaterCaveGenerator.hpp"
void WaterRavineGenerator::addTunnel(int64_t randomSeed, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D tunnel,
               float angle, float slope, float curvature, int tunnelStartSegment, int tunnelEndSegment,
               double widthMultiplier) {

    if (!isOceanic(g.getBiomeAt(1, (int)tunnel.x, (int)tunnel.z)))
        return;

    RNG rng;
    rng.setSeed(randomSeed);
    auto offsetX = (double) (chunk.x * 16 + 8);
    auto offsetZ = (double) (chunk.z * 16 + 8);
    float curvatureChangeRate = 0.0F;
    float slopeChangeRate = 0.0F;

    if (tunnelEndSegment < 1) {
        constexpr int rangeBoundary = MapGenBase::range * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        tunnelEndSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isSegmentAtCenter = false;

    if (tunnelStartSegment == -1) {
        tunnelStartSegment = tunnelEndSegment / 2;
        isSegmentAtCenter = true;
    }

    float tunnelRadiusMultiplier = 1.0F;

    for (int segment = 0; segment < 128; ++segment) {
        if (segment == 0 || rng.nextInt(3) == 0) { tunnelRadiusMultiplier = 1.0F + rng.nextFloat() * rng.nextFloat(); }

        rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
    }

    for (; tunnelStartSegment < tunnelEndSegment; tunnelStartSegment++) {
        double adjustedWidth =
                1.5 +
                (double) (sinf((float) tunnelStartSegment * PI_FLOAT / (float) tunnelEndSegment) * angle);
        double adjustedHeight = adjustedWidth * widthMultiplier;
        adjustedWidth = adjustedWidth * ((double) rng.nextFloat() * 0.25 + 0.75);
        adjustedHeight = adjustedHeight * ((double) rng.nextFloat() * 0.25 + 0.75);
        float cosCurvature = cosf(curvature);
        float sinCurvature = sinf(curvature);
        tunnel.x += (double) (cosf(slope) * cosCurvature);
        tunnel.y += (double) sinCurvature;
        tunnel.z += (double) (sinf(slope) * cosCurvature);
        curvature = curvature * 0.7F;
        curvature = curvature + slopeChangeRate * 0.05F;
        slope += curvatureChangeRate * 0.05F;
        slopeChangeRate = slopeChangeRate * 0.8F;
        curvatureChangeRate = curvatureChangeRate * 0.5F;
        slopeChangeRate = slopeChangeRate + (rng.nextFloat() - rng.nextFloat()) * rng.nextFloat() * 2.0F;
        curvatureChangeRate = curvatureChangeRate + (rng.nextFloat() - rng.nextFloat()) * rng.nextFloat() * 4.0F;

        //good up to here
        if (isSegmentAtCenter || rng.nextInt(4) != 0) {
            double distanceX = tunnel.x - offsetX;
            double distanceZ = tunnel.z - offsetZ;
            auto remainingSegments = (double) (tunnelEndSegment - tunnelStartSegment);
            auto maxDistance = (double) (angle + 18.0F);

            if (distanceX * distanceX + distanceZ * distanceZ - remainingSegments * remainingSegments >
                maxDistance * maxDistance) {
                return;
            }

            if (tunnel.x >= offsetX - 16.0 - adjustedWidth * 2.0 && tunnel.z >= offsetZ - 16.0 - adjustedWidth * 2.0 &&
                tunnel.x <= offsetX + 16.0 + adjustedWidth * 2.0 && tunnel.z <= offsetZ + 16.0 + adjustedWidth * 2.0) {
                int startX = (int) floor(tunnel.x - adjustedWidth) - chunk.x * 16 - 1;
                int endX = (int) floor(tunnel.x + adjustedWidth) - chunk.x * 16 + 1;
                int startY = (int) floor(tunnel.y - adjustedHeight) - 1;
                int endY = (int) floor(tunnel.y + adjustedHeight) + 1;
                int startZ = (int) floor(tunnel.z - adjustedWidth) - chunk.z * 16 - 1;
                int endZ = (int) floor(tunnel.z + adjustedWidth) - chunk.z * 16 + 1;

                if (startX < 0) startX = 0;
                if (endX > 16) endX = 16;
                endY = MathHelper::clamp(endY, 1, 120);
                startY = MathHelper::clamp(startY, 1, endY);
                if (startZ < 0) startZ = 0;
                if (endZ > 16) endZ = 16;

                for (int x = startX; x < endX; ++x) {
                    double dX = ((double) (x + chunk.x * 16) + 0.5 - tunnel.x) / adjustedWidth;

                    for (int z = startZ; z < endZ; ++z) {
                        double dZ = ((double) (z + chunk.z * 16) + 0.5 - tunnel.z) / adjustedWidth;
                        bool replaceableBlockDetected = false;

                        double dxdxdzdz = dX * dX + dZ * dZ;
                        if (dxdxdzdz < 1.0) {
                            for (int y = endY - 1; y >= startY; --y) {
                                double dY = ((double) y + 0.5 - tunnel.y) / adjustedHeight;

                                if (dxdxdzdz * (double) rs[y - 1] + dY * dY / 6.0 < 1.0 && y < 62) {
                                    if (WaterCaveGenerator::canReplaceBlock(chunkPrimer->getBlockId(x, y, z))) {
                                        if (y < 10) {
                                            if (rng.nextFloat() >= 0.25) {
                                                chunkPrimer->setBlockId(x, y, z, Items::OBSIDIAN_ID);
                                            } else {
                                                chunkPrimer->setBlockId(x, y, z, Items::MAGMA_BLOCK_ID);
                                            }
                                        } else {
                                            bool placedWater = false;
                                            if (x < 15) {
                                                if (chunkPrimer->getBlockId(x + 1, y, z) == Items::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(x + 1, y, z, Items::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (x > 0) {
                                                if (chunkPrimer->getBlockId(x - 1, y, z) == Items::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(x - 1, y, z, Items::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (z < 15) {
                                                if (chunkPrimer->getBlockId(x, y, z + 1) == Items::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(x, y, z + 1, Items::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (z > 0) {
                                                if (chunkPrimer->getBlockId(x, y, z - 1) == Items::AIR_ID) {
                                                    placedWater = true;
                                                    chunkPrimer->setBlockId(x, y, z - 1, Items::FLOWING_WATER_ID);
                                                }
                                            }

                                            if (!placedWater) {
                                                chunkPrimer->setBlockId(x, y, z, Items::STILL_WATER_ID);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (isSegmentAtCenter) { break; }
            }
        }
    }
}

void WaterRavineGenerator::addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) {
    if EXPECT_FALSE (rng.nextInt(20) == 0) {
        auto tunnelStartX = (double) (baseChunkX * 16 + rng.nextInt(16));
        auto tunnelStartY = (double) (rng.nextInt(rng.nextInt(40) + 8) + 20);
        //if(rng.nextFloat() <= 0.2F)
            //tunnelStartY += 15.0;
        if((float)((double)((0x43300000LL << 32) | (rng.nextInt() ^ 0x80000000)) - 4503601774854144.0) <= 0.2F) {
            tunnelStartY += 15.0;
        }
        auto tunnelStartZ = (double) (baseChunkZ * 16 + rng.nextInt(16));
        float tunnelDirection = rng.nextFloat() * (PI_FLOAT * 2.0F);
        float tunnelSlope = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
        float tunnelLengthMultiplier = (rng.nextFloat() * 3.0F) + (rng.nextFloat() * 3.0F);
        if(rng.nextFloat() < 0.05)
            tunnelLengthMultiplier *= 2;
        addTunnel((int64_t) rng.nextLong(), {targetX, targetZ}, chunkPrimer,
                  {tunnelStartX, tunnelStartY, tunnelStartZ},
                  tunnelLengthMultiplier, tunnelDirection, tunnelSlope, 0, 0, 3.0);
    }
}
