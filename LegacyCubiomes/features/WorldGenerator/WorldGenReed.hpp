#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class BlockReed {
public:
    static bool canBlockStay(const World* worldIn, const Pos3D& pos);
};

class WorldGenReed final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG& rand, const Pos3D& position) const override;
};