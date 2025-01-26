#pragma once

#include "AbstractWaterCaveGen.hpp"


class WaterRavineGenerator final : public AbstractWaterCaveGen {

public:

    explicit WaterRavineGenerator(const Generator& generator) : AbstractWaterCaveGen(generator) {}

    MU static Pos2DVec_t getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper);





    void addFeature(World& worldIn, Pos2D baseChunk, bool accurate) override;

    void addTunnel(World& worldIn, i64 randomSeed, Pos2D baseChunk,
                   DoublePos3D tunnel, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier);



    void addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    void addTunnel(ChunkPrimer* chunkPrimer, i64 randomSeed, Pos2D currentChunk,
                   DoublePos3D tunnel, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier);


private:
    std::vector<float> rs = std::vector(128, 0.0F);

};




