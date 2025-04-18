#pragma once

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"


class AbstractMapGen {
protected:
    static constexpr int CHUNK_RANGE = 8;
    static constexpr float RESERVE_MULTIPLIER = 1.4F;

public:
    World& world;
    Generator* g;
    RNG rng;
    BoundingBox genBounds = BoundingBox::EMPTY;

    explicit AbstractMapGen(World& world) : world(world), g(world.getGenerator()) {}

    virtual ~AbstractMapGen() = default;

    /// used for caves and ravines
    MU static Pos2DTemplate<i64> getSeedMultiplier(const Generator* g);
    /// used for caves and ravines
    MU void setupRNG(Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos);
    /// used for water caves and water ravines
    MU static void setupRNG(const Generator* g, RNG& rng, Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos);

    void setGenBounds(const BoundingBox& bounds) {
        genBounds = bounds;
    }


    void generate(ChunkPrimer* primer, Pos2D target, bool accurate = true) {
        Pos2DTemplate<i64> seedMultiplier = getSeedMultiplier(world.getGenerator());

        const Pos2D lower = target - CHUNK_RANGE;
        const Pos2D upper = target + CHUNK_RANGE;
        Pos2D chunkPos;
        for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
            for (chunkPos.z = lower.x; chunkPos.z <= upper.x; ++chunkPos.z) {
                setupRNG(seedMultiplier, chunkPos);
                addFeature(primer, chunkPos, target, accurate);
            }
        }
    }

    virtual void addFeature(World& worldIn, Pos2D baseChunk, bool accurate) = 0;

    virtual void addFeature(ChunkPrimer* chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) = 0;
};
