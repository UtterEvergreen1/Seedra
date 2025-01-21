#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

class BlockHugeMushroom {
public:
    enum class EnumType {
        ALL_INSIDE,
        NORTH_WEST,
        NORTH,
        NORTH_EAST,
        WEST,
        CENTER,
        EAST,
        SOUTH_WEST,
        SOUTH,
        SOUTH_EAST,
        STEM,
        ALL_OUTSIDE = 14,
        ALL_STEM
    };
};


class WorldGenBigMushroom final : public AbstractWorldGenerator {
public:
    explicit WorldGenBigMushroom(lce::Block* mushroomType = nullptr) : mushroomType(mushroomType) {}

    bool generate(World * worldIn, RNG& rand, const Pos3D& position) const override;

private:
    lce::Block* mushroomType;
};