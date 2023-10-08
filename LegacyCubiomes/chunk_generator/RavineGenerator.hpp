#pragma once

#include <cmath>
#include "MapGenBase.hpp"


class RavineGenerator : public MapGenBase {
public:
    explicit RavineGenerator(const Generator &generator) : MapGenBase(generator) {}

    RavineGenerator(int64_t worldSeed, LCEVERSION lceVersion, CONSOLE console, BIOMESCALE biomeScale) :
        MapGenBase(worldSeed, lceVersion, console, biomeScale) {}

    std::vector<float> rs = std::vector<float>(128, 0.0F);

    unsigned char topBlock(int x, int z);

    static bool canReplaceBlock(uint16_t blockAt, uint16_t blockAbove);

    void addTunnel(int64_t randomSeed, Pos2D chunk, ChunkPrimer *chunkPrimer,
                   DoublePos3D tunnel, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier);

    void recursiveGenerate(int neighborChunkX, int neighborChunkZ,
                           int currentChunkX, int currentChunkZ,
                           ChunkPrimer *chunkPrimerIn) override;
};
