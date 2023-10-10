#pragma once

#include <cmath>
#include "MapGenBase.hpp"

class RavineGenerator : public MapGenBase
{
public:
    explicit RavineGenerator(const Generator& generator) : MapGenBase(generator) {}
    RavineGenerator(int64_t worldSeed, LCEVERSION lceVersion, CONSOLE console, BIOMESCALE biomeScale) : MapGenBase(worldSeed, lceVersion, console, biomeScale) {}
    std::vector<float> rs = std::vector<float>(128, 0.0F);
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
    static bool canReplaceBlock(uint16_t blockAt, uint16_t blockAbove)
    {
        switch (blockAt) {
            case Items::STONE_ID:
            case Items::GRASS_ID:
            case Items::DIRT_ID:
            case Items::SANDSTONE_ID:
            case Items::SNOW_ID:
            case Items::MYCELIUM_ID:
            case Items::WHITE_HARDENED_CLAY_ID:
            case Items::HARDENED_CLAY_ID:
            case Items::RED_SANDSTONE_ID:
                return true;
            case Items::SAND_ID:
            case Items::GRAVEL_ID:
                return blockAbove != Items::AIR_ID &&
                       blockAbove != Items::STILL_WATER_ID; // wii u?
                //return blockAbove != Items::STILL_WATER_ID;
            default:
                return false;
        }
    }
    void addTunnel(int64_t randomSeed, int chunkX, int chunkZ, ChunkPrimer* chunk, double tunnelX,
                   double tunnelY, double tunnelZ, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier)
    {
        uint64_t random;
        setSeed(&random, randomSeed);
        double offsetX = (double)(chunkX * 16 + 8);
        double offsetZ = (double)(chunkZ * 16 + 8);
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
            double adjustedWidth = 1.5 + (double)(MathHelper::sin((float)tunnelStartSegment * PI_FLOAT / (float)tunnelEndSegment) * angle);
            double adjustedHeight = adjustedWidth * widthMultiplier;
            adjustedWidth = adjustedWidth * ((double)nextFloat(&random) * 0.25 + 0.75);
            adjustedHeight = adjustedHeight * ((double)nextFloat(&random) * 0.25 + 0.75);
            float cosCurvature = MathHelper::cos(curvature);
            float sinCurvature = MathHelper::sin(curvature);
            tunnelX += (double)(MathHelper::cos(slope) * cosCurvature);
            tunnelY += (double)sinCurvature;
            tunnelZ += (double)(MathHelper::sin(slope) * cosCurvature);
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
                double remainingSegments = (double)(tunnelEndSegment - tunnelStartSegment);
                double maxDistance = (double)(angle + 2.0F + 16.0F);

                //std::cout << distanceX << " " <<  distanceZ << " " << remainingSegments << " " << maxDistance << std::endl;

                if (distanceX * distanceX + distanceZ * distanceZ - remainingSegments * remainingSegments > maxDistance * maxDistance)
                {
                    //std::cout << "returned" << std::endl;
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
                                if (y >= 0 && y < 128)
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
                                    //startY - 1 <=
                                    //for (int y = endY; y > startY; --y)
                                    for (int y = endY - 1; y >= startY; --y)
                                    {
                                        double dY = ((double)y + 0.5 - tunnelY) / adjustedHeight;

                                        if ((dX * dX + dZ * dZ) * (double)this->rs[y] + dY * dY / 6.0 < 1.0)
                                        {
                                            uint16_t currentBlock = chunk->getBlock(x, y, z);
                                            uint16_t blockAbove = chunk->getBlock(x, y + 1, z);

                                            if (currentBlock == Items::GRASS_ID)
                                            {
                                                replaceableBlockDetected = true;
                                            }

                                            /*if (iBlockState1 == Items::STONE_ID ||
                                                iBlockState1 == Items::GRASS_ID ||
                                                iBlockState1 == Items::DIRT_ID)*/
                                            if(canReplaceBlock(currentBlock, blockAbove))
                                            {
                                                if (y < 11)
                                                {
                                                    chunk->setBlock(x, y, z, Items::FLOWING_LAVA_ID);
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
            float tunnelDirection = nextFloat(&rng) * (PI_FLOAT * 2.0F);
            float tunnelSlope = (nextFloat(&rng) - 0.5F) * 2.0F / 8.0F;
            float tunnelLengthMultiplier = (nextFloat(&rng) * 2.0F + nextFloat(&rng)) * 2.0F;
            this->addTunnel((int64_t)nextLong(&rng), currentChunkX, currentChunkZ, chunkPrimerIn,
                            tunnelStartX, tunnelStartY, tunnelStartZ, tunnelLengthMultiplier,
                            tunnelDirection, tunnelSlope, 0, 0, 3.0);
        }
    }
};
