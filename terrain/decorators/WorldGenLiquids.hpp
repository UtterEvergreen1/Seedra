#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenLiquids final : public AbstractWorldGenerator {
    int blockId;

public:
    explicit WorldGenLiquids(const int blockId) : blockId(blockId) {}

    bool generate(World * worldIn, RNG& rand, const Pos3D& position) const override;
};