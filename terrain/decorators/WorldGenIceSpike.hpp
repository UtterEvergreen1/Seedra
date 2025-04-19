#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenIceSpike
 * @brief A final class responsible for generating ice spike structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for ice spike generation.
 */
class WorldGenIceSpike final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates an ice spike at the specified position in the world.
     * @param worldIn Pointer to the World object where the ice spike will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the ice spike should be generated.
     * @return True if the ice spike generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
