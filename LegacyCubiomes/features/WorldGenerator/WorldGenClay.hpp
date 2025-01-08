#pragma once

#include "WorldGenerator.hpp"


class World;

class WorldGenClay final : public WorldGenerator {
    int numberOfBlocks;

public:
    explicit WorldGenClay(const int numBlocks) : numberOfBlocks(numBlocks) {}

    bool generate(World* worldIn, RNG& rand, const Pos3D& position) const override;
};