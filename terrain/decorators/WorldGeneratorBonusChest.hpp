#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGeneratorBonusChest
 * @brief A final class responsible for generating bonus chests in the world.
 * Inherits from AbstractWorldGenerator and provides methods for bonus chest generation.
 */
class WorldGeneratorBonusChest final : public AbstractWorldGenerator {
public:
    /**
     * @brief Generates a bonus chest at the specified position in the world.
     * @param worldIn Pointer to the World object where the bonus chest will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the bonus chest should be generated.
     * @return True if the bonus chest generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
