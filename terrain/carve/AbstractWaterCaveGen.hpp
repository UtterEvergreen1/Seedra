#pragma once

#include "AbstractMapGen.hpp"

/**
 * @class AbstractWaterCaveGen
 * @brief Specialized map generator for creating water caves and water ravines.
 *
 * This class extends the AbstractMapGen class and provides functionality
 * for generating underwater features such as water caves and ravines.
 */
class AbstractWaterCaveGen : public AbstractMapGen {
public:
    /**
     * @brief Constructs an AbstractWaterCaveGen object.
     * @param world Reference to the world object used for generation.
     */
    explicit AbstractWaterCaveGen(World &world) : AbstractMapGen(world) {
    }

    /**
     * @brief Virtual destructor for AbstractWaterCaveGen.
     */
    ~AbstractWaterCaveGen() override;

    /**
     * @brief Retrieves the seed multiplier for water caves and water ravines.
     * @param g Pointer to the generator.
     * @return The seed multiplier as a 2D position.
     */
    MU static Pos2D getSeedMultiplier(const Generator *g);

    /**
     * @brief Sets up the RNG for water caves and water ravines.
     * @param seedMultiplier The seed multiplier.
     * @param chunkPos The position of the chunk.
     */
    MU void setupRNG(Pos2D seedMultiplier, Pos2D chunkPos);

    /**
     * @brief Sets up the RNG for water caves and water ravines.
     * @param g Pointer to the generator.
     * @param rng Reference to the RNG.
     * @param seedMultiplier The seed multiplier.
     * @param chunkPos The position of the chunk.
     */
    MU static void setupRNG(const Generator *g, RNG &rng, Pos2D seedMultiplier, Pos2D chunkPos);

    /**
     * @brief Generates underwater features within the specified target chunk.
     * @param primer Pointer to the chunk primer.
     * @param target The target chunk position.
     * @param accurate Whether to use accurate generation (default is true).
     */
    void generateUnderwater(ChunkPrimer *primer, Pos2D target, bool accurate = true);
};
