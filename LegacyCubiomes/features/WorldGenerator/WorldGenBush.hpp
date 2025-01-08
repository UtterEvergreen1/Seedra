#pragma once

#include "WorldGenerator.hpp"


namespace lce::blocks { class Block; }
class World;

class BlockBush {
public:
    static bool canPlaceBlockAt(World* world, const Pos3D& pos);

    static bool canBlockStay(World* world, const Pos3D& pos);
};

class WorldGenBush final : public WorldGenerator {
    const lce::blocks::Block* block;

public:
    explicit WorldGenBush(const lce::blocks::Block* blockIn) { this->block = blockIn; }

    bool generate(World* worldIn, RNG& rng, const Pos3D& pos) const override;
};
