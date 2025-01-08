#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenTaiga2 final : public WorldGenAbstractTree {
public:
    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};
