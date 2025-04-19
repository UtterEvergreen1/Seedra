#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenIcePath
 * @brief A final class responsible for generating ice path structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for ice path generation.
 */
class WorldGenIcePath final : public AbstractWorldGenerator {
public:
    /**
     * @brief Constructs a WorldGenIcePath object with the specified path width.
     * @param path_width The width of the ice path to generate.
     */
    explicit WorldGenIcePath(const int path_width)
        : pathWidth(path_width) {
    }

    /**
     * @brief Generates an ice path at the specified position in the world.
     * @param worldIn Pointer to the World object where the ice path will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the ice path should be generated.
     * @return True if the ice path generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    static const lce::BlockState PATH_BLOCK; /**< The block state representing the ice path. */
    const int pathWidth; /**< The width of the ice path to generate. */
};
