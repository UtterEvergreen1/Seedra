#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenClay
 * @brief A final class responsible for generating clay structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for clay generation.
 */
class WorldGenClay final : public AbstractWorldGenerator {
    int numberOfBlocks; /**< The number of clay blocks to generate. */

public:
    /**
     * @brief Constructs a WorldGenClay object with the specified number of blocks.
     * @param numBlocks The number of clay blocks to generate.
     */
    explicit WorldGenClay(const int numBlocks) : numberOfBlocks(numBlocks) {
    }

    /**
     * @brief Generates a clay structure at the specified position in the world.
     * @param worldIn Pointer to the World object where the clay will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the clay structure should be generated.
     * @return True if the clay generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;
};
