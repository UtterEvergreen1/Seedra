#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"

class WorldGenBlockBlob final : public WorldGenerator {
public:
    WorldGenBlockBlob(const lce::blocks::Block *blockIn, const int startRadiusIn)
        : block(blockIn), startRadius(startRadiusIn) {
    }

    bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    const lce::blocks::Block *block;
    int startRadius;
};
