#pragma once

#include "WorldGenerator.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenBlockBlob final : public WorldGenerator {
public:
    WorldGenBlockBlob(const lce::blocks::Block *blockIn, const int startRadiusIn)
        : block(blockIn), startRadius(startRadiusIn) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    const lce::blocks::Block *block;
    int startRadius;
};
