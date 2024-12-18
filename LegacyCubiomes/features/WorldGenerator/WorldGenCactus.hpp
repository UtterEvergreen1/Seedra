#pragma once
#include "WorldGenerator.hpp"

class BlockCactus {
public:
    static bool canBlockStay(const ChunkPrimer *worldIn, const Pos3D &pos);
};

class WorldGenCactus final : public WorldGenerator {
public:
    bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const override;
};
