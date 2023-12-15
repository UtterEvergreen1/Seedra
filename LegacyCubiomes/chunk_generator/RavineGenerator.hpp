#pragma once

#include "MapGenBase.hpp"
#include <cmath>


class RavineGenerator : public MapGenBase {
public:
    explicit RavineGenerator(const Generator& generator) : MapGenBase(generator) {}

    RavineGenerator(CONSOLE console, LCEVERSION version, int64_t seed, WORLDSIZE size, BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}

    std::vector<float> rs = std::vector<float>(128, 0.0F);

    unsigned char topBlock(int x, int z);

    static bool canReplaceBlock(uint16_t blockAt, uint16_t blockAbove);

    void addTunnel(int64_t randomSeed, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D tunnel, float angle,
                   float slope, float curvature, int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier);

    void addFeature(int neighborChunkX, int neighborChunkZ, int currentChunkX, int currentChunkZ,
                           ChunkPrimer* chunkPrimerIn) override;
};
