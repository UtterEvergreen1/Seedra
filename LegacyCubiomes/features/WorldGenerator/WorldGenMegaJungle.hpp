#pragma once
#include "WorldGenHugeTrees.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenMegaJungle final : public WorldGenHugeTrees {
public:
    WorldGenMegaJungle(int baseHeight, int extraRandomHeight, const lce::blocks::Block* woodMetadata, const lce::blocks::Block* leavesMetadata)
        : WorldGenHugeTrees(baseHeight, extraRandomHeight, woodMetadata, leavesMetadata) {}

    bool generate(World * worldIn, RNG& rng, const Pos3D& pos) const override;

private:
    static void placeVine(World *worldIn, RNG& rand, const Pos3D& pos, FACING direction) ;
    void createCrown(World *worldIn, const Pos3D& pos, int width) const;
};