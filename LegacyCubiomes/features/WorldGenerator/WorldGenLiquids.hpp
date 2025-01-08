#pragma once

#include "WorldGenerator.hpp"


class World;

class WorldGenLiquids final : public WorldGenerator {
    int blockId;

public:
    explicit WorldGenLiquids(const int blockId) : blockId(blockId) {}

    bool generate(World * worldIn, RNG& rand, const Pos3D& position) const override;
};