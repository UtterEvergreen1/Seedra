#pragma once
#include "WorldGenAbstractTree.hpp"
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenShrub final : public WorldGenAbstractTree {
    const lce::blocks::Block *leavesMetadata;
    const lce::blocks::Block *woodMetadata;

public:
    WorldGenShrub(const lce::blocks::Block *wood, const lce::blocks::Block *leaves)
        : leavesMetadata(leaves), woodMetadata(wood) {
    }

    bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &position) const override;
};
