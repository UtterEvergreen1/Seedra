#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenSwamp final : public WorldGenAbstractTree {
public:
    bool generate(World * worldIn, RNG &rand, const Pos3D &position) const override;

private:
    static void addVine(World *worldIn, const Pos3D &pos, EnumFacing facing);
};
