#pragma once
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class ChunkPrimer;

class WorldGenerator {
public:
    virtual ~WorldGenerator() = default;

    virtual bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const = 0;
};
