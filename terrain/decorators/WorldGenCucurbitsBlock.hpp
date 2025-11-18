#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenCucurbitsBlock
 * @brief A final class responsible for generating cucurbit blocks in the world.
 * Inherits from AbstractWorldGenerator and provides methods for cucurbit block generation.
 */
class WorldGenCucurbitsBlock final : public AbstractWorldGenerator {
public:
    u16 m_blockId; /**< The ID of the block to generate. */
    bool m_hasRotation; /**< Indicates whether the block has rotation metadata. */

    /**
     * @brief Constructs a WorldGenCucurbitsBlock object.
     * @param block The ID of the block to generate.
     * @param has_rotation Optional parameter indicating if the block has rotation metadata. Defaults to false.
     */
    explicit WorldGenCucurbitsBlock(c_u16 block, const bool has_rotation = false)
        : m_blockId(block), m_hasRotation(has_rotation) {
    }

    /**
     * @brief Generates a cucurbit block at the specified position in the world.
     * @param worldIn Pointer to the World object where the block will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the block should be generated.
     * @return True if the block generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    /**
     * @brief Generates a random rotation metadata value for the block.
     * @param rng Reference to the random number generator.
     * @return An integer representing the rotation metadata.
     */
    static u8 getRandomRotationMetaData(RNG &rng);
};
