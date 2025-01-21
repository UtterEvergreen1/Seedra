#pragma once


#include "lce/blocks/blocksInit.hpp"
#include "common/rng.hpp"

class StrongholdStones {
public:

    static const lce::Block* selectBlocks(RNG& rand, bool placeBlocks) {
        if (placeBlocks) {
            const float f = rand.nextFloat();
            if (f < 0.2F) { return &lce::BlocksInit::CRACKED_STONE_BRICKS; }
            if (f < 0.5F) { return &lce::BlocksInit::MOSSY_STONE_BRICKS; }
            if (f < 0.55F) { return &lce::BlocksInit::STONE_BRICK_MONSTER_EGG; }
            return &lce::BlocksInit::STONE_BRICKS;
        }
        return &lce::BlocksInit::AIR;
    }
};






