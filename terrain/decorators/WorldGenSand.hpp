#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenSand final : public AbstractWorldGenerator {
    const lce::Block* block;
    int radius;

public:
    explicit WorldGenSand(const lce::Block* block, const int radiusIn) : block(block), radius(radiusIn) {}

    bool generate(World* worldIn, RNG& rand, const Pos3D& position) const override;
};