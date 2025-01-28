#pragma once


#include "lce/blocks/blocksInit.hpp"
#include "common/rng.hpp"

class StrongholdStones {
public:

    static lce::BlockState selectBlocks(RNG& rand, c_bool placeBlocks) {
        if (placeBlocks) {
            const float f = rand.nextFloat();
            if (f < 0.2F) { return lce::BlocksInit::CRACKED_STONE_BRICKS.getState(); }
            if (f < 0.5F) { return lce::BlocksInit::MOSSY_STONE_BRICKS.getState(); }
            if (f < 0.55F) { return lce::BlocksInit::STONE_BRICK_MONSTER_EGG.getState(); }
            return lce::BlocksInit::STONE_BRICKS.getState();
        }
        return lce::BlocksInit::AIR.getState();
    }
};






