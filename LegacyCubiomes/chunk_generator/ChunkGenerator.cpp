#include "ChunkGenerator.hpp"
#include "LegacyCubiomes/utils/range.hpp"
#include "biome.hpp"


ChunkGeneratorOverWorld::ChunkGeneratorOverWorld(const Generator &generator) : g(generator) {
    rng.setSeed(g.getWorldSeed());
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
            float f = 10.0F / sqrt((float) (i * i + j * j) + 0.2F);
            biomeWeights[i + 2 + (j + 2) * 5] = f;
        }
    }
    biomesForGeneration = nullptr;
}


ChunkGeneratorOverWorld::~ChunkGeneratorOverWorld() {
    if (biomesForGeneration)
        free(biomesForGeneration);
}


void ChunkGeneratorOverWorld::setBiomesForGeneration(int x, int z, int width, int height, int scale) {
    Range r{};
    // if ((scale & 0b1000000010101) == scale ) { // faster comparison
    if (scale == 1 || scale == 4 || scale == 16 || scale == 64 || scale == 256) {
        r.scale = scale;
    } else {
        r.scale = 1;
    }
    // Define the position and size for a horizontal area:
    r.x = x, r.z = z;   // position (x,z)
    r.sx = width, r.sz = height; // size (width,height)
    // Set the vertical range as a plane near sea level at scale 1:4.
    // r.y = 1, r.sy = 1;
    int *biomeIds = g.allocCache(r);
    g.genBiomes(biomeIds, r);
    if (biomesForGeneration)
        free(biomesForGeneration);
    biomesForGeneration = biomeIds;
}


void ChunkGeneratorOverWorld::setBlocksInChunk(int chunkX, int chunkZ, ChunkPrimer *primer) {

    setBiomesForGeneration(chunkX * 4 - 2, chunkZ * 4 - 2, 10, 10, 4);
    generateHeightmap(chunkX * 4, 0, chunkZ * 4);

    for (int i = 0; i < 4; ++i) {
        int j = i * 5;
        int k = (i + 1) * 5;

        for (int l = 0; l < 4; ++l) {
            int i1 = (j + l) * 33;
            int j1 = (j + l + 1) * 33;
            int k1 = (k + l) * 33;
            int l1 = (k + l + 1) * 33;

            for (int i2 = 0; i2 < 32; ++i2) {
                double d1 = heightMap[i1 + i2];
                double d2 = heightMap[j1 + i2];
                double d3 = heightMap[k1 + i2];
                double d4 = heightMap[l1 + i2];
                double d5 = (heightMap[i1 + i2 + 1] - d1) * 0.125;
                double d6 = (heightMap[j1 + i2 + 1] - d2) * 0.125;
                double d7 = (heightMap[k1 + i2 + 1] - d3) * 0.125;
                double d8 = (heightMap[l1 + i2 + 1] - d4) * 0.125;

                for (int j2 = 0; j2 < 8; ++j2) {
                    double d10 = d1;
                    double d11 = d2;
                    double d12 = (d3 - d1) * 0.25;
                    double d13 = (d4 - d2) * 0.25;

                    for (int k2 = 0; k2 < 4; ++k2) {
                        double d16 = (d11 - d10) * 0.25;
                        double lvt_45_1_ = d10 - d16;

                        for (int l2 = 0; l2 < 4; ++l2) {
                            if ((lvt_45_1_ += d16) > 0.0) {
                                primer->setBlock(i * 4 + k2, i2 * 8 + j2, l * 4 + l2, Items::STONE_ID);
                            } else if (i2 * 8 + j2 < 63) { // 63 is sea level
                                primer->setBlock(i * 4 + k2, i2 * 8 + j2, l * 4 + l2, Items::STILL_WATER_ID);
                            }
                        }

                        d10 += d12;
                        d11 += d13;
                    }

                    d1 += d5;
                    d2 += d6;
                    d3 += d7;
                    d4 += d8;
                }
            }
        }
    }
}


void ChunkGeneratorOverWorld::replaceBiomeBlocks(int x, int z, ChunkPrimer *primer, int *biomesIn) {
    depthBuffer = surfaceNoise.getRegion(depthBuffer, (double) (x * 16), (double) (z * 16), 16, 16, 0.0625, 0.0625, 1.0);
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            Biome *biome = Biome::getBiomeForId(biomesIn[j + i * 16]);
            biome->genTerrainBlocks(g.getWorldSeed(), rng, primer, x * 16 + i, z * 16 + j, depthBuffer[j + i * 16]);
        }
    }
}


ChunkPrimer* ChunkGeneratorOverWorld::provideChunk(int x, int z) {
    rng.setSeed((int64_t) x * 341873128712LL + (int64_t) z * 132897987541LL);
    auto *chunkPrimer = new ChunkPrimer();
    setBlocksInChunk(x, z, chunkPrimer);
    setBiomesForGeneration(x * 16, z * 16, 16, 16, 1);
    replaceBiomeBlocks(x, z, chunkPrimer, biomesForGeneration);
    return chunkPrimer;
}


void ChunkGeneratorOverWorld::generateHeightmap(int x, int y, int z) {
    // double depthNoiseScaleX = 200.0;
    // double depthNoiseScaleZ = 200.0;
    // double depthNoiseScaleExponent = 0.5;
    // double mainNoiseScaleX = 80.0;
    // double mainNoiseScaleY = 160.0;
    // double mainNoiseScaleZ = 80.0;
    depthRegion = depthNoise.genNoiseOctaves(depthRegion, x, z, 5, 5, 200.0, 200.0, 0.5);
    mainNoiseRegion = mainPerlinNoise.genNoiseOctaves(mainNoiseRegion, x, y, z, 5, 33, 5,
                                                      8.55515, 4.277575, 8.55515);
    minLimitRegion = minLimitPerlinNoise.genNoiseOctaves(minLimitRegion, x, y, z, 5, 33, 5,
                                                         684.412, 684.412, 684.412);
    maxLimitRegion = maxLimitPerlinNoise.genNoiseOctaves(maxLimitRegion, x, y, z, 5, 33, 5,
                                                         684.412, 684.412, 684.412);
    int i = 0;
    int j = 0;

    for (int k = 0; k < 5; ++k) {
        for (int l = 0; l < 5; ++l) {
            float f2 = 0.0F;
            float f3 = 0.0F;
            float f4 = 0.0F;
            int biome = biomesForGeneration[k + 2 + (l + 2) * 10];

            for (int j1 = -2; j1 <= 2; ++j1) {
                for (int k1 = -2; k1 <= 2; ++k1) {
                    int biome1 = biomesForGeneration[k + j1 + 2 + (l + k1 + 2) * 10];
                    double f5;
                    double f6;
                    getBiomeDepthAndScale(biome1, &f5, &f6, 0);
                    /* if (this->terrainType == WorldType.AMPLIFIED && f5 > 0.0F)
                     {
                         f5 = 1.0F + f5 * 2.0F;
                         f6 = 1.0F + f6 * 4.0F;
                     }
                     */
                    float f7 = biomeWeights[j1 + 2 + (k1 + 2) * 5] / (f5 + 2.0F);

                    double biomeBaseHeight;
                    double biome1BaseHeight;
                    getBiomeDepthAndScale(biome, &biomeBaseHeight, 0, 0);
                    getBiomeDepthAndScale(biome1, &biome1BaseHeight, 0, 0);
                    if (biome1BaseHeight > biomeBaseHeight)
                        f7 /= 2.0F;

                    f2 += f6 * f7;
                    f3 += f5 * f7;
                    f4 += f7;
                }
            }

            f2 = f2 / f4;
            f3 = f3 / f4;
            f2 = f2 * 0.9F + 0.1F;
            f3 = (f3 * 4.0F - 1.0F) / 8.0F;
            double d7 = depthRegion[j] / 8000.0;

            if (d7 < 0.0)
                d7 = -d7 * 0.3;

            d7 = d7 * 3.0 - 2.0;

            if (d7 < 0.0) {
                d7 = d7 / 2.0;

                if (d7 < -1.0)
                    d7 = -1.0;

                d7 = d7 / 1.4;
                d7 = d7 / 2.0;
            } else {
                if (d7 > 1.0)
                    d7 = 1.0;

                d7 = d7 / 8.0;
            }

            ++j;
            auto d8 = (double) f3;
            auto d9 = (double) f2;
            d8 = d8 + d7 * 0.2;
            d8 = d8 * (double) 8.5 / 8.0; //baseSize = 8.5
            double d0 = (double) 8.5 + d8 * 4.0;

            for (int l1 = 0; l1 < 33; ++l1) {
                double d1 = ((double) l1 - d0) * (double) 12.0 * 128.0 / 256.0 / d9;

                if (d1 < 0.0)
                    d1 *= 4.0;

                double d2 = minLimitRegion[i] / (double) 512.0; // lowerLimitScale = 512.0
                double d3 = maxLimitRegion[i] / (double) 512.0; // upperLimitScale = 512.0
                double d4 = (mainNoiseRegion[i] / 10.0 + 1.0) / 2.0;
                // TODO: this could be the problem??? swap to d2, d3, d4
                double d5 = MathHelper::clampedLerp(d4, d2, d3) - d1;

                if (l1 > 29) {
                    auto d6 = (double) ((float) (l1 - 29) / 3.0F);
                    d5 = d5 * (1.0 - d6) + -10.0 * d6;
                }

               heightMap[i] = d5;
                ++i;
            }
        }
    }
}
