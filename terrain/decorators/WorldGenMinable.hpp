#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenMinable final : public AbstractWorldGenerator {
    const lce::BlockState oreBlock;
    int blockCount;

public:
    WorldGenMinable(const lce::BlockState block, const int blockCount) : oreBlock(block), blockCount(blockCount) {}

    bool generate(World* world, RNG& rng, const Pos3D& pos) const override;
};
