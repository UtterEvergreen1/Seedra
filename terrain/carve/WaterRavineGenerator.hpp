#pragma once

#include "AbstractWaterCaveGen.hpp"

/**
 * @class WaterRavineGenerator
 * @brief A final class responsible for generating water ravines in the world.
 * Inherits from AbstractWaterCaveGen.
 */
class WaterRavineGenerator final : public AbstractWaterCaveGen {
public:
    /**
     * @brief Constructs a WaterRavineGenerator object.
     * @param world Reference to the World object.
     */
    explicit WaterRavineGenerator(World &world) : AbstractWaterCaveGen(world) {
    }

    /**
     * @brief Determines the starting chunks for water ravine generation within a given range.
     * @param g Pointer to the Generator object.
     * @param lower The lower bound of the chunk range.
     * @param upper The upper bound of the chunk range.
     * @return A vector of 2D positions representing the starting chunks.
     */
    MU static Pos2DVec_t getStartingChunks(const Generator *g, Pos2D lower, Pos2D upper);

    /**
     * @brief Adds a water ravine feature to the world.
     * @param worldIn Reference to the World object.
     * @param baseChunk The base chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(World &worldIn, Pos2D baseChunk, bool accurate) override;

    /**
     * @brief Adds a tunnel to the water ravine.
     * @param worldIn Reference to the World object.
     * @param randomSeed The random seed for tunnel generation.
     * @param baseChunk The base chunk position.
     * @param tunnel The starting position of the tunnel.
     * @param angle The angle of the tunnel.
     * @param slope The slope of the tunnel.
     * @param curvature The curvature of the tunnel.
     * @param tunnelStartSegment The starting segment of the tunnel.
     * @param tunnelEndSegment The ending segment of the tunnel.
     * @param widthMultiplier The width multiplier for the tunnel.
     */
    void addTunnel(World &worldIn, i64 randomSeed, Pos2D baseChunk,
                   DoublePos3D tunnel, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier);

    /**
     * @brief Adds a water ravine feature to a chunk.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param baseChunk The base chunk position.
     * @param currentChunk The current chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(ChunkPrimer *chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    /**
     * @brief Adds a tunnel to a chunk as part of the water ravine generation.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param randomSeed The random seed for tunnel generation.
     * @param currentChunk The current chunk position.
     * @param tunnel The starting position of the tunnel.
     * @param angle The angle of the tunnel.
     * @param slope The slope of the tunnel.
     * @param curvature The curvature of the tunnel.
     * @param tunnelStartSegment The starting segment of the tunnel.
     * @param tunnelEndSegment The ending segment of the tunnel.
     * @param widthMultiplier The width multiplier for the tunnel.
     */
    void addTunnel(ChunkPrimer *chunkPrimer, i64 randomSeed, Pos2D currentChunk,
                   DoublePos3D tunnel, float angle, float slope, float curvature,
                   int tunnelStartSegment, int tunnelEndSegment, double widthMultiplier);

private:
    /**
     * @brief A vector of random float values used for water ravine generation.
     */
    std::vector<float> rs = std::vector(128, 0.0F);
};
