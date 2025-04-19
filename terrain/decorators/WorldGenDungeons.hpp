#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenDungeons
 * @brief A final class responsible for generating dungeon structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for dungeon generation.
 */
class WorldGenDungeons final : public AbstractWorldGenerator {
    static std::string SPAWNER_TYPES[]; /**< Array of possible mob spawner types. */

    /**
     * @brief Picks a random mob spawner type from the available options.
     * @param rng Reference to the random number generator.
     */
    static void pickMobSpawner(RNG &rng);

public:
    /**
     * @brief Generates a dungeon at the specified position in the world.
     * @param worldIn Pointer to the World object where the dungeon will be generated.
     * @param rng Reference to the random number generator.
     * @param position The position where the dungeon should be generated.
     * @return True if the dungeon generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;
};
