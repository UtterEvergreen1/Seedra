#pragma once

#include "AbstractWaterCaveGen.hpp"

/**
 * @class WaterCaveGenerator
 * @brief A final class responsible for generating water caves in the world.
 * Inherits from AbstractWaterCaveGen.
 */
class WaterCaveGenerator final : public AbstractWaterCaveGen {
public:
    /**
     * @brief Constructs a WaterCaveGenerator object.
     * @param world Reference to the World object.
     */
    explicit WaterCaveGenerator(World &world) : AbstractWaterCaveGen(world) {
    }

    /**
     * @brief Determines the starting chunks for water cave generation within a given range.
     * @param g Pointer to the Generator object.
     * @param lower The lower bound of the chunk range.
     * @param upper The upper bound of the chunk range.
     * @return A vector of 2D positions representing the starting chunks.
     */
    MU static Pos2DVec_t getStartingChunks(const Generator *g, Pos2D lower, Pos2D upper);

    /**
     * @brief Adds a water cave feature to the world.
     * @param worldIn Reference to the World object.
     * @param baseChunk The base chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(World &worldIn, Pos2D baseChunk, bool accurate) override;

    /**
     * @brief Adds a room to the water cave.
     * @param worldIn Reference to the World object.
     * @param seedModifier The seed modifier for room generation.
     * @param baseChunk The base chunk position.
     * @param roomStart The starting position of the room.
     * @param rng Reference to the random number generator.
     */
    void addRoom(World &worldIn, c_i64 seedModifier, Pos2D baseChunk, const DoublePos3D &roomStart, RNG &rng);

    /**
     * @brief Adds a tunnel to the water cave.
     * @param worldIn Reference to the World object.
     * @param seedModifier The seed modifier for tunnel generation.
     * @param baseChunk The base chunk position.
     * @param start The starting position of the tunnel.
     * @param theTunnelWidth The width of the tunnel.
     * @param theTunnelDirection The direction of the tunnel.
     * @param theTunnelSlope The slope of the tunnel.
     * @param theCurrentSegment The current segment of the tunnel.
     * @param theMaxSegment The maximum number of segments for the tunnel.
     * @param theHeightMultiplier The height multiplier for the tunnel.
     */
    void addTunnel(World &worldIn, c_i64 seedModifier, Pos2D baseChunk, DoublePos3D start, c_float theTunnelWidth,
                   float theTunnelDirection, float theTunnelSlope, int theCurrentSegment, int theMaxSegment,
                   c_double theHeightMultiplier);

    /**
     * @brief Checks if a block can be replaced during water cave generation.
     * @param blockAt The block at the current position.
     * @return True if the block can be replaced, false otherwise.
     */
    static bool canReplaceBlock(u16 blockAt);

    /**
     * @brief Adds a water cave feature to a chunk.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param baseChunk The base chunk position.
     * @param currentChunk The current chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(ChunkPrimer *chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    /**
     * @brief Adds a room to a chunk as part of the water cave generation.
     * @param seedModifier The seed modifier for room generation.
     * @param target The target chunk position.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param roomStart The starting position of the room.
     * @param rng Reference to the random number generator.
     */
    void addRoom(i64 seedModifier, Pos2D target, ChunkPrimer *chunkPrimer, const DoublePos3D &roomStart, RNG &rng);

    /**
     * @brief Adds a tunnel to a chunk as part of the water cave generation.
     * @param chunkPrimer Pointer to the ChunkPrimer object.
     * @param seedModifier The seed modifier for tunnel generation.
     * @param chunk The current chunk position.
     * @param start The starting position of the tunnel.
     * @param theTunnelWidth The width of the tunnel.
     * @param tunnelDirection The direction of the tunnel.
     * @param theTunnelSlope The slope of the tunnel.
     * @param currentTunnelSegment The current segment of the tunnel.
     * @param maxTunnelSegment The maximum number of segments for the tunnel.
     * @param theHeightMultiplier The height multiplier for the tunnel.
     */
    void addTunnel(ChunkPrimer *chunkPrimer, c_i64 seedModifier, Pos2D chunk, DoublePos3D start, c_float theTunnelWidth,
                   float tunnelDirection, float theTunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   c_double theHeightMultiplier);
};
