#pragma once
#include "WorldGenerator.hpp"

class WorldGenIceSpike final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};
