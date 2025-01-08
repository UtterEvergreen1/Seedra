#pragma once

#include "WorldGenAbstractTree.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenHugeTrees : public WorldGenAbstractTree {
public:
    WorldGenHugeTrees(const int baseHeightIn, const int extraRandomHeightIn, const lce::blocks::Block* woodMetadataIn,
                      const lce::blocks::Block* leavesMetadataIn)
        : baseHeight(baseHeightIn), extraRandomHeight(extraRandomHeightIn), woodMetadata(woodMetadataIn),
          leavesMetadata(leavesMetadataIn) {}

protected:
    int getHeight(RNG& rand) const;

    static bool isSpaceAt(World* worldIn, const Pos3D& leavesPos, int height);

    static bool ensureDirtsUnderneath(const Pos3D& pos, World* worldIn);

    static bool ensureGrowable(World* worldIn, RNG& rng, const Pos3D& treePos, int height);

    void growLeavesLayerStrict(World* worldIn, const Pos3D& layerCenter, int width) const;

    void growLeavesLayer(World* worldIn, const Pos3D& layerCenter, int width) const;

    int baseHeight;
    int extraRandomHeight;
    const lce::blocks::Block* woodMetadata;
    const lce::blocks::Block* leavesMetadata;
};