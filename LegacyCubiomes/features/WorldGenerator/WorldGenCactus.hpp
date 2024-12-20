#pragma once
#include "WorldGenerator.hpp"

class BlockCactus {
public:
    static bool canBlockStay(const World *worldIn, const Pos3D &pos);
};

class WorldGenCactus final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};
