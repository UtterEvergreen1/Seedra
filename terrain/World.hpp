#pragma once

#include <unordered_map>
#include <atomic>
#include <mutex>

#include "Chunk.hpp"
#include "structures/gen/village/village.hpp"
#include "structures/gen/mineshaft/mineshaft.hpp"
#include "structures/gen/stronghold/stronghold.hpp"

#include "common/Pos2DTemplate.hpp"
#include "common/Pos3DTemplate.hpp"

class Biome;

/**
 * @class World
 * @brief Represents the world and provides methods for managing chunks, biomes, and structures.
 */
class World {
public:
    /**
     * @brief Constructs a World object with the specified generator.
     * @param g Pointer to the Generator used for world generation.
     */
    explicit World(Generator *g);

    /**
     * @brief Destructor for the World class.
     */
    ~World();

    /**
     * @brief Deletes the world and cleans up resources.
     */
    void deleteWorld();

    /**
     * @brief Retrieves the generator associated with the world.
     * @return Pointer to the Generator.
     */
    Generator *getGenerator() const;

    /**
     * @brief Adds a chunk to the world at the specified position.
     * @param pos The position of the chunk.
     * @param chunk Pointer to the ChunkPrimer to add.
     */
    void addChunk(const Pos2D &pos, ChunkPrimer *chunk);

    /**
     * @brief Checks if a chunk exists at the specified position.
     * @param pos The position of the chunk.
     * @return True if the chunk exists, false otherwise.
     */
    bool chunkExists(const Pos2D &pos) const;

    /**
     * @brief Retrieves the chunk at the specified position.
     * @param pos The position of the chunk.
     * @return Pointer to the ChunkPrimer.
     */
    ChunkPrimer *getChunk(const Pos2D &pos);

    /**
     * @brief Retrieves or creates a chunk at the specified position.
     * @param chunkPos The position of the chunk.
     * @return Pointer to the ChunkPrimer.
     */
    ChunkPrimer *getOrCreateChunk(const Pos2D &chunkPos);

    /**
     * @brief Creates chunks around the specified position within a given radius.
     * @param pos The center position.
     * @param radius The radius around the position.
     */
    void createChunks(const Pos2D &pos, int radius);

    /**
     * @brief Decorates caves in the specified area.
     * @param theStartPosition The starting position.
     * @param radius The radius around the starting position.
     * @param hasWaterCaves Whether the caves contain water.
     */
    void decorateCaves(const Pos2D &theStartPosition, int radius, bool hasWaterCaves);

    /**
     * @brief Decorates chunks around the specified position within a given radius.
     * @param pos The center position.
     * @param radius The radius around the position.
     */
    void decorateChunks(const Pos2D &pos, int radius);

    /**
     * @brief Generates biomes for the world.
     */
    void generateWorldBiomes();

    /**
     * @brief Retrieves the world biomes.
     * @return Pointer to the array of world biomes.
     */
    int *getWorldBiomes() const;

    /**
     * @brief Retrieves the biomes for a specific chunk.
     * @param pos The position of the chunk.
     * @return Pointer to the array of chunk biomes.
     */
    int *getChunkBiomes(const Pos2D &pos) const;

    /**
     * @brief Retrieves the biome at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return Pointer to the Biome.
     */
    Biome *getBiomeAt(int x, int z) const;

    /**
     * @brief Retrieves the biome ID at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The biome ID.
     */
    int getBiomeIdAt(int x, int z) const;

    /**
     * @brief Retrieves all chunks in the world.
     * @return Reference to the unordered map of chunks.
     */
    auto &getChunks() const;

    /**
     * @brief Locks the chunk mutex to prevent concurrent access.
     */
    void lockChunks();

    /**
     * @brief Unlocks the chunk mutex.
     */
    void unlockChunks();

    /**
     * @brief Retrieves the block ID at the specified coordinates.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     * @return The block ID.
     */
    int getBlockId(int x, int y, int z);

    /**
     * @brief Retrieves the block ID at the specified position.
     * @param pos The position.
     * @return The block ID.
     */
    int getBlockId(const Pos3D &pos);

    /**
     * @brief Retrieves the block state at the specified coordinates.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     * @return The block state.
     */
    lce::BlockState getBlock(int x, int y, int z);

    /**
     * @brief Retrieves the block state at the specified position.
     * @param pos The position.
     * @return The block state.
     */
    lce::BlockState getBlock(const Pos3D &pos);

    /**
     * @brief Notifies neighboring blocks of a change at the specified coordinates.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     */
    void notifyNeighbors(int x, int y, int z);

    /**
     * @brief Sets the block ID at the specified coordinates.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     * @param blockId The block ID to set.
     */
    void setBlock(int x, int y, int z, int blockId);

    /**
     * @brief Sets the block ID at the specified position.
     * @param pos The position.
     * @param blockId The block ID to set.
     */
    void setBlockId(const Pos3D &pos, int blockId);

    /**
     * @brief Sets the block ID and metadata at the specified coordinates.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     * @param blockId The block ID to set.
     * @param meta The metadata to set.
     */
    void setBlock(int x, int y, int z, int blockId, int meta);

    /**
     * @brief Sets the block ID and metadata at the specified position.
     * @param pos The position.
     * @param blockId The block ID to set.
     * @param meta The metadata to set.
     */
    void setBlock(const Pos3D &pos, int blockId, int meta);

    /**
     * @brief Sets the block state at the specified coordinates.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     * @param blockstate The block state to set.
     */
    void setBlock(int x, int y, int z, lce::BlockState blockstate);

    /**
     * @brief Sets the block state at the specified position.
     * @param pos The position.
     * @param blockstate The block state to set.
     */
    void setBlock(const Pos3D &pos, lce::BlockState blockstate);

    /**
     * @brief Checks if the block at the specified coordinates is air.
     * @param x The X-coordinate.
     * @param y The Y-coordinate.
     * @param z The Z-coordinate.
     * @return True if the block is air, false otherwise.
     */
    bool isAirBlock(int x, int y, int z);

    /**
     * @brief Checks if the block at the specified position is air.
     * @param pos The position.
     * @return True if the block is air, false otherwise.
     */
    bool isAirBlock(const Pos3D &pos);

    /**
     * @brief Retrieves the height of the terrain at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The height of the terrain.
     */
    int getHeight(int x, int z);

    /**
     * @brief Retrieves the height of the terrain at the specified position.
     * @param pos The position.
     * @return The height of the terrain.
     */
    Pos3D getHeight(const Pos3D &pos);

    /**
     * @brief Retrieves the top solid or liquid block at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The Y-coordinate of the top solid or liquid block.
     */
    int getTopSolidOrLiquidBlock(int x, int z);

    /**
     * @brief Retrieves the top solid or liquid block at the specified position.
     * @param pos The position.
     * @return The position of the top solid or liquid block.
     */
    Pos3D getTopSolidOrLiquidBlock(const Pos3D &pos);

    /**
     * @brief Checks if the specified position is snowy.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return True if the position is snowy, false otherwise.
     */
    ND bool isSnowyAt(int x, int z) const;

    /**
     * @brief Checks if the specified position has an ideal temperature.
     * @param pos The position.
     * @return True if the temperature is ideal, false otherwise.
     */
    ND bool hasIdealTemperature(const Pos3D &pos) const;

    /**
     * @brief Retrieves the precipitation height at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The precipitation height.
     */
    int getPrecipitationHeight(c_int x, c_int z);

    /**
     * @brief Checks if the block at the specified position can freeze.
     * @param pos The position.
     * @param noWaterAdj Whether to ignore adjacent water blocks.
     * @return True if the block can freeze, false otherwise.
     */
    bool canBlockFreeze(const Pos3D &pos, bool noWaterAdj);

    /**
     * @brief Checks if water at the specified position can freeze.
     * @param pos The position.
     * @return True if the water can freeze, false otherwise.
     */
    ND bool canBlockFreezeWater(const Pos3D &pos);

    /**
     * @brief Checks if snow can accumulate at the specified position.
     * @param pos The position.
     * @param checkLight Whether to check light levels.
     * @return True if snow can accumulate, false otherwise.
     */
    bool canSnowAt(const Pos3D &pos, bool checkLight);

    /**
     * @brief Retrieves the sea level of the world.
     * @return The sea level.
     */
    static int getSeaLevel();

    /**
     * @brief Generates mineshafts in the world.
     */
    void generateMineshafts();

    /**
     * @brief Generates villages in the world.
     */
    void generateVillages();

    /**
     * @brief Generates strongholds in the world.
     */
    void generateStrongholds();

    std::vector<gen::Village> villages; ///< List of generated villages.
    std::vector<gen::Mineshaft> mineshafts; ///< List of generated mineshafts.
    std::vector<gen::Stronghold> strongholds; ///< List of generated strongholds.
    std::unordered_map<Pos2D, ChunkPrimer *, Pos2D::Hasher> chunks; ///< Map of chunks in the world.
    BoundingBox worldBounds; ///< Bounding box of the world.
    std::mutex chunkMutex; ///< Mutex for synchronizing chunk access.

private:
    int *biomes = nullptr; ///< Pointer to the array of world biomes.
    std::atomic<ChunkPrimer *> lastChunk = nullptr; ///< Pointer to the last accessed chunk.
    std::atomic<Pos2D> lastChunkCoords = Pos2D(-100000, -100000); ///< Coordinates of the last accessed chunk.

    Generator *g; ///< Pointer to the Generator used for world generation.
};

#include "World.inl"
