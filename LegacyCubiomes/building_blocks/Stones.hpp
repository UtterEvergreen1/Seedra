#pragma once


#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/blocks/blocks.hpp"

class Stones {
public:

    static const lce::blocks::Block* selectBlocks(RNG& rand, bool placeBlocks) {
        if (placeBlocks) {
            float f = rand.nextFloat();
            if (f < 0.2F) {
                return &lce::blocks::BlocksInit::CRACKED_STONE_BRICKS;
            } else if (f < 0.5F) {
                return &lce::blocks::BlocksInit::MOSSY_STONE_BRICKS;
            } else if (f < 0.55F) {
                return &lce::blocks::BlocksInit::STONE_BRICK_MONSTER_EGG;
            } else {
                return &lce::blocks::BlocksInit::STONE_BRICKS;
            }
        } else {
            return &lce::blocks::BlocksInit::AIR;
        }
    }
};






