#include "ChunkGenerator.hpp"

#include "terrain/biomes/biome.hpp"

#include "common/range.hpp"
#include "common/timer.hpp"
#include "terrain/biomes/biomeDepthAndScale.hpp"
#include "terrain/noise/NoiseGen.hpp"


/*
for (int i = -2; i <= 2; ++i) {
    for (int j = -2; j <= 2; ++j) {
        c_float f = 10.0F / sqrt(static_cast<float>(i * i + j * j) + 0.2F);
        biomeWeights[i + 2 + (j + 2) * 5] = f;
    }
}
 */

double timeNoiseSetup = 0;

static_assert(sizeof(float) == sizeof(uint32_t), "float and uint32_t must be the same size");
static constexpr std::array<uint32_t, 25> BIOME_WEIGHTS = {
        0x405F7F69, 0x408C544C, 0x409C24DE, 0x408C544C, 0x405F7F69,
        0x408C544C, 0x40D7BE74, 0x41120F31, 0x40D7BE74, 0x408C544C,
        0x409C24DE, 0x41120F31, 0x41B2E2AC, 0x41120F31, 0x409C24DE,
        0x408C544C, 0x40D7BE74, 0x41120F31, 0x40D7BE74, 0x408C544C,
        0x405F7F69, 0x408C544C, 0x409C24DE, 0x408C544C, 0x405F7F69,
};


ChunkGeneratorOverWorld::ChunkGeneratorOverWorld(const Generator& generator)
    : g(&generator), chunkNoise(g->getChunkNoise()), rng(g->getChunkNoise().rng), depthRegion(), depthBuffer(),
      heightMap(), mainNoiseRegion(), minLimitRegion(), maxLimitRegion() {

    // rng.setSeed(g->getWorldSeed());
    // Timer time;
    // minLimitPerlinNoise.setNoiseGeneratorOctaves(rng, 16);
    // maxLimitPerlinNoise.setNoiseGeneratorOctaves(rng, 16);
    // mainPerlinNoise.setNoiseGeneratorOctaves(rng, 8);
    // surfaceNoise.setNoiseGeneratorPerlin(rng, 4);
    // scaleNoise.setNoiseGeneratorOctaves(rng, 10);
    // depthNoise.setNoiseGeneratorOctaves(rng, 16);
    // timeNoiseSetup += time.getSeconds();

    // forestNoise.setNoiseGeneratorOctaves(&rng, 8);

    biomesForGeneration = nullptr;
}


ChunkGeneratorOverWorld::~ChunkGeneratorOverWorld() {
    if (biomesForGeneration) free(biomesForGeneration);
}


void ChunkGeneratorOverWorld::setBiomesForGeneration(c_int x, c_int z, c_int width, c_int height,
                                                     c_u32 scale) {
    Range r{};
    // if ((scale & 0b1000000010101) == scale ) { // faster comparison
    if (scale == 1 || scale == 4 || scale == 16 || scale == 64 || scale == 256) {
        r.scale = scale;
    } else {
        r.scale = 1;
    }
    // Define the position and size for a horizontal area:
    r.x = x; r.z = z;            // position (x,z)
    r.sx = width; r.sz = height; // size (width,height)
    // Set the vertical range as a plane near sea level at scale 1:4.
    // r.y = 1, r.sy = 1;

    biome_t* biomeIds = g->allocCache(r);
    g->genBiomes(biomeIds, r);
    if (biomesForGeneration) free(biomesForGeneration);
    biomesForGeneration = biomeIds;
}


void ChunkGeneratorOverWorld::setBlocksInChunk(c_int chunkX, c_int chunkZ, ChunkPrimer* primer) {

    generateHeightmap(chunkX * 4, 0, chunkZ * 4);
    c_bool chunkHasHeightFalloff = hasHeightFalloff(chunkX, chunkZ);

    for (int subX = 0; subX < 4; ++subX) {
        c_int j = subX * 5;
        c_int k = (subX + 1) * 5;

        for (int subZ = 0; subZ < 4; ++subZ) {
            c_int i1 = (j + subZ) * 17;
            c_int j1 = (j + subZ + 1) * 17;
            c_int k1 = (k + subZ) * 17;
            c_int l1 = (k + subZ + 1) * 17;

            for (int subY = 0; subY < 16; ++subY) {

                double h00 = heightMap[i1 + subY];
                double h10 = heightMap[j1 + subY];
                double h01 = heightMap[k1 + subY];
                double h11 = heightMap[l1 + subY];

                c_double iy00 = (heightMap[i1 + subY + 1] - h00) * 0.125;
                c_double iy10 = (heightMap[j1 + subY + 1] - h10) * 0.125;
                c_double iy01 = (heightMap[k1 + subY + 1] - h01) * 0.125;
                c_double iy11 = (heightMap[l1 + subY + 1] - h11) * 0.125;

                for (int ym = 0; ym < 8; ++ym) {
                    c_int y = subY * 8 + ym;

                    double heightAtXZ0 = h00;
                    double heightAtXZ1 = h10;

                    c_double xiZ0 = (h01 - h00) * 0.25;
                    c_double xzZ1 = (h11 - h10) * 0.25;

                    static constexpr int SEA_LEVEL = 64;

                    for (int xm = 0; xm < 4; ++xm) {
                        c_int x = subX * 4 + xm;

                        c_double hStep = (heightAtXZ1 - heightAtXZ0) * 0.25;
                        double density = heightAtXZ0 - hStep;
                        // int defaultId = (subY * 8 + ym < SEA_LEVEL) ? 9 : 0;

                        if (chunkHasHeightFalloff) {
                            // Slower path with height falloff
                            for (int zm = 0; zm < 4; ++zm) {
                                c_int z = subZ * 4 + zm;
                                c_int worldX = chunkX * 16 + x;
                                c_int worldZ = chunkZ * 16 + z;

                                c_double heightFalloff = getHeightFalloff(worldX, worldZ);

                                density += hStep;

                                u16 blockId;
                                if (static_cast<int>(heightFalloff) == 128) {
                                    blockId = (y <= SEA_LEVEL - 10) ? 1 : ((y < SEA_LEVEL) ? 9 : 0);
                                } else {
                                    blockId = (density > heightFalloff) ? 1 : ((y < SEA_LEVEL) ? 9 : 0);
                                }

                                if (blockId != 0) {
                                    primer->setBlockId(x, y, z, blockId);
                                }
                            }
                        } else {
                            // Fast path: no height falloff, no world coordinate calculation
                            for (int zm = 0; zm < 4; ++zm) {
                                c_int z = subZ * 4 + zm;

                                density += hStep;

                                c_u16 blockId = (density > 0.0) ? 1 : ((y < SEA_LEVEL) ? 9 : 0);

                                if (blockId != 0) {
                                    primer->setBlockId(x, y, z, blockId);
                                }
                            }
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
    c_int blockX = chunkX * 16;
    c_int blockZ = chunkZ * 16;
    chunkNoise.surfaceNoise.getRegion<16, 16, 0.0625, 0.0625, 0.5, 1.0>(g->getConsole(), depthBuffer, blockX, blockZ);
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            Biome* biome = Biome::getBiomeForId(biomesForGeneration[j + i * 16]);
            biome->genTerrainBlocks(g->getWorldSeed(), rng, primer, blockX + i, blockZ + j,
                                    depthBuffer[static_cast<u64>(static_cast<u32>(j + i * 16))],
                                    biome->m_topBlock, biome->m_fillerBlock);
        }
    }
}


void ChunkGeneratorOverWorld::provideChunk(ChunkPrimer *chunkPrimer, c_int x,c_int z) {
    rng.setSeed(static_cast<u64>(static_cast<i64>(x) * 341873128712LL + static_cast<i64>(z) * 132897987541LL));
    setBiomesForGeneration(x * 4 - 2, z * 4 - 2, 10, 10, 4);
    setBlocksInChunk(x, z, chunkPrimer);
    setBiomesForGeneration(x * 16, z * 16, 16, 16, 1);
    replaceBiomeBlocks(x, z, chunkPrimer);
    chunkPrimer->stage = Stage::STAGE_WATER_CAVES;
}


void ChunkGeneratorOverWorld::generateHeightmap(c_int x, c_int y, c_int z) {
    chunkNoise.tempNoise.          getRegion<5, 5, 0.025 / 1.5, 0.025 / 1.5, 0.5, 0.25>(g->getConsole(), tempRegion,     x * 4, z * 4);
    chunkNoise.humidNoise.         getRegion<5, 5, 0.05 / 1.5, 0.05 / 1.5, 0.5, 1.0 / 3.0>(g->getConsole(), humidRegion,     x * 4, z * 4);
    chunkNoise.detailNoise.        getRegion<5, 5, 0.25 / 1.5, 0.25 / 1.5, 0.5, 0.5882352941176471>(g->getConsole(), detailRegion,     x * 4, z * 4);

    for(int i = 0; i < 25; ++i) {
        double detail_blend = detailRegion[i] * 1.1 + 0.5;

        // --- BLEND TEMPERATURE ---
        double temp = (tempRegion[i] * 0.15 + 0.7) * 0.99 + (detail_blend * 0.01);

        // Square the inverse
        temp = 1.0 - (1.0 - temp) * (1.0 - temp);

        // Clamp 0.0 to 1.0
        if(temp < 0.0) temp = 0.0;
        if(temp > 1.0) temp = 1.0;

        tempRegion[i] = temp;

        // --- BLEND HUMIDITY ---
        double humid = (humidRegion[i] * 0.15 + 0.5) * 0.998 + (detail_blend * 0.002);

        // Clamp 0.0 to 1.
        if(humid < 0.0) humid = 0.0;
        if(humid > 1.0) humid = 1.0;

        humidRegion[i] = humid;
    }

    chunkNoise.scaleNoise.         getRegion<double, 5,  1, 5, 1.121,   1.0,      1.121  >(g, scaleRegion,     x, 10, z);
    chunkNoise.depthNoise.         getRegion<double, 5,  1, 5, 200.0,   1.0,      200.0  >(g, depthRegion,     x, 10, z);
    chunkNoise.maxLimitPerlinNoise.getRegion<double, 5, 17, 5, 684.412, 684.412,  684.412>(g, maxLimitRegion,  x,  y, z);
    chunkNoise.minLimitPerlinNoise.getRegion<double, 5, 17, 5, 684.412, 684.412,  684.412>(g, minLimitRegion,  x,  y, z);
    chunkNoise.mainPerlinNoise.    getRegion<double, 5, 17, 5, 8.55515, 4.277575, 8.55515>(g, mainNoiseRegion, x,  y, z);
    size_t limitIdx = 0;
    int gridIdx = 0;
    constexpr int y_segments = 17;

    for (int cellX = 0; cellX < 5; ++cellX) {
        for (int cellZ = 0; cellZ < 5; ++cellZ) {
            double temp = tempRegion[gridIdx];
            double humid = humidRegion[gridIdx];

            double tempHumidFactor = humid * temp;
            double climateMultiplier = 1.0 - tempHumidFactor;
            climateMultiplier *= climateMultiplier;
            climateMultiplier *= climateMultiplier;
            climateMultiplier = 1.0 - climateMultiplier;

            // 2. Calculate Volatility (Terrain Roughness)
            double volatility = (scaleRegion[gridIdx] + 256.0) / 512.0;
            volatility *= climateMultiplier;
            if(volatility > 1.0) volatility = 1.0;

            // 3. Calculate Raw Base Height
            double rawBaseHeight = depthRegion[gridIdx] / 8000.0;

            // Replicate the exact Beta 1.6.6 shaping logic
            if(rawBaseHeight < 0.0) rawBaseHeight = -rawBaseHeight * 0.3;
            rawBaseHeight = rawBaseHeight * 3.0 - 2.0;

            if(rawBaseHeight < 0.0) {
                rawBaseHeight /= 2.0;
                if(rawBaseHeight < -1.0) rawBaseHeight = -1.0;
                rawBaseHeight /= 1.4;
                rawBaseHeight /= 2.0;
                volatility = 0.0;
            } else {
                if(rawBaseHeight > 1.0) rawBaseHeight = 1.0;
                rawBaseHeight /= 8.0;
            }

            if(volatility < 0.0) volatility = 0.0;
            volatility += 0.5;

            rawBaseHeight = rawBaseHeight * y_segments / 16.0;

            // Final Base Height for this X/Z column
            double baseHeight = y_segments / 2.0 + rawBaseHeight * 4.0;

            for(int y_segment = 0; y_segment < y_segments; ++y_segment) {
                double interpolatedNoise = 0.0;
                double heightFalloff = ((double)y_segment - baseHeight) * 12.0 / volatility;
                if(heightFalloff < 0.0) {
                    heightFalloff *= 4.0;
                }

                double maxLimit = maxLimitRegion[limitIdx] / 512.0;
                double minLimit = minLimitRegion[limitIdx] / 512.0;
                double mainNoiseBlend = (mainNoiseRegion[limitIdx] / 10.0 + 1.0) / 2.0;
                if(mainNoiseBlend < 0.0) {
                    interpolatedNoise = maxLimit;
                } else if(mainNoiseBlend > 1.0) {
                    interpolatedNoise = minLimit;
                } else {
                    interpolatedNoise = maxLimit + (minLimit - maxLimit) * mainNoiseBlend;
                }

                interpolatedNoise -= heightFalloff;
                if(y_segment > y_segments - 4) {
                    double topFade = (double)((float)(y_segment - (y_segments - 4)) / 3.0F);
                    interpolatedNoise = interpolatedNoise * (1.0 - topFade) + -10.0 * topFade;
                }

                heightMap[limitIdx] = interpolatedNoise;
                ++limitIdx;
            }


            ++gridIdx;
        }
    }
}

inline bool ChunkGeneratorOverWorld::hasHeightFalloff(c_int chunkX, c_int chunkZ) const {
    c_int b = g->getWorldChunkBounds(); // half-size (world in [-b, b-1])

    // Fast interior reject: strictly >31 from all 4 edges => falloff is 0.
    return b - std::abs(chunkX + (chunkX > 0)) < HEIGHT_FALLOFF_BORDER_CHUNKS ||
           b - std::abs(chunkZ + (chunkZ > 0)) < HEIGHT_FALLOFF_BORDER_CHUNKS;
}

inline double ChunkGeneratorOverWorld::getHeightFalloff(c_int blockX, c_int blockZ) const {
    // BORDER = 32; result = max(0, 32 - nearestDist) * 4

    const int b = g->getWorldCoordinateBounds(); // half-size (world in [-b, b-1])

    c_int distToMinX = std::max(0, blockX + b);
    c_int distToMaxX = std::max(0, b - blockX - 1);
    c_int distToMinZ = std::max(0, blockZ + b);
    c_int distToMaxZ = std::max(0, b - blockZ - 1);

    c_int nearestDist = std::min(std::min(distToMinX, distToMaxX),
                                 std::min(distToMinZ, distToMaxZ));

    c_int h = HEIGHT_FALLOFF_BORDER - nearestDist;
    if (h <= 0) return 0.0;
    return static_cast<double>(h << 2); // *4 via shift
}
