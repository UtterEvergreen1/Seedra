#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockReed
 * @brief A utility class for handling reed block behavior in the world.
 */
class BlockReed {
public:
    /**
     * @brief Checks if a reed block can remain at the specified position in the world.
     * @param worldIn Pointer to the World object where the check is performed.
     * @param pos The position to check for reed block stability.
     * @return True if the reed block can stay at the specified position, false otherwise.
     */
    static bool canBlockStay(World *worldIn, const Pos3D &pos);
};

/**
 * @class WorldGenReed
 * @brief A final class responsible for generating reed structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for reed generation.
 */
class WorldGenReed final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates reed structures at the specified position in the world.
     * @param worldIn Pointer to the World object where the reeds will be generated.
     * @param rng Reference to the random number generator.
     * @param position The position where the reeds should be generated.
     * @return True if the reed generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;
};
