#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class ChunkPrimer;

class WorldGenMinable : public WorldGenerator {
    lce::blocks::Block const* oreBlock;
    int blockCount;

public:
    WorldGenMinable(lce::blocks::Block const* block, int blockCount) : oreBlock(block), blockCount(blockCount) {}
    bool generate(ChunkPrimer* chunk, RNG& rng, const Pos3D& pos) const override;
};
