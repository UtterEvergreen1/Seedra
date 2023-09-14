#pragma once

#include <cmath>
#include "MapGenBase.hpp"

class CaveGenerator : public MapGenBase
{
public:
    explicit CaveGenerator(const Generator& generator) : MapGenBase(generator) {}
    CaveGenerator(int64_t worldSeed, BIOMESCALE biomeScale) : MapGenBase(worldSeed, biomeScale) {}
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
            return Items::SNOW_BLOCK_ID;
        case stone_beach:
            return Items::STONE_ID;
        default:
            return 2;
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
            return true;
        case Items::SAND_ID:
        case Items::GRAVEL_ID:
            return blockAbove != Items::STILL_WATER_ID;
        default:
            return false;
        }
    }
    void addTunnel(int64_t seedModifier, int chunkX, int chunkZ, ChunkPrimer* chunk, double startX,
                   double startY, double startZ, float tunnelWidth, float tunnelDirection, float tunnelSlope,
                   int currentTunnelSegment, int maxTunnelSegment, double tunnelHeightMultiplier)
    {
        auto targetCenterX = (double)(chunkX * 16 + 8);
        auto targetCenterZ = (double)(chunkZ * 16 + 8);
        float directionModifier = 0.0F;
        float slopeModifier = 0.0F;
        uint64_t random;
        setSeed(&random, seedModifier);

        if (maxTunnelSegment <= 0)
        {
            int rangeBoundary = (int)this->range * 16 - 16;
            maxTunnelSegment = rangeBoundary - nextInt(&random, rangeBoundary / 4);
        }

        bool isMainTunnel = false;

        if (currentTunnelSegment == -1)
        {
            currentTunnelSegment = maxTunnelSegment / 2;
            isMainTunnel = true;
        }

        int splitPoint = nextInt(&random, maxTunnelSegment / 2) + maxTunnelSegment / 4;

        for (bool isTunnelWide = nextInt(&random, 6) == 0; currentTunnelSegment < maxTunnelSegment; ++currentTunnelSegment)
        {
            double tunnelWidthScaled = 1.5 + (double)(std::sin((float)currentTunnelSegment * (float)PI / (float)maxTunnelSegment) * tunnelWidth);
            double tunnelHeight = tunnelWidthScaled * tunnelHeightMultiplier;
            float directionCosine = std::cos(tunnelSlope);
            float directionSine = std::sin(tunnelSlope);
            startX += (double)(std::cos(tunnelDirection) * directionCosine);
            startY += (double)directionSine;
            startZ += (double)(std::sin(tunnelDirection) * directionCosine);

            if (isTunnelWide)
            {
                tunnelSlope = tunnelSlope * 0.92F;
            }
            else
            {
                tunnelSlope = tunnelSlope * 0.7F;
            }

            tunnelSlope = tunnelSlope + slopeModifier * 0.1F;
            tunnelDirection += directionModifier * 0.1F;
            slopeModifier = slopeModifier * 0.9F;
            directionModifier = directionModifier * 0.75F;
            slopeModifier = slopeModifier + (nextFloat(&random) - nextFloat(&random)) * nextFloat(&random) * 2.0F;
            directionModifier = directionModifier + (nextFloat(&random) - nextFloat(&random)) * nextFloat(&random) * 4.0F;

            if (!isMainTunnel && currentTunnelSegment == splitPoint && tunnelWidth > 1.0F && maxTunnelSegment > 0)
            {
                addTunnel((int64_t)nextLong(&random), chunkX, chunkZ, chunk, startX, startY, startZ, nextFloat(&random) * 0.5F + 0.5F, tunnelDirection - ((float)PI / 2.0F), tunnelSlope / 3.0F, currentTunnelSegment, maxTunnelSegment, 1.0);
                addTunnel((int64_t)nextLong(&random), chunkX, chunkZ, chunk, startX, startY, startZ, nextFloat(&random) * 0.5F + 0.5F, tunnelDirection + ((float)PI / 2.0F), tunnelSlope / 3.0F, currentTunnelSegment, maxTunnelSegment, 1.0);
                return;
            }

            if (isMainTunnel || nextInt(&random, 4) != 0)
            {
                double distanceX = startX - targetCenterX;
                double distanceZ = startZ - targetCenterZ;
                auto segmentsRemaining = (double)(maxTunnelSegment - currentTunnelSegment);
                auto maxDistance = (double)(tunnelWidth + 18.0F);

                if (distanceX * distanceX + distanceZ * distanceZ - segmentsRemaining * segmentsRemaining > maxDistance * maxDistance)
                {
                    return;
                }

                if (startX >= targetCenterX - 16.0 - tunnelWidthScaled * 2.0 && startZ >= targetCenterZ - 16.0 - tunnelWidthScaled * 2.0 && startX <= targetCenterX + 16.0 + tunnelWidthScaled * 2.0 && startZ <= targetCenterZ + 16.0 + tunnelWidthScaled * 2.0)
                {
                    int minX = (int)floor(startX - tunnelWidthScaled) - chunkX * 16 - 1;
                    int maxX = (int)floor(startX + tunnelWidthScaled) - chunkX * 16 + 1;
                    int minY = (int)floor(startY - tunnelHeight) - 1;
                    int maxY = (int)floor(startY + tunnelHeight) + 1;
                    int minZ = (int)floor(startZ - tunnelWidthScaled) - chunkZ * 16 - 1;
                    int maxZ = (int)floor(startZ + tunnelWidthScaled) - chunkZ * 16 + 1;

                    if (minX < 0) minX = 0;
                    if (maxX > 16) maxX = 16;
                    if (minY < 1) minY = 1;
                    if (maxY > 120) maxY = 120;
                    if (minZ < 0) minZ = 0;
                    if (maxZ > 16) maxZ = 16;

                    bool hasWater = false;

                    for (int currentX = minX; !hasWater && currentX < maxX; ++currentX)
                    {
                        for (int currentZ = minZ; !hasWater && currentZ < maxZ; ++currentZ)
                        {
                            for (int currentY = maxY + 1; !hasWater && currentY >= minY - 1; --currentY)
                            {
                                if (currentY >= 0 && currentY < 128)
                                {
                                    uint16_t blockType = chunk->getBlock(currentX, currentY, currentZ);
                                    if (blockType == Items::STILL_WATER_ID || blockType == Items::FLOWING_WATER_ID)
                                    {
                                        hasWater = true;
                                    }

                                    if (currentY != minY - 1 && currentX != minX && currentX != maxX - 1 && currentZ != minZ && currentZ != maxZ - 1)
                                    {
                                        currentY = minY;
                                    }
                                }
                            }
                        }
                    }

                    if (!hasWater)
                    {
                        for (int currentX = minX; currentX < maxX; ++currentX)
                        {
                            double scaleX = ((double)(currentX + chunkX * 16) + 0.5 - startX) / tunnelWidthScaled;

                            for (int currentZ = minZ; currentZ < maxZ; ++currentZ)
                            {
                                double scaleZ = ((double)(currentZ + chunkZ * 16) + 0.5 - startZ) / tunnelWidthScaled;
                                bool isTopBlock = false;

                                if (scaleX * scaleX + scaleZ * scaleZ < 1.0)
                                {
                                    for (int currentY = maxY; currentY > minY; --currentY)
                                    {
                                        double scaleY = ((double)(currentY - 1) + 0.5 - startY) / tunnelHeight;

                                        if (scaleY > -0.7 && scaleX * scaleX + scaleY * scaleY + scaleZ * scaleZ < 1.0)
                                        {
                                            uint16_t currentBlock = chunk->getBlock(currentX, currentY, currentZ);
                                            uint16_t blockAbove = chunk->getBlock(currentX, currentY + 1, currentZ);

                                            if (currentBlock == Items::GRASS_ID || currentBlock == Items::MYCELIUM_ID)
                                            {
                                                isTopBlock = true;
                                            }

                                            if (canReplaceBlock(currentBlock, blockAbove))
                                            {
                                                if (currentY < 11)
                                                {
                                                    chunk->setBlock(currentX, currentY, currentZ, Items::STILL_LAVA_ID);
                                                }
                                                else
                                                {
                                                    chunk->setBlock(currentX, currentY, currentZ, Items::AIR_ID);
                                                    if (isTopBlock && chunk->getBlock(currentX, currentY - 1, currentZ) == Items::DIRT_ID)
                                                    {
                                                        chunk->setBlock(currentX, currentY - 1, currentZ, this->topBlock(currentX + chunkX * 16, currentZ + chunkZ * 16));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (isMainTunnel)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    void addRoom(int64_t seedModifier, int targetX, int targetZ, ChunkPrimer* chunkPrimer, double roomStartX, double roomStartY, double roomStartZ, uint64_t* random)
    {
        return this->addTunnel(seedModifier, targetX, targetZ, chunkPrimer, roomStartX, roomStartY, roomStartZ, 1.0F + nextFloat(random) * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
    }

    void recursiveGenerate(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) override
    {
        int tunnelCount = nextInt(&rng, nextInt(&rng, nextInt(&rng, 40) + 1) + 1);

        if EXPECT_FALSE(nextInt(&rng, 15) != 0)
        {
            return;
        }

        for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel)
        {
            double tunnelStartX = (double)(baseChunkX * 16 + nextInt(&rng, 16));
            double tunnelStartY = (double)nextInt(&rng, nextInt(&rng, 120) + 8);
            double tunnelStartZ = (double)(baseChunkZ * 16 + nextInt(&rng, 16));
            int segmentCount = 1;

            if (nextInt(&rng, 4) == 0)
            {
                addRoom((int64_t)nextLong(&rng), targetX, targetZ, chunkPrimer, tunnelStartX, tunnelStartY, tunnelStartZ, &rng);
                segmentCount = nextInt(&rng, 4) + 1;
            }

            for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment)
            {
                float yaw = nextFloat(&rng) * ((float)PI * 2.0F);
                float pitch = (nextFloat(&rng) - 0.5F) * 2.0F / 8.0F;
                float tunnelLength = nextFloat(&rng) * 2.0F + nextFloat(&rng);

                if (nextInt(&rng, 10) == 0)
                {
                    tunnelLength *= nextFloat(&rng) * nextFloat(&rng) * 3.0F + 1.0F;
                }

                addTunnel((int64_t)nextLong(&rng), targetX, targetZ, chunkPrimer, tunnelStartX, tunnelStartY, tunnelStartZ, tunnelLength, yaw, pitch, 0, 0, 1.0);
            }
        }
    }
};
