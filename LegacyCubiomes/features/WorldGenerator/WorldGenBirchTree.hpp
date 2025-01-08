#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenBirchTree final : public WorldGenAbstractTree {
public:
    explicit WorldGenBirchTree(const bool use_extra_random_height)
        : useExtraRandomHeight(use_extra_random_height) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    bool useExtraRandomHeight;
};
