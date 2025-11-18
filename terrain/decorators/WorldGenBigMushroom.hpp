#pragma once

#include "AbstractWorldGenerator.hpp"
#include "lce/blocks/blocksInit.hpp"

class World;

/**
 * @class BlockHugeMushroom
 * @brief Represents the different types of huge mushroom blocks.
 */
class BlockHugeMushroom {
public:
    /**
     * @enum EnumType
     * @brief Enum representing the various parts of a huge mushroom.
     */
    enum class EnumType {
        ALL_INSIDE, /**< All inside blocks of the mushroom. */
        NORTH_WEST, /**< North-west corner block of the mushroom. */
        NORTH, /**< North side block of the mushroom. */
        NORTH_EAST, /**< North-east corner block of the mushroom. */
        WEST, /**< West side block of the mushroom. */
        CENTER, /**< Center block of the mushroom. */
        EAST, /**< East side block of the mushroom. */
        SOUTH_WEST, /**< South-west corner block of the mushroom. */
        SOUTH, /**< South side block of the mushroom. */
        SOUTH_EAST, /**< South-east corner block of the mushroom. */
        STEM, /**< Stem block of the mushroom. */
        ALL_OUTSIDE = 14, /**< All outside blocks of the mushroom. */
        ALL_STEM /**< All stem blocks of the mushroom. */
    };
};

/**
 * @class WorldGenBigMushroom
 * @brief A final class responsible for generating big mushrooms in the world.
 * Inherits from AbstractWorldGenerator.
 */
class WorldGenBigMushroom final : public AbstractWorldGenerator {
public:
    /**
     * @brief Constructs a WorldGenBigMushroom object with a specified mushroom type.
     * @param mushroomType The type of mushroom block to generate. Defaults to AIR.
     */
    explicit WorldGenBigMushroom(const lce::BlockState mushroomType = lce::BlocksInit::AIR.getState())
        : m_mushroomType(mushroomType) {
    }

    /**
     * @brief Generates a big mushroom at the specified position in the world.
     * @param worldIn Pointer to the World object where the mushroom will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the mushroom should be generated.
     * @return True if the generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;

private:
    lce::BlockState m_mushroomType; /**< The type of mushroom block to generate. */
};
