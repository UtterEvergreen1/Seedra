#pragma once

#include <vector>
#include <string>
#include <ctime>

#include "ChunkPrimer.hpp"
#include "NoiseGenerator.hpp"

class ChunkGeneratorOverWorld
{
public:
    Generator g;
    uint64_t random;
    NoiseGeneratorOctaves minLimitPerlinNoise;
    NoiseGeneratorOctaves maxLimitPerlinNoise;
    NoiseGeneratorOctaves mainPerlinNoise;
    NoiseGeneratorPerlin surfaceNoise;
    NoiseGeneratorOctaves scaleNoise; // unused
    NoiseGeneratorOctaves depthNoise;
    //NoiseGeneratorOctaves forestNoise; // unused
    int* biomesForGeneration;
    std::vector<double> depthRegion;
    std::vector<double> depthBuffer;
    std::vector<double> heightMap;
    std::vector<float> biomeWeights;

    std::vector<double> mainNoiseRegion;
    std::vector<double> minLimitRegion;
    std::vector<double> maxLimitRegion;
    ChunkGeneratorOverWorld(const Generator& generator);
    ChunkGeneratorOverWorld(int64_t worldSeed, WORLDSIZE worldSize, BIOMESCALE biomeScale);
    ~ChunkGeneratorOverWorld();
    void setBiomesForGeneration(int x, int z, int width, int height, int scale);
    void setBlocksInChunk(int chunkX, int chunkZ, ChunkPrimer *primer);
    void replaceBiomeBlocks(int x, int z, ChunkPrimer *primer, int* biomesIn);
    ChunkPrimer* provideChunk(int x, int z);
    void generateHeightmap(int p_185978_1_, int p_185978_2_, int p_185978_3_);
    static double clampedLerp(double lowerBnd, double upperBnd, double slide) {
        if (slide < 0.0)
            return lowerBnd;
        else
            return slide > 1.0 ? upperBnd : lowerBnd + (upperBnd - lowerBnd) * slide;
    }
};
