#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenSand : public WorldGenerator {
    const lce::blocks::Block* block;
    int radius;

public:
    explicit WorldGenSand(const lce::blocks::Block* block, const int radiusIn) : block(block), radius(radiusIn) {}

    bool generate(World * worldIn, RNG& rand, const Pos3D& position) const override;
};