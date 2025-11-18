#pragma once

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

/**
 * @class AbstractMapGen
 * @brief Base class for map generation, providing utilities for generating features like caves and ravines.
 */
class AbstractMapGen {
protected:
    /**
     * @brief The range of chunks to process during generation.
     */
    static constexpr int CHUNK_RANGE = 8;

    /**
     * @brief Multiplier used to reserve memory for generation.
     */
    static constexpr float RESERVE_MULTIPLIER = 1.4F;

public:
    /**
     * @brief Reference to the world object used for generation.
     */
    World &m_world;

    /**
     * @brief Pointer to the generator associated with the world.
     */
    Generator *m_g;

    /**
     * @brief Random number generator used for feature generation.
     */
    RNG m_rng;

    /**
     * @brief Bounding box defining the generation area.
     */
    BoundingBox m_genBounds = BoundingBox::EMPTY;

    /**
     * @brief Constructs an AbstractMapGen object.
     * @param world Reference to the world object.
     */
    explicit AbstractMapGen(World &world) : m_world(world), m_g(world.getGenerator()) {
    }

    /**
     * @brief Virtual destructor for AbstractMapGen.
     */
    virtual ~AbstractMapGen();

    /**
     * @brief Retrieves the seed multiplier used for caves and ravines.
     * @param g Pointer to the generator.
     * @return The seed multiplier as a 2D position template.
     */
    MU static Pos2DTemplate<i64> getSeedMultiplier(const Generator *g);

    /**
     * @brief Sets up the RNG for caves and ravines.
     * @param seedMultiplier The seed multiplier.
     * @param chunkPos The position of the chunk.
     */
    MU void setupRNG(Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos);

    /**
     * @brief Sets up the RNG for water caves and water ravines.
     * @param g Pointer to the generator.
     * @param rng Reference to the RNG.
     * @param seedMultiplier The seed multiplier.
     * @param chunkPos The position of the chunk.
     */
    MU static void setupRNG(const Generator *g, RNG &rng, Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos);

    /**
     * @brief Sets the generation bounds.
     * @param bounds The bounding box to set.
     */
    void setGenBounds(const BoundingBox &bounds) {
        m_genBounds = bounds;
    }

    /**
     * @brief Generates features within the specified target chunk.
     * @param primer Pointer to the chunk primer.
     * @param target The target chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void generate(ChunkPrimer *primer, Pos2D target, bool accurate = true) {
        Pos2DTemplate<i64> seedMultiplier = getSeedMultiplier(m_world.getGenerator());

        const Pos2D lower = target - CHUNK_RANGE;
        const Pos2D upper = target + CHUNK_RANGE;
        Pos2D chunkPos;
        for (chunkPos.x = lower.x; chunkPos.x <= upper.x; ++chunkPos.x) {
            for (chunkPos.z = lower.z; chunkPos.z <= upper.z; ++chunkPos.z) {
                setupRNG(seedMultiplier, chunkPos);
                addFeature(primer, chunkPos, target, accurate);
            }
        }
    }

    /**
     * @brief Adds a feature to the world at the specified chunk position.
     * @param worldIn Reference to the world object.
     * @param baseChunk The base chunk position.
     * @param accurate Whether to use accurate generation.
     */
    virtual void addFeature(World &worldIn, Pos2D baseChunk, bool accurate) = 0;

    /**
     * @brief Adds a feature to the specified chunk.
     * @param chunkPrimer Pointer to the chunk primer.
     * @param baseChunk The base chunk position.
     * @param currentChunk The current chunk position.
     * @param accurate Whether to use accurate generation.
     */
    virtual void addFeature(ChunkPrimer *chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) = 0;
};
