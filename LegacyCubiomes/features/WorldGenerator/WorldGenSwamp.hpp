#pragma once
#include "WorldGenAbstractTree.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenSwamp final : public WorldGenAbstractTree {
public:
    bool generate(ChunkPrimer *worldIn, RNG &rand, const Pos3D &position) const override;

private:
    static void addVine(ChunkPrimer *worldIn, const Pos3D &pos, FACING direction);
};
