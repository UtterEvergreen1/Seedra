#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockVine
 * @brief Represents the vine block and provides utility methods for its placement and behavior.
 */
class BlockVine {
public:
    /**
     * @brief Checks if a vine block can be placed on a specific side of a block at the given position.
     * @param worldIn Pointer to the World object where the check is performed.
     * @param pos The position of the block to check.
     * @param facing The side of the block to check for vine placement.
     * @return True if the vine block can be placed, false otherwise.
     */
    static bool canPlaceBlockOnSide(World *worldIn, const Pos3D &pos, const EnumFacing &facing);

private:
    /**
     * @brief Checks if a block with the given ID can support a vine block.
     * @param blockId The ID of the block to check.
     * @return True if the block can support a vine, false otherwise.
     */
    static bool canBlockStay(int blockId);
};

/**
 * @class WorldGenVines
 * @brief A final class responsible for generating vine structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for vine generation.
 */
class WorldGenVines final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates vines at the specified position in the world.
     * @param worldIn Pointer to the World object where the vines will be generated.
     * @param rng Reference to the random number generator.
     * @param position The position where the vines should be generated.
     * @return True if the vine generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;
};
