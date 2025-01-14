#pragma once


#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/blocks/blocks.hpp"

class StrongholdStones {
public:

    static const lce::blocks::Block* selectBlocks(RNG& rand, bool placeBlocks) {
        if (placeBlocks) {
            const float f = rand.nextFloat();
            if (f < 0.2F) { return &lce::blocks::BlocksInit::CRACKED_STONE_BRICKS; }
            if (f < 0.5F) { return &lce::blocks::BlocksInit::MOSSY_STONE_BRICKS; }
            if (f < 0.55F) { return &lce::blocks::BlocksInit::STONE_BRICK_MONSTER_EGG; }
            return &lce::blocks::BlocksInit::STONE_BRICKS;
        }
        return &lce::blocks::BlocksInit::AIR;
    }
};






