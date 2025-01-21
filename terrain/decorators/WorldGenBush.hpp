#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockBush {
public:
    static bool canPlaceBlockAt(World* world, const Pos3D& pos);

    static bool canBlockStay(World* world, const Pos3D& pos);
};

class WorldGenBush final : public AbstractWorldGenerator {
    const lce::Block* block;

public:
    explicit WorldGenBush(const lce::Block* blockIn) { this->block = blockIn; }

    bool generate(World* worldIn, RNG& rng, const Pos3D& pos) const override;
};
