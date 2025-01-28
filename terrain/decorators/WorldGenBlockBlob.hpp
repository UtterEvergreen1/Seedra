#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenBlockBlob final : public AbstractWorldGenerator {
public:
    WorldGenBlockBlob(const lce::BlockState blockIn, const int startRadiusIn)
        : block(blockIn), startRadius(startRadiusIn) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    const lce::BlockState block;
    int startRadius;
};
