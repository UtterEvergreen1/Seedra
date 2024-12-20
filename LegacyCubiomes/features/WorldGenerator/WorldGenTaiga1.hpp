#pragma once
#include "WorldGenAbstractTree.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/blocks/blocks.hpp"

class WorldGenTaiga1 final : public WorldGenAbstractTree {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& pos) const override;
};