#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockReed {
public:
    static bool canBlockStay(World* worldIn, const Pos3D& pos);
};

class WorldGenReed final : public AbstractWorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& position) const override;
};