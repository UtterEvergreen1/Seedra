#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class BlockReed {
public:
    static bool canBlockStay(World* worldIn, const Pos3D& pos);
};

class WorldGenReed final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& position) const override;
};