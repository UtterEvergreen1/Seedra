#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockVine {
public:
    static bool canPlaceBlockOnSide(World* worldIn, const Pos3D& pos, const EnumFacing& facing);

private:
    static bool canBlockStay(int blockId);
};

class WorldGenVines final : public AbstractWorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& position) const override;
};