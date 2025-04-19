#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockDeadBush
 * @brief Represents a dead bush block and provides utility methods for placement and stability checks.
 */
class BlockDeadBush {
public:
    /**
     * @brief Checks if the dead bush block can be placed at the specified position in the world.
     * @param world Pointer to the World object.
     * @param pos The position to check for block placement.
     * @return True if the block can be placed, false otherwise.
     */
    static bool canPlaceBlockAt(World *world, const Pos3D &pos);

    /**
     * @brief Checks if the specified block ID can sustain a dead bush.
     * @param blockId The ID of the block to check.
     * @return True if the block can sustain a dead bush, false otherwise.
     */
    static bool canSustainBush(int blockId);

    /**
     * @brief Checks if the dead bush block can remain at the specified position in the world.
     * @param world Pointer to the World object.
     * @param pos The position to check for block stability.
     * @return True if the block can stay, false otherwise.
     */
    static bool canBlockStay(World *world, const Pos3D &pos);
};

/**
 * @class WorldGenDeadBush
 * @brief A final class responsible for generating dead bush structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for dead bush generation.
 */
class WorldGenDeadBush final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates a dead bush at the specified position in the world.
     * @param worldIn Pointer to the World object where the dead bush will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the dead bush should be generated.
     * @return True if the dead bush generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
