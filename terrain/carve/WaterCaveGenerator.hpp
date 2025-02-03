#pragma once

#include "AbstractWaterCaveGen.hpp"


class WaterCaveGenerator final : public AbstractWaterCaveGen {

public:
    explicit WaterCaveGenerator(World& world) : AbstractWaterCaveGen(world) {}

    MU static Pos2DVec_t getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper);


    void addFeature(World& worldIn, Pos2D baseChunk, bool accurate) override;


    void addRoom(World& worldIn, c_i64 seedModifier, Pos2D baseChunk, const DoublePos3D& roomStart, RNG& rng);


    void addTunnel(World& worldIn, c_i64 seedModifier, Pos2D baseChunk, DoublePos3D start, c_float theTunnelWidth,
                   float theTunnelDirection, float theTunnelSlope, int theCurrentSegment, int theMaxSegment,
                   c_double theHeightMultiplier);


    static bool canReplaceBlock(u16 blockAt);


    void addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    void addRoom(i64 seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, const DoublePos3D& roomStart, RNG& rng);

    void addTunnel(ChunkPrimer* chunkPrimer, c_i64 seedModifier, Pos2D chunk, DoublePos3D start, c_float theTunnelWidth,
                   float tunnelDirection, float theTunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   c_double theHeightMultiplier);
};
