#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenLiquids : public WorldGenerator {
    int blockId;

public:
    explicit WorldGenLiquids(int blockId) : blockId(blockId) {}

    bool generate(World * worldIn, RNG& rand, const Pos3D& position) const override;
};