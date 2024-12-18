#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenClay final : public WorldGenerator {
    int numberOfBlocks;

public:
    explicit WorldGenClay(const int numBlocks) : numberOfBlocks(numBlocks) {}

    bool generate(ChunkPrimer* worldIn, RNG& rand, const Pos3D& position) const override;
};