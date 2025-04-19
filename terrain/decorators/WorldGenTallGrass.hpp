#pragma once

#include "WorldGenBush.hpp"

class World;

/**
 * @class BlockTallGrass
 * @brief Represents the tall grass block with different variants and provides utility methods for its behavior.
 * Inherits from BlockBush.
 */
class BlockTallGrass : public BlockBush {
public:
    /**
     * @enum EnumType
     * @brief Enum representing the different types of tall grass.
     */
    enum class EnumType {
        DEAD_BUSH, /**< Represents a dead bush variant. */
        FERN, /**< Represents a fern variant. */
        GRASS, /**< Represents a grass variant. */
        TALL_GRASS /**< Represents a tall grass variant. */
    };

    /**
     * @brief Retrieves the block state corresponding to the given tall grass type.
     * @param type The type of tall grass.
     * @return The block state associated with the specified type.
     */
    static lce::BlockState getBlockVariantById(EnumType type);

    /**
     * @brief Checks if the block can remain at the specified position in the world.
     * @param worldIn Pointer to the World object.
     * @param pos The position to check.
     * @return True if the block can stay, false otherwise.
     */
    static bool canBlockStay(World *worldIn, const Pos3D &pos);
};

/**
 * @class WorldGenTallGrass
 * @brief A final class responsible for generating tall grass structures in the world.
 * Inherits from AbstractWorldGenerator.
 */
class WorldGenTallGrass final : public AbstractWorldGenerator {
public:
    /**
     * @brief Constructs a WorldGenTallGrass object with the specified tall grass type.
     * @param typeIn The type of tall grass to generate.
     */
    explicit WorldGenTallGrass(const BlockTallGrass::EnumType typeIn) : type(typeIn) {
    }

    /**
     * @brief Generates tall grass at the specified position in the world.
     * @param worldIn Pointer to the World object where the tall grass will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the tall grass should be generated.
     * @return True if the generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    BlockTallGrass::EnumType type; /**< The type of tall grass to generate. */
};
