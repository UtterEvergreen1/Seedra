#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenShrub final : public WorldGenAbstractTree {
    const lce::BlockState leavesMetadata;
    const lce::BlockState woodMetadata;

public:
    WorldGenShrub(const lce::BlockState wood, const lce::BlockState leaves)
        : leavesMetadata(leaves), woodMetadata(wood) {}

    bool generate(World* worldIn, RNG& rng, const Pos3D& position) const override;
};
