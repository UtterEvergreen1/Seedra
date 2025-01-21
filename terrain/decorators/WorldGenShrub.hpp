#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenShrub final : public WorldGenAbstractTree {
    const lce::Block* leavesMetadata;
    const lce::Block* woodMetadata;

public:
    WorldGenShrub(const lce::Block* wood, const lce::Block* leaves)
        : leavesMetadata(leaves), woodMetadata(wood) {}

    bool generate(World* worldIn, RNG& rng, const Pos3D& position) const override;
};
