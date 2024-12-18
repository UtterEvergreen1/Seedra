#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "lce/blocks/blocks.hpp"

class WorldGenIcePath final : public WorldGenerator {
public:

    explicit WorldGenIcePath(const int path_width)
        : pathWidth(path_width) {
    }

    bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    const lce::blocks::Block* pathBlock = &lce::blocks::PACKED_ICE;
    const int pathWidth;
};
