#pragma once

#include "WorldGenerator.hpp"


class World;

class WorldGenIceSpike final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};
