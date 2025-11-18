#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenLiquids
 * @brief A final class responsible for generating liquid structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for liquid generation.
 */
class WorldGenLiquids final : public AbstractWorldGenerator {
    u16 m_blockId; /**< The ID of the liquid block to generate. */

public:
    /**
     * @brief Constructs a WorldGenLiquids object with the specified block ID.
     * @param blockId The ID of the liquid block to generate.
     */
    explicit WorldGenLiquids(const u16 blockId) : m_blockId(blockId) {
    }

    /**
     * @brief Generates a liquid structure at the specified position in the world.
     * @param worldIn Pointer to the World object where the liquid will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the liquid should be generated.
     * @return True if the liquid generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;
};
