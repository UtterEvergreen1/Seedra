#pragma once

#include "ChunkPrimer.hpp"

/**
 * @class World
 * @brief Forward declaration of the World class.
 */
class World;

namespace Chunk {

    /**
     * @brief Flag to enable or disable village generation.
     *
     * This static variable determines whether villages should be generated
     * in the chunk.
     */
    MU static bool GENERATE_VILLAGES = true;

    /**
     * @brief Flag to enable or disable mineshaft generation.
     *
     * This static variable determines whether mineshafts should be generated
     * in the chunk.
     */
    MU static bool GENERATE_MINESHAFTS = true;

    /**
     * @brief Flag to enable or disable stronghold generation.
     *
     * This static variable determines whether strongholds should be generated
     * in the chunk.
     */
    MU static bool GENERATE_STRONGHOLDS = true;

    /**
     * @brief Provides a chunk by populating the given ChunkPrimer.
     *
     * This function generates the terrain and features for a chunk at the
     * specified position using the provided generator.
     *
     * @param chunkPrimer Pointer to the ChunkPrimer to populate.
     * @param g Reference to the Generator used for terrain generation.
     * @param chunkPos The position of the chunk to generate.
     */
    MU extern void provideChunk(ChunkPrimer* chunkPrimer, const Generator &g, const Pos2D& chunkPos);

    /**
     * @brief Creates and provides a new chunk.
     *
     * This function allocates and initializes a new ChunkPrimer for the
     * specified chunk position using the provided generator.
     *
     * @param g Reference to the Generator used for terrain generation.
     * @param chunkPos The position of the chunk to generate.
     * @return A pointer to the newly created ChunkPrimer.
     */
    MU extern ChunkPrimer *provideNewChunk(const Generator &g, const Pos2D& chunkPos);

    /**
     * @brief Populates a chunk with additional features.
     *
     * This function adds features such as structures, decorations, and caves
     * to the specified chunk in the given world.
     *
     * @param world Reference to the World object.
     * @param chunkPos The position of the chunk to populate.
     */
    MU extern void populateChunk(World& world, Pos2D chunkPos);

    /**
     * @brief Populates caves in the specified chunk.
     *
     * This function generates cave systems within the chunk at the given
     * position in the world.
     *
     * @param world Reference to the World object.
     * @param chunkPos The position of the chunk to populate with caves.
     */
    MU extern void populateCaves(World& world, Pos2D chunkPos);

    /**
     * @brief Populates light levels in the specified chunk.
     *
     * This function calculates and applies light levels for the chunk at the
     * given position in the world.
     *
     * @param world Reference to the World object.
     * @param chunkPos The position of the chunk to populate with light levels.
     */
    MU extern void populateLight(World& world, Pos2D chunkPos);

    /**
     * @brief Populates structures in the specified chunk.
     *
     * This function generates structures such as villages, mineshafts, and
     * strongholds within the chunk at the given position in the world.
     *
     * @param world Reference to the World object.
     * @param chunkPos The position of the chunk to populate with structures.
     */
    MU extern void populateStructures(World& world, Pos2D chunkPos);

    /**
     * @brief Populates decorations in the specified chunk.
     *
     * This function adds decorative elements such as trees, flowers, and
     * other vegetation to the chunk at the given position in the world.
     *
     * @param world Reference to the World object.
     * @param chunkPos The position of the chunk to populate with decorations.
     */
    MU extern void populateDecorations(World& world, Pos2D chunkPos);

}