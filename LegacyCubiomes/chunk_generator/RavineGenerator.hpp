#pragma once

#include "MapGenBase.hpp"


class RavineGenerator final : public MapGenBase {

public:
    explicit RavineGenerator(const Generator& generator) : MapGenBase(generator) {}

    RavineGenerator(const lce::CONSOLE console, const LCEVERSION version,
                    c_i64 seed, const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}

    ~RavineGenerator() override = default;

    std::vector<float> rs = std::vector(128, 0.0F);

    ND unsigned char topBlock(int x, int z) const;

    static bool canReplaceBlock(u16 blockAt, u16 blockAbove);

    void addTunnel(i64 randomSeed, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D tunnel, float angle,
                   float slope, float curvature, int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier, bool accurate);

    void addFeature(int baseChunkX, int baseChunkZ, int currentChunkX, int currentChunkZ,
                           ChunkPrimer* chunkPrimer, bool accurate) override;
};
