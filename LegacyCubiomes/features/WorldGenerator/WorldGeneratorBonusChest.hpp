#pragma once

#include "WorldGenerator.hpp"


class World;

class WorldGeneratorBonusChest final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& pos) const override;
};