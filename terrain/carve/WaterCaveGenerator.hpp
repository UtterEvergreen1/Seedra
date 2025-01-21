#pragma once

#include "AbstractWaterCaveGen.hpp"


class WaterCaveGenerator final : public AbstractWaterCaveGen {

public:
    explicit WaterCaveGenerator(const Generator& generator) : AbstractWaterCaveGen(generator) {}

    WaterCaveGenerator(const lce::CONSOLE console, const LCEVERSION version,
        c_i64 seed, const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
        : AbstractWaterCaveGen(console, version, seed, size, scale) {}

    static bool canReplaceBlock(u16 blockAt);

    void addTunnel(i64 seedModifier, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D start, float tunnelWidth,
                   float tunnelDirection, float tunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   double tunnelHeightMultiplier);

    void addRoom(i64 seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, const DoublePos3D& roomStart, RNG& rng);

    void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer, bool accurate) override;
};
