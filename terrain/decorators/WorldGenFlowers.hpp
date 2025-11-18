#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class BlockFlower
 * @brief Represents a flower block and provides an enumeration of flower types.
 */
class BlockFlower {
public:
    /**
     * @enum EnumFlowerType
     * @brief Enum representing the different types of flowers.
     */
    enum class EnumFlowerType : int8_t {
        DANDELION, /**< Represents a dandelion flower. */
        POPPY, /**< Represents a poppy flower. */
        BLUE_ORCHID, /**< Represents a blue orchid flower. */
        ALLIUM, /**< Represents an allium flower. */
        AZURE_BLUET, /**< Represents an azure bluet flower. */
        RED_TULIP, /**< Represents a red tulip flower. */
        ORANGE_TULIP, /**< Represents an orange tulip flower. */
        WHITE_TULIP, /**< Represents a white tulip flower. */
        PINK_TULIP, /**< Represents a pink tulip flower. */
        OXEYE_DAISY, /**< Represents an oxeye daisy flower. */
        NUM_TYPES /**< Represents the total number of flower types. */
    };
};

/**
 * @class WorldGenFlowers
 * @brief A final class responsible for generating flower structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for flower generation.
 */
class WorldGenFlowers final : public AbstractWorldGenerator {
    int m_flowerId; /**< The ID of the flower block to generate. */
    BlockFlower::EnumFlowerType m_flowerType; /**< The type of flower to generate. */

public:
    /**
     * @brief Constructs a WorldGenFlowers object.
     * @param flowerId The ID of the flower block to generate.
     * @param type The type of flower to generate.
     */
    WorldGenFlowers(const int flowerId, const BlockFlower::EnumFlowerType type)
        : m_flowerId(flowerId), m_flowerType(type) {
    }

    /**
     * @brief Sets the flower block ID and type to be generated.
     * @param flowerId The ID of the flower block to generate.
     * @param typeIn The type of flower to generate.
     */
    void setGeneratedBlock(const int flowerId, const BlockFlower::EnumFlowerType typeIn) {
        this->m_flowerId = flowerId;
        this->m_flowerType = typeIn;
    }

    /**
     * @brief Generates a flower at the specified position in the world.
     * @param worldIn Pointer to the World object where the flower will be generated.
     * @param rng Reference to the random number generator.
     * @param position The position where the flower should be generated.
     * @return True if the flower generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;
};
