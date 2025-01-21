#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenBlockBlob final : public AbstractWorldGenerator {
public:
    WorldGenBlockBlob(const lce::Block *blockIn, const int startRadiusIn)
        : block(blockIn), startRadius(startRadiusIn) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    const lce::Block *block;
    int startRadius;
};
