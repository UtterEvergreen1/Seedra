#pragma once
#include "WorldGenAbstractTree.hpp"

class WorldGenTaiga2 : public WorldGenAbstractTree {
public:
    bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const override;
};
