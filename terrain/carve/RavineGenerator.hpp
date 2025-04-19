#pragma once

#include "AbstractMapGen.hpp"

/**
 * @class RavineGenerator
 * @brief A final class responsible for generating ravines in the world.
 * Inherits from AbstractMapGen.
 */
class RavineGenerator final : public AbstractMapGen {
public:
    /**
     * @brief Constructs a RavineGenerator object.
     * @param world Reference to the World object.
     */
    explicit RavineGenerator(World &world) : AbstractMapGen(world) {
    }

    /**
     * @brief Default destructor for RavineGenerator.
     */
    ~RavineGenerator() override = default;

    /**
     * @brief Determines the starting chunks for ravine generation within a given range.
     * @param g Pointer to the Generator object.
     * @param lower The lower bound of the chunk range.
     * @param upper The upper bound of the chunk range.
     * @return A vector of 2D positions representing the starting chunks.
     */
    MU static Pos2DVec_t getStartingChunks(const Generator *g, Pos2D lower, Pos2D upper);

    /**
     * @brief Adds a ravine feature to the world.
     * @param worldIn Reference to the World object.
     * @param baseChunk The base chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(World &worldIn, Pos2D baseChunk, bool accurate) override;

    /**
     * @brief Adds a tunnel to the world as part of the ravine generation.
     * @param worldIn Reference to the World object.
     * @param randomSeed The random seed for tunnel generation.
     * @param baseChunk The base chunk position.
     * @param tunnel The starting position of the tunnel.
     * @param angle The angle of the tunnel.
     * @param slope The slope of the tunnel.
     * @param curvature The curvature of the tunnel.
     * @param theStartSegment The starting segment of the tunnel.
     * @param theEndSegment The ending segment of the tunnel.
     * @param theWidthMultiplier The width multiplier for the tunnel.
     * @param accurate Whether to use accurate generation.
     */
    void addTunnel(World &worldIn, i64 randomSeed, Pos2D baseChunk, DoublePos3D tunnel, float angle,
                   float slope, float curvature, int theStartSegment, int theEndSegment, double theWidthMultiplier,
                   bool accurate);

    /**
     * @brief Adds a ravine feature to a chunk.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param baseChunk The base chunk position.
     * @param currentChunk The current chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(ChunkPrimer *chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    /**
     * @brief Adds a tunnel to a chunk as part of the ravine generation.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param randomSeed The random seed for tunnel generation.
     * @param currentChunk The current chunk position.
     * @param tunnel The starting position of the tunnel.
     * @param angle The angle of the tunnel.
     * @param slope The slope of the tunnel.
     * @param curvature The curvature of the tunnel.
     * @param theStartSegment The starting segment of the tunnel.
     * @param theEndSegment The ending segment of the tunnel.
     * @param theWidthMultiplier The width multiplier for the tunnel.
     * @param accurate Whether to use accurate generation.
     */
    void addTunnel(ChunkPrimer *chunkPrimer, i64 randomSeed, Pos2D currentChunk, DoublePos3D tunnel, float angle,
                   float slope, float curvature, int theStartSegment, int theEndSegment, double theWidthMultiplier,
                   bool accurate);

private:
    /**
     * @brief A vector of random float values used for ravine generation.
     */
    std::vector<float> rs = std::vector(128, 0.0F);

    /**
     * @brief Determines the top block at a given position.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The top block as an unsigned char.
     */
    ND unsigned char topBlock(int x, int z) const;

    /**
     * @brief Checks if a block can be replaced during ravine generation.
     * @param blockAt The block at the current position.
     * @param blockAbove The block above the current position.
     * @return True if the block can be replaced, false otherwise.
     */
    static bool canReplaceBlock(u16 blockAt, u16 blockAbove);
};
