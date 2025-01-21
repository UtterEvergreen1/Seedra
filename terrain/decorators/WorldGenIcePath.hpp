#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenIcePath final : public AbstractWorldGenerator {
public:

    explicit WorldGenIcePath(const int path_width)
        : pathWidth(path_width) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    static const lce::Block* PATH_BLOCK;
    const int pathWidth;
};
