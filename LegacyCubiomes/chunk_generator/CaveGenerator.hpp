#pragma once

#include "MapGenBase.hpp"


class CaveGenerator final : public MapGenBase {

public:
    explicit CaveGenerator(const Generator& generator) : MapGenBase(generator) {}

    CaveGenerator(const CONSOLE console, const LCEVERSION version,
                  const int64_t seed, const WORLDSIZE size, const BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}

    ~CaveGenerator() override = default;

    ND unsigned char topBlock(int x, int z) const;

    static bool canReplaceBlock(uint16_t blockAt, uint16_t blockAbove);

    void addTunnel(int64_t seedModifier, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D start, float tunnelWidth,
                   float tunnelDirection, float tunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   double tunnelHeightMultiplier, bool accurate);

    void addRoom(int64_t seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, const DoublePos3D& roomStart, RNG& rng, bool accurate);

    void addFeature(int baseChunkX, int baseChunkZ, int currentChunkX, int currentChunkZ,
                    ChunkPrimer* chunkPrimer, bool accurate) override;
};
