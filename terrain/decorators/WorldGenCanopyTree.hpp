#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenCanopyTree final : public WorldGenAbstractTree {
public:
    bool generate(World* worldIn, RNG& rand, const Pos3D& position) const override;

private:
    static bool placeTreeOfHeight(World* worldIn, const Pos3D& pos, int height);

    static void placeLogAt(World* worldIn, const Pos3D& pos);

    static void placeLeafAt(World* worldIn, int x, int y, int z);
};
