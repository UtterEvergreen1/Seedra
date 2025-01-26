#pragma once

#include "AbstractMapGen.hpp"


class RavineGenerator final : public AbstractMapGen {

public:

    explicit RavineGenerator(const Generator& generator) : AbstractMapGen(generator) {}

    ~RavineGenerator() override = default;



    MU static Pos2DVec_t getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper);



    void addFeature(World& worldIn, Pos2D baseChunk, bool accurate) override;

    void addTunnel(World& worldIn, i64 randomSeed, Pos2D baseChunk, DoublePos3D tunnel, float angle,
                   float slope, float curvature, int theStartSegment, int theEndSegment, double theWidthMultiplier,
                   bool accurate);



    void addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    void addTunnel(ChunkPrimer* chunkPrimer, i64 randomSeed, Pos2D currentChunk, DoublePos3D tunnel, float angle,
                   float slope, float curvature, int theStartSegment, int theEndSegment, double theWidthMultiplier,
                   bool accurate);


private:

    std::vector<float> rs = std::vector(128, 0.0F);

    ND unsigned char topBlock(int x, int z) const;

    static bool canReplaceBlock(u16 blockAt, u16 blockAbove);
};
