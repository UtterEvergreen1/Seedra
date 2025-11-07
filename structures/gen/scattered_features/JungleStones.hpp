#pragma once

#include "lce/blocks/blocksInit.hpp"
#include "common/rng.hpp"


class JunglePyramidStones {
public:
    static lce::BlockState selectBlocks(RNG &rng) {
        const float f = rng.nextFloat();
        if (f < 0.4F) { return lce::BlocksInit::COBBLESTONE.getState(); }
        return lce::BlocksInit::MOSS_STONE.getState();
    }
};
