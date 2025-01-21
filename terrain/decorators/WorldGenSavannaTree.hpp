#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenSavannaTree final : public WorldGenAbstractTree {
public:
    bool generate(World * worldIn, RNG &rand, const Pos3D &position) const override;

private:
    static void placeLogAt(World *worldIn, const Pos3D &pos);

    static void placeLeafAt(World *worldIn, const Pos3D &pos);
};
