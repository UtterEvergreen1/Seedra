#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockCactus {
public:
    static bool canBlockStay(World *worldIn, const Pos3D &pos);
};

class WorldGenCactus final : public AbstractWorldGenerator {
public:
    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};
