#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockCactus
 * @brief Represents a cactus block and provides utility methods for stability checks.
 */
class BlockCactus {
public:
    /**
     * @brief Checks if the cactus block can remain at the specified position in the world.
     * @param worldIn Pointer to the World object.
     * @param pos The position to check for block stability.
     * @return True if the block can stay, false otherwise.
     */
    static bool canBlockStay(World *worldIn, const Pos3D &pos);
};

/**
 * @class WorldGenCactus
 * @brief A final class responsible for generating cactus structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for cactus generation.
 */
class WorldGenCactus final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates a cactus at the specified position in the world.
     * @param worldIn Pointer to the World object where the cactus will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the cactus should be generated.
     * @return True if the cactus generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
