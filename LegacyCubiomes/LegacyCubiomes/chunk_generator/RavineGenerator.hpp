#pragma once

#include <cmath>
#include "MapGenBase.hpp"

class RavineGenerator : public MapGenBase
{
public:
    explicit RavineGenerator(const Generator& generator) : MapGenBase(generator) {}
    RavineGenerator(int64_t worldSeed, BIOMESCALE biomeScale) : MapGenBase(worldSeed, biomeScale) {}
    std::vector<float> rs = std::vector<float>(1024, 0.0F);
    unsigned char topBlock(int x, int z) {
        int biomeID = g.getBiomeAt(1, x, z);
        switch (biomeID) {
            case beach:
            case desert:
            case mesa:
                return Items::SAND_ID;
            case mushroom_field_shore:
            case mushroom_fields:
                return Items::MYCELIUM_ID;
            case snowy_beach:
            case snowy_mountains:
            case snowy_taiga:
            case snowy_taiga_hills:
            case snowy_tundra:
                return Items::SNOW_ID;
            case stone_beach:
                return Items::STONE_ID;
            default:
                return Items::GRASS_ID;
        }
    }
    void addTunnel(int64_t randomSeed, int chunkX, int chunkZ, ChunkPrimer* chunk, double tunnelX,
                   double tunnelY, double tunnelZ, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier)
    {
        uint64_t random;
        setSeed(&random, randomSeed);
        auto offsetX = (double)(chunkX * 16 + 8);
        auto offsetZ = (double)(chunkX * 16 + 8);
        float curvatureChangeRate = 0.0F;
        float slopeChangeRate = 0.0F;

        if (tunnelEndSegment <= 0)
        {
            int maxSegment = (int)this->range * 16 - 16;
            tunnelEndSegment = maxSegment - nextInt(&random, maxSegment / 4);
        }

        bool isSegmentAtCenter = false;

        if (tunnelStartSegment == -1)
        {
            tunnelStartSegment = tunnelEndSegment / 2;
            isSegmentAtCenter = true;
        }

        float tunnelRadiusMultiplier = 1.0F;

        for (int segment = 0; segment < 128; ++segment)
        {
            if (segment == 0 || nextInt(&random, 3) == 0)
            {
                tunnelRadiusMultiplier = 1.0F + nextFloat(&random) * nextFloat(&random);
            }

            this->rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
        }

        for (; tunnelStartSegment < tunnelEndSegment; tunnelStartSegment++)
        {
            double adjustedWidth = 1.5 + (double)(std::sin((float)tunnelStartSegment * (float)PI / (float)tunnelEndSegment) * angle);
            double adjustedHeight = adjustedWidth * widthMultiplier;
            adjustedWidth = adjustedWidth * ((double)nextFloat(&random) * 0.25 + 0.75);
            adjustedHeight = adjustedHeight * ((double)nextFloat(&random) * 0.25 + 0.75);
            float cosCurvature = std::cos(curvature);
            float sinCurvature = std::sin(curvature);
            tunnelX += (double)(std::cos(slope) * cosCurvature);
            tunnelY += (double)sinCurvature;
            tunnelZ += (double)(std::sin(slope) * cosCurvature);
            curvature = curvature * 0.7F;
            curvature = curvature + slopeChangeRate * 0.05F;
            slope += curvatureChangeRate * 0.05F;
            slopeChangeRate = slopeChangeRate * 0.8F;
            curvatureChangeRate = curvatureChangeRate * 0.5F;
            slopeChangeRate = slopeChangeRate + (nextFloat(&random) - nextFloat(&random)) * nextFloat(&random) * 2.0F;
            curvatureChangeRate = curvatureChangeRate + (nextFloat(&random) - nextFloat(&random)) * nextFloat(&random) * 4.0F;

            if (isSegmentAtCenter || nextInt(&random, 4) != 0)
            {
                double distanceX = tunnelX - offsetX;
                double distanceZ = tunnelZ - offsetZ;
                auto remainingSegments = (double)(tunnelEndSegment - tunnelStartSegment);
                auto maxDistance = (double)(angle + 2.0F + 16.0F);

                if (distanceX * distanceX + distanceZ * distanceZ - remainingSegments * remainingSegments > maxDistance * maxDistance)
                {
                    return;
                }

                if (tunnelX >= offsetX - 16.0 - adjustedWidth * 2.0 && tunnelZ >= offsetZ - 16.0 - adjustedWidth * 2.0 && tunnelX <= offsetX +
                                                                                                                                     16.0 + adjustedWidth * 2.0 && tunnelZ <= offsetZ + 16.0 + adjustedWidth * 2.0)
                {
                    int startX = (int)floor(tunnelX - adjustedWidth) - chunkX * 16 - 1;
                    int endX = (int)floor(tunnelX + adjustedWidth) - chunkX * 16 + 1;
                    int startY = (int)floor(tunnelY - adjustedHeight) - 1;
                    int endY = (int)floor(tunnelY + adjustedHeight) + 1;
                    int startZ = (int)floor(tunnelZ - adjustedWidth) - chunkZ * 16 - 1;
                    int endZ = (int)floor(tunnelZ + adjustedWidth) - chunkZ * 16 + 1;

                    if (startX < 0) startX = 0;
                    if (endX > 16) endX = 16;
                    if (startY < 1) startY = 1;
                    if (endY > 120) endY = 120;
                    if (startZ < 0) startZ = 0;
                    if (endZ > 16) endZ = 16;

                    bool waterOrLavaDetected = false;

                    for (int x = startX; !waterOrLavaDetected && x < endX; ++x)
                    {
                        for (int z = startZ; !waterOrLavaDetected && z < endZ; ++z)
                        {
                            for (int y = endY + 1; !waterOrLavaDetected && y >= startY - 1; --y)
                            {
                                if (y >= 0 && y < 256)
                                {
                                    uint16_t blockId = chunk->getBlock(x, y, z);
                                    if (blockId == Items::FLOWING_WATER_ID || blockId == Items::STILL_WATER_ID)
                                    {
                                        waterOrLavaDetected = true;
                                    }

                                    if (y != startY - 1 && x != startX && x != endX - 1 && z != startZ && z != endZ - 1)
                                    {
                                        y = startY;
                                    }
                                }
                            }
                        }
                    }

                    if (!waterOrLavaDetected)
                    {
                        for (int x = startX; x < endX; ++x)
                        {
                            double dX = ((double)(x + chunkX * 16) + 0.5 - tunnelX) / adjustedWidth;

                            for (int z = startZ; z < endZ; ++z)
                            {
                                double dZ = ((double)(z + chunkZ * 16) + 0.5 - tunnelZ) / adjustedWidth;
                                bool replaceableBlockDetected = false;

                                if (dX * dX + dZ * dZ < 1.0)
                                {
                                    for (int y = endY; y > startY; --y)
                                    {
                                        double dY = ((double)(y - 1) + 0.5 - tunnelY) / adjustedHeight;

                                        if ((dX * dX + dZ * dZ) * (double)this->rs[y - 1] + dY * dY / 6.0 < 1.0)
                                        {
                                            uint16_t iBlockState1 = chunk->getBlock(x, y, z);

                                            if (iBlockState1 == Items::GRASS_ID)
                                            {
                                                replaceableBlockDetected = true;
                                            }

                                            if (iBlockState1 == Items::STONE_ID ||
                                                iBlockState1 == Items::GRASS_ID ||
                                                iBlockState1 == Items::DIRT_ID)
                                            {
                                                if (y - 1 < 10)
                                                {
                                                    chunk->setBlock(x, y, z, Items::STILL_LAVA_ID);
                                                }
                                                else
                                                {
                                                    chunk->setBlock(x, y, z, Items::AIR_ID);

                                                    if (replaceableBlockDetected && chunk->getBlock(x, y - 1, z) == Items::DIRT_ID)
                                                    {
                                                        chunk->setBlock(x, y - 1, z, this->topBlock(x + chunkX * 16, z + chunkZ * 16));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (isSegmentAtCenter)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    void recursiveGenerate(int neighborChunkX, int neighborChunkZ, int currentChunkX, int currentChunkZ, ChunkPrimer* chunkPrimerIn) override
    {
        if EXPECT_FALSE(nextInt(&rng, 50) == 0)
        {
            double tunnelStartX = (double)(neighborChunkX * 16 + nextInt(&rng, 16));
            double tunnelStartY = (double)(nextInt(&rng, nextInt(&rng, 40) + 8) + 20);
            double tunnelStartZ = (double)(neighborChunkZ * 16 + nextInt(&rng, 16));
            float tunnelDirection = nextFloat(&rng) * ((float)PI * 2.0F);
            float tunnelSlope = (nextFloat(&rng) - 0.5F) * 2.0F / 8.0F;
            float tunnelLengthMultiplier = (nextFloat(&rng) * 2.0F + nextFloat(&rng)) * 2.0F;
            this->addTunnel((int64_t)nextLong(&rng), currentChunkX, currentChunkZ, chunkPrimerIn,
                            tunnelStartX, tunnelStartY, tunnelStartZ, tunnelLengthMultiplier,
                            tunnelDirection, tunnelSlope, 0, 0, 3.0);
        }
    }
};
