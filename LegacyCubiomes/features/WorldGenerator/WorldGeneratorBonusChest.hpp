#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGeneratorBonusChest : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& pos) const override;
};