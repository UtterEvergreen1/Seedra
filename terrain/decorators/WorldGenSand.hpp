#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenSand
 * @brief A final class responsible for generating sand structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for sand generation.
 */
class WorldGenSand final : public AbstractWorldGenerator {
    const lce::BlockState block; /**< The block state representing the type of sand to generate. */
    int radius; /**< The radius of the sand structure to generate. */

public:
    /**
     * @brief Constructs a WorldGenSand object with the specified block state and radius.
     * @param block The block state representing the type of sand to generate.
     * @param radiusIn The radius of the sand structure to generate.
     */
    explicit WorldGenSand(const lce::BlockState block, const int radiusIn) : block(block), radius(radiusIn) {
    }

    /**
     * @brief Generates a sand structure at the specified position in the world.
     * @param worldIn Pointer to the World object where the sand will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the sand structure should be generated.
     * @return True if the sand generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;
};
