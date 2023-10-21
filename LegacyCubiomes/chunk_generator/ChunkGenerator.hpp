#pragma once

#include <vector>
#include <string>
#include <ctime>

#include "ChunkPrimer.hpp"
#include "NoiseGenerator.hpp"


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
    //NoiseGeneratorOctaves forestNoise; // unused
    int *biomesForGeneration;
    std::vector<double> depthRegion;
    std::vector<double> depthBuffer;
    std::vector<double> heightMap;
    std::vector<float> biomeWeights;

    std::vector<double> mainNoiseRegion;
    std::vector<double> minLimitRegion;
    std::vector<double> maxLimitRegion;

    explicit ChunkGeneratorOverWorld(const Generator &generator);
    ~ChunkGeneratorOverWorld();

    void setBiomesForGeneration(int x, int z, int width, int height, int scale);
    void setBlocksInChunk(int chunkX, int chunkZ, ChunkPrimer *primer);
    void replaceBiomeBlocks(int x, int z, ChunkPrimer *primer, int *biomesIn);
    ChunkPrimer *provideChunk(int x, int z);
    inline ChunkPrimer *provideChunk(Pos2D chunkPos) {
        return provideChunk(chunkPos.x, chunkPos.z);
    };
    void generateHeightmap(int x, int y, int z);
};
