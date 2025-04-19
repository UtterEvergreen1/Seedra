#pragma once

#include "lce/blocks/blocksInit.hpp"
#include "common/rng.hpp"

/**
 * @class StrongholdStones
 * @brief Handles the selection of block states for Stronghold structures.
 *
 * This class provides functionality to randomly select block states for Stronghold structures
 * based on specific probabilities, including cracked, mossy, and monster egg variants of stone bricks.
 */
class StrongholdStones {
public:
    /**
     * @brief Selects a block state for Stronghold structures based on random probabilities.
     *
     * This method uses a random number generator to determine which block state to return.
     * If `placeBlocks` is true, it selects from various stone brick types with specific probabilities:
     * - 20% chance for cracked stone bricks.
     * - 30% chance for mossy stone bricks.
     * - 5% chance for stone brick monster eggs.
     * - 45% chance for regular stone bricks.
     * If `placeBlocks` is false, it returns an air block state.
     *
     * @param rand The random number generator to use.
     * @param placeBlocks A boolean flag indicating whether to place blocks or return air.
     * @return The selected block state.
     */
    static lce::BlockState selectBlocks(RNG &rand, c_bool placeBlocks) {
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
