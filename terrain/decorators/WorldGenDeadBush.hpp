#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockDeadBush {
public:
    static bool canPlaceBlockAt(World* world, const Pos3D& pos);

    static bool canSustainBush(int blockId);

    static bool canBlockStay(World* world, const Pos3D& pos);
};

class WorldGenDeadBush final : public AbstractWorldGenerator {
public:
    bool generate(World* worldIn, RNG& rng, const Pos3D& pos) const override;
};