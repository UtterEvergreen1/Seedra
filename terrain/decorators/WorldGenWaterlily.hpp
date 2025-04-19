#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockLilyPad
 * @brief Represents the lily pad block and provides utility methods for its placement and behavior.
 */
class BlockLilyPad {
public:
    /**
     * @brief Checks if a lily pad block can be placed at the specified position in the world.
     * @param worldIn Pointer to the World object where the check is performed.
     * @param pos The position to check for lily pad placement.
     * @return True if the lily pad block can be placed, false otherwise.
     */
    static bool canPlaceBlockAt(World *worldIn, const Pos3D &pos);

private:
    /**
     * @brief Checks if a block with the given ID can support a lily pad block.
     * @param blockId The ID of the block to check.
     * @return True if the block can support a lily pad, false otherwise.
     */
    static bool canSustainBush(int blockId);
};

/**
 * @class WorldGenWaterlily
 * @brief A final class responsible for generating waterlily structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for waterlily generation.
 */
class WorldGenWaterlily final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates waterlilies at the specified position in the world.
     * @param worldIn Pointer to the World object where the waterlilies will be generated.
     * @param rng Reference to the random number generator.
     * @param position The position where the waterlilies should be generated.
     * @return True if the waterlily generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;
};
