#pragma once

#include <cmath>
#include "MapGenBase.hpp"

class CaveGenerator : public MapGenBase {
public:
    explicit CaveGenerator(const Generator &generator) : MapGenBase(generator) {}

    CaveGenerator(int64_t worldSeed, LCEVERSION lceVersion, CONSOLE console, BIOMESCALE biomeScale) :
        MapGenBase(worldSeed, lceVersion, console, biomeScale) {}

    unsigned char topBlock(int x, int z);

    static bool canReplaceBlock(uint16_t blockAt, uint16_t blockAbove);

    void addTunnel(int64_t seedModifier, Pos2D chunk, ChunkPrimer *chunkPrimer, DoublePos3D start,
                   float tunnelWidth, float tunnelDirection, float tunnelSlope,
                   int currentTunnelSegment, int maxTunnelSegment, double tunnelHeightMultiplier);

    void addRoom(int64_t seedModifier, Pos2D target, ChunkPrimer *chunkPrimer, DoublePos3D roomStart, uint64_t *rng);

    void recursiveGenerate(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer *chunkPrimer) override;
};
