#pragma once

#include "WorldGenHugeTrees.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenMegaJungle final : public WorldGenHugeTrees {
public:
    WorldGenMegaJungle(const int baseHeight, const int extraRandomHeight, const lce::blocks::Block* woodMetadata,
                       const lce::blocks::Block* leavesMetadata)
        : WorldGenHugeTrees(baseHeight, extraRandomHeight, woodMetadata, leavesMetadata) {}

    bool generate(World* worldIn, RNG& rng, const Pos3D& pos) const override;

private:
    static void placeVine(World* worldIn, RNG& rand, const Pos3D& pos, FACING direction);
    void createCrown(World* worldIn, const Pos3D& pos, int width) const;
};