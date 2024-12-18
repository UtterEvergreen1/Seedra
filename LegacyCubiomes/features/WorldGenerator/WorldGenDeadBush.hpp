#pragma once
#include "WorldGenerator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenDeadBush final : public WorldGenerator {
public:
    bool generate(ChunkPrimer *worldIn, RNG &rand, const Pos3D &position) const override;
};