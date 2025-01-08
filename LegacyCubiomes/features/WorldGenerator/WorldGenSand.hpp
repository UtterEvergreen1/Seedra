#pragma once

#include "WorldGenerator.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenSand final : public WorldGenerator {
    const lce::blocks::Block* block;
    int radius;

public:
    explicit WorldGenSand(const lce::blocks::Block* block, const int radiusIn) : block(block), radius(radiusIn) {}

    bool generate(World* worldIn, RNG& rand, const Pos3D& position) const override;
};