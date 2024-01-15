#pragma once

#include "WaterCaveGeneratorBase.hpp"


class WaterCaveGenerator final : public WaterCaveGeneratorBase {

public:
    explicit WaterCaveGenerator(const Generator& generator) : WaterCaveGeneratorBase(generator) {}

    WaterCaveGenerator(const CONSOLE console, const LCEVERSION version,
        const int64_t seed, const WORLDSIZE size, const BIOMESCALE scale)
        : WaterCaveGeneratorBase(console, version, seed, size, scale) {}

    static bool canReplaceBlock(uint16_t blockAt);

    void addTunnel(int64_t seedModifier, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D start, float tunnelWidth,
                   float tunnelDirection, float tunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   double tunnelHeightMultiplier);

    void addRoom(int64_t seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, const DoublePos3D& roomStart, RNG& rng);

    void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) override;
};
