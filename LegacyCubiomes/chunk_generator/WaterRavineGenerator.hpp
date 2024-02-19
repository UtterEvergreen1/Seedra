#pragma once

#include "WaterCaveGeneratorBase.hpp"


class WaterRavineGenerator final : public WaterCaveGeneratorBase {

public:
    explicit WaterRavineGenerator(const Generator& generator) : WaterCaveGeneratorBase(generator) {}

    std::vector<float> rs = std::vector(128, 0.0F);

    void addTunnel(int64_t randomSeed, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D tunnel,
                   float angle, float slope, float curvature, int tunnelStartSegment, int tunnelEndSegment,
                   double widthMultiplier);

    void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer, bool accurate) override;
};




