#pragma once
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class ChunkPrimer;

class WorldGenerator {
public:
    virtual ~WorldGenerator() = default;
    virtual bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const = 0;
};
