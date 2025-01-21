#pragma once

#include "AbstractMapGen.hpp"


class CaveGenerator final : public AbstractMapGen {

public:
    explicit CaveGenerator(const Generator& generator) : AbstractMapGen(generator) {}

    MU CaveGenerator(const lce::CONSOLE console, const LCEVERSION version,
                  c_i64 seed, const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
        : AbstractMapGen(console, version, seed, size, scale) {}

    ~CaveGenerator() override = default;

    ND unsigned char topBlock(int x, int z) const;

    static bool canReplaceBlock(u16 blockAt, u16 blockAbove);

    void addTunnel(i64 seedModifier, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D start, float tunnelWidth,
                   float tunnelDirection, float tunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   double tunnelHeightMultiplier, bool accurate);

    void addRoom(i64 seedModifier, Pos2D target, ChunkPrimer* chunkPrimer,
                 const DoublePos3D& roomStart, RNG& rng, bool accurate);

    void addFeature(int baseChunkX, int baseChunkZ, int currentChunkX, int currentChunkZ,
                    ChunkPrimer* chunkPrimer, bool accurate) override;
};
