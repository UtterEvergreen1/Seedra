#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenShrub
 * @brief A final class responsible for generating shrub structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for shrub generation.
 */
class WorldGenShrub final : public WorldGenAbstractTree {
    const lce::BlockState leavesMetadata; /**< The block state representing the type of leaves for the shrub. */
    const lce::BlockState woodMetadata; /**< The block state representing the type of wood for the shrub. */

public:
    /**
     * @brief Constructs a WorldGenShrub object with the specified wood and leaves block states.
     * @param wood The block state representing the type of wood for the shrub.
     * @param leaves The block state representing the type of leaves for the shrub.
     */
    WorldGenShrub(const lce::BlockState wood, const lce::BlockState leaves)
        : leavesMetadata(leaves), woodMetadata(wood) {
    }

    /**
     * @brief Generates a shrub at the specified position in the world.
     * @param worldIn Pointer to the World object where the shrub will be generated.
     * @param rng Reference to the random number generator.
     * @param position The position where the shrub should be generated.
     * @return True if the shrub generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;
};
