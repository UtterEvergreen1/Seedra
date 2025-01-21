#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGeneratorBonusChest final : public AbstractWorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& pos) const override;
};