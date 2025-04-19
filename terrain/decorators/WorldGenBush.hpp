#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockBush
 * @brief Represents a bush block and provides utility methods for placement and stability checks.
 */
class BlockBush {
public:
    /**
     * @brief Checks if the bush block can be placed at the specified position in the world.
     * @param world Pointer to the World object.
     * @param pos The position to check for block placement.
     * @return True if the block can be placed, false otherwise.
     */
    static bool canPlaceBlockAt(World *world, const Pos3D &pos);

    /**
     * @brief Checks if the bush block can remain at the specified position in the world.
     * @param world Pointer to the World object.
     * @param pos The position to check for block stability.
     * @return True if the block can stay, false otherwise.
     */
    static bool canBlockStay(World *world, const Pos3D &pos);
};

/**
 * @class WorldGenBush
 * @brief A final class responsible for generating bush structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for bush generation.
 */
class WorldGenBush final : public AbstractWorldGenerator {
    lce::BlockState block; /**< The block state used for the bush. */

public:
    /**
     * @brief Constructs a WorldGenBush object with the specified block state.
     * @param blockIn The block state to use for the bush.
     */
    explicit WorldGenBush(const lce::BlockState blockIn) : block(blockIn) {
    }

    /**
     * @brief Generates a bush at the specified position in the world.
     * @param worldIn Pointer to the World object where the bush will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the bush should be generated.
     * @return True if the bush generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
