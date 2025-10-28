#include "ChunkGenerator.hpp"

#include "terrain/biomes/biome.hpp"

#include "common/range.hpp"
#include "terrain/biomes/biomeDepthAndScale.hpp"


ChunkGeneratorOverWorld::ChunkGeneratorOverWorld(const Generator& generator) : g(&generator) {
    rng.setSeed(g->getWorldSeed());
    minLimitPerlinNoise.setNoiseGeneratorOctaves(rng, 16);
    maxLimitPerlinNoise.setNoiseGeneratorOctaves(rng, 16);
    mainPerlinNoise.setNoiseGeneratorOctaves(rng, 8);
    surfaceNoise.setNoiseGeneratorPerlin(rng, 4);
    scaleNoise.setNoiseGeneratorOctaves(rng, 10);
    depthNoise.setNoiseGeneratorOctaves(rng, 16);
    // forestNoise.setNoiseGeneratorOctaves(&rng, 8);
    depthBuffer.resize(256);
    heightMap.resize(825);
    biomeWeights.resize(25);

    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            c_float f = 10.0F / sqrt(static_cast<float>(i * i + j * j) + 0.2F);
            biomeWeights[i + 2 + (j + 2) * 5] = f;
        }
    }
    biomesForGeneration = nullptr;
}


ChunkGeneratorOverWorld::~ChunkGeneratorOverWorld() {
    if (biomesForGeneration) free(biomesForGeneration);
}


void ChunkGeneratorOverWorld::setBiomesForGeneration(c_int x, c_int z, c_int width, c_int height, c_int scale) {
    Range r{};
    // if ((scale & 0b1000000010101) == scale ) { // faster comparison
    if (scale == 1 || scale == 4 || scale == 16 || scale == 64 || scale == 256) {
        r.scale = scale;
    } else {
        r.scale = 1;
    }
    // Define the position and size for a horizontal area:
    r.x = x, r.z = z;            // position (x,z)
    r.sx = width, r.sz = height; // size (width,height)
    // Set the vertical range as a plane near sea level at scale 1:4.
    // r.y = 1, r.sy = 1;

    biome_t* biomeIds = g->allocCache(r);
    g->genBiomes(biomeIds, r);
    if (biomesForGeneration) free(biomesForGeneration);
    biomesForGeneration = biomeIds;
}


void ChunkGeneratorOverWorld::setBlocksInChunk(c_int chunkX, c_int chunkZ, ChunkPrimer* primer) {

    generateHeightmap(chunkX * 4, 0, chunkZ * 4);

    for (int subX = 0; subX < 4; ++subX) {
        int j = subX * 5;
        int k = (subX + 1) * 5;

        for (int subZ = 0; subZ < 4; ++subZ) {
            c_int i1 = (j + subZ) * 33;
            c_int j1 = (j + subZ + 1) * 33;
            c_int k1 = (k + subZ) * 33;
            c_int l1 = (k + subZ + 1) * 33;

            for (int subY = 0; subY < 32; ++subY) {

                double h00 = heightMap[i1 + subY];
                double h10 = heightMap[j1 + subY];
                double h01 = heightMap[k1 + subY];
                double h11 = heightMap[l1 + subY];

                double iy00 = (heightMap[i1 + subY + 1] - h00) * 0.125;
                double iy10 = (heightMap[j1 + subY + 1] - h10) * 0.125;
                double iy01 = (heightMap[k1 + subY + 1] - h01) * 0.125;
                double iy11 = (heightMap[l1 + subY + 1] - h11) * 0.125;

                for (int ym = 0; ym < 8; ++ym) {
                    double heightAtXZ0 = h00;
                    double heightAtXZ1 = h10;

                    c_double xiZ0 = (h01 - h00) * 0.25;
                    c_double xzZ1 = (h11 - h10) * 0.25;

                    static constexpr int SEA_LEVEL = 63;

                    for (int xm = 0; xm < 4; ++xm) {

                        double _hStep = (heightAtXZ1 - heightAtXZ0) * 0.25;
                        double lvt_45_1_ = heightAtXZ0 - _hStep;
                        // int defaultId = (subY * 8 + ym < SEA_LEVEL) ? 9 : 0;

                        for (int zm = 0; zm < 4; ++zm) {
                            int x = subX * 4 + xm;
                            int y = subY * 8 + ym;
                            int z = subZ * 4 + zm;
                            int distance;


                            int worldX = chunkX * 16 + x;
                            int worldZ = chunkZ * 16 + z;
                            double heightFalloff = getHeightFalloff(worldX, worldZ, &distance);

                            if (distance == 0) {
                                if (y <= SEA_LEVEL - 10)
                                    primer->setBlockId(x, y, z, 1);
                                else if (y < SEA_LEVEL)
                                    primer->setBlockId(x, y, z, 9);

                            } else {
                                if ((lvt_45_1_ += _hStep) > heightFalloff) {
                                    primer->setBlockId(x, y, z, 1);
                                } else if (y < SEA_LEVEL) {
                                    primer->setBlockId(x, y, z, 9);
                                }
                            }


//                            int blockId = defaultId;
//                            int distance;
//
//                            double heightFallOff = getHeightFallOff(chunkX * 16  + x, chunkZ * 16 + z, &distance);
//                            if (heightFallOff < (lvt_45_1_ += _hStep)) {
//                                blockId = 1;
//                            } else {
//                                blockId = defaultId;
//                            }
//
//                            if (distance == 0) {
//                                if (y <= SEA_LEVEL - 10)
//                                    blockId = 1;
//                                else if (y < SEA_LEVEL)
//                                    blockId = 9;
//                            }
//
//                            // if ((lvt_45_1_ += _hStep) > 0.0) {
//                            //     blockId = lce::blocks::STONE_ID;
//                            // } else if (y < SEA_LEVEL) {
//                            //     blockId = lce::blocks::STILL_WATER_ID;
//                            // }
//
//
//
//                            primer->setBlockId(x, y, z, blockId);
                        }

                        heightAtXZ0 += xiZ0;
                        heightAtXZ1 += xzZ1;
                    }

                    h00 += iy00;
                    h10 += iy10;
                    h01 += iy01;
                    h11 += iy11;
                }
            }
        }
    }
}


void ChunkGeneratorOverWorld::replaceBiomeBlocks(c_int chunkX, c_int chunkZ, ChunkPrimer* primer) {
    surfaceNoise.getRegion(depthBuffer, chunkX * 16, chunkZ * 16, 16, 16, 0.0625, 0.0625, 1.0);
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            Biome* biome = Biome::getBiomeForId(biomesForGeneration[j + i * 16]);
            biome->genTerrainBlocks(g->getWorldSeed(), rng, primer, chunkX * 16 + i, chunkZ * 16 + j, depthBuffer[j + i * 16]);
        }
    }
}


void ChunkGeneratorOverWorld::provideChunk(ChunkPrimer *chunkPrimer, c_int x,c_int z) {
    rng.setSeed((i64) x * 341873128712LL + (i64) z * 132897987541LL);
    setBiomesForGeneration(x * 4 - 2, z * 4 - 2, 10, 10, 4);
    setBlocksInChunk(x, z, chunkPrimer);
    setBiomesForGeneration(x * 16, z * 16, 16, 16, 1);
    replaceBiomeBlocks(x, z, chunkPrimer);
    chunkPrimer->stage = Stage::STAGE_WATER_CAVES;
}


void ChunkGeneratorOverWorld::generateHeightmap(c_int x, c_int y, c_int z) {
    depthNoise.genNoiseOctaves(g, depthRegion, x, 10, z, 5, 1, 5, 200.0, 1.0, 200.0);
    mainPerlinNoise.genNoiseOctaves(g, mainNoiseRegion, x, y, z, 5, 33, 5, 8.55515, 4.277575, 8.55515);
    minLimitPerlinNoise.genNoiseOctaves(g, minLimitRegion, x, y, z, 5, 33, 5, 684.412, 684.412, 684.412);
    maxLimitPerlinNoise.genNoiseOctaves(g, maxLimitRegion, x, y, z, 5, 33, 5, 684.412, 684.412, 684.412);
    int noiseIdx = 0;
    int depthIdx = 0;

    for (int cellX = 0; cellX < 5; ++cellX) {
        for (int cellZ = 0; cellZ < 5; ++cellZ) {
            float scaleAvg = 0.0F;
            float depthAvg = 0.0F;
            float weightSum = 0.0F;
            const biome_t centerBiome = biomesForGeneration[cellX + 2 + (cellZ + 2) * 10];

            for (int nbDX = -2; nbDX <= 2; ++nbDX) {
                for (int nbDZ = -2; nbDZ <= 2; ++nbDZ) {
                    const biome_t neighborBiome = biomesForGeneration[cellX + nbDX + 2 + (cellZ + nbDZ + 2) * 10];
                    double neighborDepth;
                    double neighborScale;
                    double neighborInvDPlus2;
                    // getBiomeDepthAndScale
                    //         (neighborBiome, &neighborDepth, &neighborScale, nullptr);
                    getBiomeDepthAndScale<true, true, false, true>
                            (neighborBiome, &neighborDepth, &neighborScale, nullptr, &neighborInvDPlus2);
                    /*
                    if (this->terrainType == WorldType.AMPLIFIED && f5 > 0.0F) {
                        f5 = 1.0F + f5 * 2.0F;
                        f6 = 1.0F + f6 * 4.0F;
                    }
                    */
                    // TODO: double being casted to float?
                    float w = biomeWeights[nbDX + 2 + (nbDZ + 2) * 5] * neighborInvDPlus2; // / (neighborDepth + 2.0F);

                    double centerDepth;
                    double neighborBaseDepth;
                    // getBiomeDepthAndScale(centerBiome, &centerDepth, nullptr, nullptr);
                    // getBiomeDepthAndScale(neighborBiome, &neighborBaseDepth, nullptr, nullptr);
                    getBiomeDepthAndScale<true, false, false, false>(centerBiome, &centerDepth, nullptr, nullptr, nullptr);
                    getBiomeDepthAndScale<true, false, false, false>(neighborBiome, &neighborBaseDepth, nullptr, nullptr, nullptr);
                    if (neighborBaseDepth > centerDepth) w /= 2.0F;

                    scaleAvg += neighborScale * w;
                    depthAvg += neighborDepth * w;
                    weightSum += w;
                }
            }

            scaleAvg = scaleAvg / weightSum;
            depthAvg = depthAvg / weightSum;
            scaleAvg = scaleAvg * 0.9F + 0.1F;
            depthAvg = (depthAvg * 4.0F - 1.0F) / 8.0F;
            double depthNoiseOffset = depthRegion[depthIdx] / 8000.0;

            if (depthNoiseOffset < 0.0) {
                depthNoiseOffset = -depthNoiseOffset * 0.3;
            }
            depthNoiseOffset = depthNoiseOffset * 3.0 - 2.0;

            if (depthNoiseOffset < 0.0) {
                depthNoiseOffset = depthNoiseOffset / 2.0;
                if (depthNoiseOffset < -1.0)
                    depthNoiseOffset = -1.0;
                depthNoiseOffset = depthNoiseOffset / 1.4;
                depthNoiseOffset = depthNoiseOffset / 2.0;
            } else {
                if (depthNoiseOffset > 1.0)
                    depthNoiseOffset = 1.0;
                depthNoiseOffset = depthNoiseOffset / 8.0;
            }

            ++depthIdx;
            auto depthBlend = (double) depthAvg;
            c_auto scaleAvgD = (double) scaleAvg;
            depthBlend = depthBlend + depthNoiseOffset * 0.2;
            depthBlend = depthBlend * (double) 8.5 / 8.0; // baseSize = 8.5
            c_double heightCenter = (double) 8.5 + depthBlend * 4.0;

            for (int yIdx = 0; yIdx < 33; ++yIdx) {
                double densityFalloff = ((double) yIdx - heightCenter) * (double) 12.0 * 128.0 / 256.0 / scaleAvgD;

                if (densityFalloff < 0.0) densityFalloff *= 4.0;

                c_double minNoise = minLimitRegion[noiseIdx] / (double) 512.0; // lowerLimitScale = 512.0
                c_double maxNoise = maxLimitRegion[noiseIdx] / (double) 512.0; // upperLimitScale = 512.0
                c_double mainBlend = (mainNoiseRegion[noiseIdx] / 10.0 + 1.0) / 2.0;
                double density = MathHelper::clampedLerp(mainBlend, minNoise, maxNoise) - densityFalloff;

                if (yIdx > 29) {
                    c_auto topFade = (double) ((float) (yIdx - 29) / 3.0F);
                    density = density * (1.0 - topFade) + -10.0 * topFade;
                }

                heightMap[noiseIdx] = density;
                ++noiseIdx;
            }
        }
    }
}




// NON_MATCHING | Score: 1630 (lower is better)
static int distanceToEdge(float a, int, int x, int z, int size) {
    using Pos3D_t = Pos3DTemplate<double>;

    int halfSize = size / 2;

    int minCoord = -halfSize;
    int maxCoord = halfSize - 1;

    auto topLeft = Pos3D_t(minCoord, 0.0f, minCoord);
    auto topRight = Pos3D_t(maxCoord, 0.0f, minCoord);
    auto bottomLeft = Pos3D_t(minCoord, 0.0f, maxCoord);
    auto bottomRight = Pos3D_t(maxCoord, 0.0f, maxCoord);

    float leftBound = minCoord - a;
    float rightBound = minCoord + a;

    float distance = a;

    bool inLeftBand = (x > leftBound) && (x < rightBound);
    bool inRightBand = (!inLeftBand) && (x > (maxCoord - a)) && (x < (maxCoord + a));

    if (inLeftBand || inRightBand) {
        auto p = Pos3D_t(x, 0.0f, z);
        auto edgeStart = (x < 1) ? topLeft : topRight;
        auto edgeEnd = (x < 1) ? bottomLeft : bottomRight;

        distance = p.distanceToSegment(edgeStart, edgeEnd);
    }

    if (((z > leftBound) && (z < rightBound)) || ((z > (maxCoord - a)) && (z < (maxCoord + a)))) {
        auto p = Pos3D_t(x, 0.0f, z);
        auto edgeStart = (z < 1) ? topLeft : topRight;
        auto edgeEnd = (z < 1) ? bottomLeft : bottomRight;

        float verticalDistance = p.distanceToSegment(edgeStart, edgeEnd);

        if (verticalDistance < distance) {
            distance = verticalDistance;
        }
    }

    return distance;
}




/*
double ChunkGeneratorOverWorld::getHeightFallOff(int blockX, int blockZ, int* distance) const {
    int size = 54 * 16;
    int nearestDist = distanceToEdge(32.0f, 0, blockX, blockZ, size);



    // MoatCheck moatChecks[] = {{mIsClassicMoat, 864}, {mIsSmallMoat, 1024}, {mIsMediumMoat, 3072}};


    if (size > 864 - 64) {
        int d = distanceToEdge(32.0f, nearestDist, blockX, blockZ, 864);
        if (d < nearestDist) {
            nearestDist = d;
        }
    }


    float result = 0.0f;
    if (nearestDist < 32) {
        result = (32 - nearestDist) * 0.03125f * 128.0f;
    }

    *distance = nearestDist;
    return result;
}*/


#include <algorithm>

double ChunkGeneratorOverWorld::getHeightFalloff(int blockX, int blockZ, int* distance) const {

    const int bounds = g->getWorldCoordinateBounds(); // (/*this->worldSize*/54 << 4) >> 1;

    const int distToMinX = std::max(0, blockX + bounds);
    const int distToMaxX = std::max(0, bounds - blockX - 1);
    const int distToMinZ = std::max(0, blockZ + bounds);
    const int distToMaxZ = std::max(0, bounds - blockZ - 1);

    const int nearestDist = std::min(std::min(distToMinX, distToMaxX),
                                     std::min(distToMinZ, distToMaxZ));

    const float heightFalloff = static_cast<float>(std::max(0, 32 - nearestDist)) * 0.03125f * 128.0f;
    *distance = nearestDist;
    return static_cast<double>(heightFalloff);
}