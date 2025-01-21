#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenMinable final : public AbstractWorldGenerator {
    lce::Block const* oreBlock;
    int blockCount;

public:
    WorldGenMinable(lce::Block const* block, const int blockCount) : oreBlock(block), blockCount(blockCount) {}

    bool generate(World* world, RNG& rng, const Pos3D& pos) const override;
};
