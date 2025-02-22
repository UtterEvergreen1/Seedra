#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "NoiseGenerator.hpp"
#include "terrain/ChunkPrimer.hpp"


class ChunkGeneratorOverWorld {
public:
    Generator g;
    RNG rng{};
    NoiseGeneratorOctaves minLimitPerlinNoise;
    NoiseGeneratorOctaves maxLimitPerlinNoise;
    NoiseGeneratorOctaves mainPerlinNoise;
    NoiseGeneratorPerlin surfaceNoise;
    NoiseGeneratorOctaves scaleNoise; // unused
    NoiseGeneratorOctaves depthNoise;
    // NoiseGeneratorOctaves forestNoise; // unused

    int* biomesForGeneration;

    std::vector<double> depthRegion;
    std::vector<double> depthBuffer;
    std::vector<double> heightMap;
    std::vector<float> biomeWeights;
    std::vector<double> mainNoiseRegion;
    std::vector<double> minLimitRegion;
    std::vector<double> maxLimitRegion;

    explicit ChunkGeneratorOverWorld(const Generator& generator);
    ~ChunkGeneratorOverWorld();

    void setBiomesForGeneration(int x, int z, int width, int height, int scale);
    void setBlocksInChunk(int chunkX, int chunkZ, ChunkPrimer* primer);
    void replaceBiomeBlocks(int x, int z, ChunkPrimer* primer, c_int* biomesIn);

    void provideChunk(ChunkPrimer *chunkPrimer, int x, int z);
    MU void provideChunk(ChunkPrimer *chunkPrimer, const Pos2D& chunkPos) { provideChunk(chunkPrimer, chunkPos.x, chunkPos.z); }
    ChunkPrimer* provideNewChunk(int x, int z) {
        ChunkPrimer* chunkPrimer = new ChunkPrimer();
        provideChunk(chunkPrimer, x, z);
        return chunkPrimer;
    }
    MU ChunkPrimer* provideNewChunk(const Pos2D& chunkPos) { return provideNewChunk(chunkPos.x, chunkPos.z); }
    void generateHeightmap(int x, int y, int z);

};
