#pragma once

#include "WorldGenerator.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenIcePath final : public WorldGenerator {
public:

    explicit WorldGenIcePath(const int path_width)
        : pathWidth(path_width) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    static const lce::blocks::Block* PATH_BLOCK;
    const int pathWidth;
};
