#pragma once

#include "AbstractMapGen.hpp"
#include <set>


class CaveGenerator final : public AbstractMapGen {

public:
    explicit CaveGenerator(World& world) : AbstractMapGen(world) {}

    ~CaveGenerator() override = default;

    MU static Pos2DVec_t getStartingChunks(const Generator* g, Pos2D lower, Pos2D upper);


    void addFeature(World& worldIn, Pos2D baseChunk, bool accurate) override;

    void addTunnel(World& worldIn, i64 theSeedModifier, Pos2D currentChunk, DoublePos3D start, float theWidth,
                   float theDirection, float theSlope, int theCurrentSegment, int theMaxSegment,
                   double theHeightMultiplier, bool accurate);

    void addRoom(World& worldIn, i64 seedModifier, Pos2D currentChunk, const DoublePos3D& roomStart, RNG& rng,
                 bool accurate);






    void addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    void addTunnel(ChunkPrimer* chunkPrimer, i64 theSeedModifier, Pos2D currentChunk, DoublePos3D startPos, float theWidth,
                   float theDirection, float theSlope, int theCurrentSegment, int theMaxSegment,
                   double theHeightMultiplier, bool accurate);

    void addRoom(ChunkPrimer* chunkPrimer, i64 seedModifier, Pos2D currentChunk, const DoublePos3D& roomStart, RNG& rng,
                 bool accurate);


private:

    ND unsigned char topBlock(int x, int z) const;

    static bool canReplaceBlock(u16 blockAt, u16 blockAbove);
};
