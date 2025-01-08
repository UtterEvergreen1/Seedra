#pragma once

#include "WorldGenerator.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenMinable final : public WorldGenerator {
    lce::blocks::Block const* oreBlock;
    int blockCount;

public:
    WorldGenMinable(lce::blocks::Block const* block, const int blockCount) : oreBlock(block), blockCount(blockCount) {}

    bool generate(World* world, RNG& rng, const Pos3D& pos) const override;
};
