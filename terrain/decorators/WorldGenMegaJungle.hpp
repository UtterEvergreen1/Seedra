#pragma once

#include "WorldGenHugeTrees.hpp"


class World;

class WorldGenMegaJungle final : public WorldGenHugeTrees {
public:
    WorldGenMegaJungle(const int baseHeight, const int extraRandomHeight, const lce::Block* woodMetadata,
                       const lce::Block* leavesMetadata)
        : WorldGenHugeTrees(baseHeight, extraRandomHeight, woodMetadata, leavesMetadata) {}

    bool generate(World* worldIn, RNG& rng, const Pos3D& pos) const override;

private:
    static void placeVine(World* worldIn, RNG& rand, const Pos3D& pos, EnumFacing facing);
    void createCrown(World* worldIn, const Pos3D& pos, int width) const;
};