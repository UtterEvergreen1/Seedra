#pragma once

#include "AbstractMapGen.hpp"
#include <set>

/**
 * @class CaveGenerator
 * @brief Final class for generating cave systems in the world.
 *
 * This class extends AbstractMapGen and provides methods for generating
 * tunnels and rooms within a cave system. It operates on both the world
 * and individual chunk levels.
 */
class CaveGenerator final : public AbstractMapGen {
public:
    /**
     * @brief Constructs a CaveGenerator object.
     * @param world Reference to the world object used for generation.
     */
    explicit CaveGenerator(World &world) : AbstractMapGen(world) {
    }

    /**
     * @brief Virtual destructor for CaveGenerator.
     */
    ~CaveGenerator() override = default;

    /**
     * @brief Retrieves the starting chunks for cave generation within a given range.
     * @param g Pointer to the generator.
     * @param lower The lower bound of the range.
     * @param upper The upper bound of the range.
     * @return A vector of starting chunk positions.
     */
    MU static Pos2DVec_t getStartingChunks(const Generator *g, Pos2D lower, Pos2D upper);

    /**
     * @brief Adds a cave feature to the world at the specified chunk position.
     * @param worldIn Reference to the world object.
     * @param baseChunk The base chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(World &worldIn, Pos2D baseChunk, bool accurate) override;

    /**
     * @brief Adds a tunnel to the world.
     * @param worldIn Reference to the world object.
     * @param theSeedModifier The seed modifier for the tunnel.
     * @param currentChunk The current chunk position.
     * @param start The starting position of the tunnel.
     * @param theWidth The width of the tunnel.
     * @param theDirection The direction of the tunnel.
     * @param theSlope The slope of the tunnel.
     * @param theCurrentSegment The current segment of the tunnel.
     * @param theMaxSegment The maximum number of segments for the tunnel.
     * @param theHeightMultiplier The height multiplier for the tunnel.
     * @param accurate Whether to use accurate generation.
     */
    void addTunnel(World &worldIn, i64 theSeedModifier, Pos2D currentChunk, DoublePos3D start, float theWidth,
                   float theDirection, float theSlope, int theCurrentSegment, int theMaxSegment,
                   double theHeightMultiplier, bool accurate);

    /**
     * @brief Adds a room to the world.
     * @param worldIn Reference to the world object.
     * @param seedModifier The seed modifier for the room.
     * @param currentChunk The current chunk position.
     * @param roomStart The starting position of the room.
     * @param rng Reference to the random number generator.
     * @param accurate Whether to use accurate generation.
     */
    void addRoom(World &worldIn, i64 seedModifier, Pos2D currentChunk, const DoublePos3D &roomStart, RNG &rng,
                 bool accurate);

    /**
     * @brief Adds a cave feature to the specified chunk.
     * @param chunkPrimer Pointer to the chunk primer.
     * @param baseChunk The base chunk position.
     * @param currentChunk The current chunk position.
     * @param accurate Whether to use accurate generation.
     */
    void addFeature(ChunkPrimer *chunkPrimer, Pos2D baseChunk, Pos2D currentChunk, bool accurate) override;

    /**
     * @brief Adds a tunnel to the specified chunk.
     * @param chunkPrimer Pointer to the chunk primer.
     * @param theSeedModifier The seed modifier for the tunnel.
     * @param currentChunk The current chunk position.
     * @param startPos The starting position of the tunnel.
     * @param theWidth The width of the tunnel.
     * @param theDirection The direction of the tunnel.
     * @param theSlope The slope of the tunnel.
     * @param theCurrentSegment The current segment of the tunnel.
     * @param theMaxSegment The maximum number of segments for the tunnel.
     * @param theHeightMultiplier The height multiplier for the tunnel.
     * @param accurate Whether to use accurate generation.
     */
    void addTunnel(ChunkPrimer *chunkPrimer, i64 theSeedModifier, Pos2D currentChunk, DoublePos3D startPos,
                   float theWidth,
                   float theDirection, float theSlope, int theCurrentSegment, int theMaxSegment,
                   double theHeightMultiplier, bool accurate);

    /**
     * @brief Adds a room to the specified chunk.
     * @param chunkPrimer Pointer to the chunk primer.
     * @param seedModifier The seed modifier for the room.
     * @param currentChunk The current chunk position.
     * @param roomStart The starting position of the room.
     * @param rng Reference to the random number generator.
     * @param accurate Whether to use accurate generation.
     */
    void addRoom(ChunkPrimer *chunkPrimer, i64 seedModifier, Pos2D currentChunk, const DoublePos3D &roomStart, RNG &rng,
                 bool accurate);

private:
    /**
     * @brief Retrieves the top block at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The top block as an unsigned char.
     */
    ND unsigned char topBlock(int x, int z) const;

    /**
     * @brief Checks if a block can be replaced during cave generation.
     * @param blockAt The block ID at the current position.
     * @param blockAbove The block ID above the current position.
     * @return True if the block can be replaced, false otherwise.
     */
    static bool canReplaceBlock(u16 blockAt, u16 blockAbove);
};
