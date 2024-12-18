#pragma once
#include "WorldGenAbstractTree.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenHugeTrees : public WorldGenAbstractTree {
public:
    WorldGenHugeTrees(int baseHeightIn, int extraRandomHeightIn, const lce::blocks::Block* woodMetadataIn, const lce::blocks::Block* leavesMetadataIn)
        : baseHeight(baseHeightIn), extraRandomHeight(extraRandomHeightIn), woodMetadata(woodMetadataIn), leavesMetadata(leavesMetadataIn) {}

protected:
    int getHeight(RNG& rand) const;

    static bool isSpaceAt(ChunkPrimer* worldIn, const Pos3D& leavesPos, int height);

    static bool ensureDirtsUnderneath(const Pos3D& pos, ChunkPrimer* worldIn);

    static bool ensureGrowable(ChunkPrimer* worldIn, RNG& rng, const Pos3D& treePos, int height);

    void growLeavesLayerStrict(ChunkPrimer* worldIn, const Pos3D& layerCenter, int width) const;

    void growLeavesLayer(ChunkPrimer* worldIn, const Pos3D& layerCenter, int width) const;

    int baseHeight;
    int extraRandomHeight;
    const lce::blocks::Block* woodMetadata;
    const lce::blocks::Block* leavesMetadata;
};