#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockDoublePlant
 * @brief Represents a double plant block and provides utility methods for placement and type management.
 */
class BlockDoublePlant {
public:
    /**
     * @enum EnumPlantType
     * @brief Enum representing the different types of double plants.
     */
    enum class EnumPlantType : u8 {
        SUNFLOWER, /**< Represents a sunflower plant. */
        SYRINGA, /**< Represents a syringa plant. */
        GRASS, /**< Represents a grass plant. */
        FERN, /**< Represents a fern plant. */
        ROSE, /**< Represents a rose plant. */
        PAEONIA /**< Represents a paeonia plant. */
    };

    /**
     * @brief Checks if the double plant block can be placed at the specified position in the world.
     * @param chunk Pointer to the World object.
     * @param pos The position to check for block placement.
     * @return True if the block can be placed, false otherwise.
     */
    static bool canPlaceBlockAt(World *chunk, const Pos3D &pos);

    /**
     * @brief Places the double plant block at the specified position with the given variant.
     * @param chunk Pointer to the World object where the block will be placed.
     * @param lowerPos The lower position of the double plant.
     * @param variant The type of double plant to place.
     */
    static void placeAt(World *chunk, const Pos3D &lowerPos, EnumPlantType variant);
};

/**
 * @class WorldGenDoublePlant
 * @brief A final class responsible for generating double plant structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for double plant generation.
 */
class WorldGenDoublePlant final : public AbstractWorldGenerator {
    mutable BlockDoublePlant::EnumPlantType plantType = BlockDoublePlant::EnumPlantType::SUNFLOWER;
    /**< The type of double plant to generate. */

public:
    /**
     * @brief Sets the type of double plant to generate.
     * @param type The type of double plant to set.
     */
    void setPlantType(const BlockDoublePlant::EnumPlantType type) const {
        this->plantType = type;
    }

    /**
     * @brief Generates a double plant at the specified position in the world.
     * @param worldIn Pointer to the World object where the double plant will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the double plant should be generated.
     * @return True if the double plant generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
