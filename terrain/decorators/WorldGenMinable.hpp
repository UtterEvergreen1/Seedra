// Seedra/terrain/decorators/WorldGenMinable.hpp
#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenMinable
 * @brief A final class responsible for generating ore veins in the world.
 * Inherits from AbstractWorldGenerator and provides methods for ore vein generation.
 */
class WorldGenMinable final : public AbstractWorldGenerator {
    const lce::BlockState m_oreBlock; /**< The block state representing the type of ore to generate. */
    int m_blockCount; /**< The number of blocks in the ore vein. */

public:
    /**
     * @brief Constructs a WorldGenMinable object with the specified ore block and block count.
     * @param block The block state representing the type of ore to generate.
     * @param blockCount The number of blocks in the ore vein.
     */
    WorldGenMinable(lce::BlockState block, int blockCount);

    /**
     * @brief Generates an ore vein at the specified position in the world.
     * @param world Pointer to the World object where the ore vein will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the ore vein should be generated.
     * @return True if the ore vein generation was successful, false otherwise.
     */
    bool generate(World *world, RNG &rng, const Pos3D &pos) const override;
};
