#pragma once

#include <atomic>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

#include "generator.hpp"
#include "BlockAccess.hpp"
#include "lce/blocks/__include.hpp"
#include "lce/registry/blockRegistry.hpp"
#include "components/BoundingBox.hpp"
#include "common/Pos3DTemplate.hpp"

/**
 * @enum Stage
 * @brief Represents the various stages of chunk generation.
 */
enum class Stage : u8 {
    STAGE_TERRAIN = 0, ///< Terrain generation stage.
    STAGE_WATER_CAVES = 1, ///< Water caves generation stage.
    STAGE_WATER_RAVINES = 2, ///< Water ravines generation stage.
    STAGE_CAVES = 4, ///< Caves generation stage.
    STAGE_RAVINES = 8, ///< Ravines generation stage.
    STAGE_STRUCTURE = 16, ///< Structure generation stage.
    STAGE_DECORATE = 32, ///< Decoration stage.
    STAGE_DONE = 64 ///< Final stage, generation complete.
};


enum class BlockOrder : u8 {
    XZY,
    OTHER,
};

static constexpr BlockOrder CHUNK_PRIMER_BLOCK_ORDER = BlockOrder::XZY;

/**
 * @class ChunkPrimer
 * @brief Represents a chunk and provides utilities for managing its blocks, data, and generation stages.
 */
class ChunkPrimer {
    static constexpr int STORAGE_SIZE = 16 * (256) * 16; ///< Total storage size for blocks in the chunk.

public:

    std::atomic<Stage> stage = Stage::STAGE_TERRAIN; ///< Current stage of chunk generation.

    std::atomic_bool isModifying = false; ///< Indicates whether the chunk is being modified.

    RNG decorateRng; ///< Random number generator used for decoration.

    std::array<u16, STORAGE_SIZE> blocks = std::array<u16, STORAGE_SIZE>(); ///< Array storing block IDs and data for the chunk.

    std::vector<u8> skyLight; ///< Buffer for storing skylight values.

    int highestYBlock = -1; ///< The highest Y-coordinate of a non-air block in the chunk.

    MU u8 heightMap[16][16]{}; ///< Height map for the chunk.

    std::vector<int> precipitationHeightMap = std::vector(256, -999); ///< Precipitation height map for the chunk.

    ChunkPrimer() = default; ///< Default constructor.

    ChunkPrimer(const ChunkPrimer &) = delete; ///< Deleted copy constructor to prevent copying.

    ChunkPrimer &operator=(const ChunkPrimer &) = delete; ///< Deleted copy assignment operator to prevent copying.

    void reset() {
        stage = Stage::STAGE_TERRAIN;
        isModifying = false;
        decorateRng = RNG();
        std::memset(blocks.data(), 0, STORAGE_SIZE * sizeof(u16));
        // skyLight.clear();
        highestYBlock = -1;
        // std::memset(heightMap, 0, 256);
        // precipitationHeightMap = std::vector(256, -999);
    }

    /**
     * @brief Retrieves the block state at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @return The block state.
     */
    ND inline lce::BlockState getBlock(c_i32 x, c_i32 y, c_i32 z) const {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return lce::BlocksInit::AIR.getState(); }
        c_u16 block = blocks[getStorageIndex(x, y, z)];
        return {static_cast<u16>(block >> 4),  static_cast<u8>(block & 15)};
    }

    /**
     * @brief Retrieves the block state at the specified position.
     * @param pos The position.
     * @return The block state.
     */
    ND inline lce::BlockState getBlock(const Pos3D &pos) const {
        return getBlock(pos.x, pos.y, pos.z);
    }

    /**
     * @brief Retrieves the block ID at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @return The block ID.
     */
    ND inline u16 getBlockId(c_i32 x, c_i32 y, c_i32 z) const {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return 0; }
        return blocks[getStorageIndex(x, y, z)] >> 4;
    }

    /**
     * @brief Retrieves the block ID at the specified position.
     * @param pos The position.
     * @return The block ID.
     */
    ND inline u16 getBlockId(const Pos3D &pos) const {
        return this->getBlockId(pos.x, pos.y, pos.z);
    }

    /**
     * @brief Sets the block state at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @param block The block state to set.
     */
    inline void setBlock(c_i32 x, c_i32 y, c_i32 z, const lce::BlockState block) {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return; }
        // TODO: optimize this
        blocks[getStorageIndex(x, y, z)] = static_cast<u16>((block.getID() << 4) | block.getDataTag());
    }

    /**
     * @brief Sets the block state at the specified position.
     * @param pos The position.
     * @param block The block state to set.
     */
    inline void setBlock(const Pos3D &pos, const lce::BlockState block) {
        this->setBlock(pos.x, pos.y, pos.z, block);
    }

    /**
     * @brief Sets both the block ID and data at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @param id The block ID to set.
     * @param data The block data to set.
     */
    MU inline void setBlockAndData(c_i32 x, c_i32 y, c_i32 z, c_u16 id, c_u8 data) {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return; }
        blocks[getStorageIndex(x, y, z)] = static_cast<u16>((id << 4) | data);
    }

    /**
     * @brief Sets both the block ID and data at the specified position.
     * @param pos The position.
     * @param id The block ID to set.
     * @param data The block data to set.
     */
    MU inline void setBlockAndData(const Pos3D &pos, c_u16 id, c_u8 data) {
        this->setBlockAndData(pos.x, pos.y, pos.z, id, data);
    }

    /**
     * @brief Sets the block ID at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @param block The block ID to set.
     */
    MU inline void setBlockId(c_i32 x, c_i32 y, c_i32 z, c_u16 block) {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return; }
        blocks[getStorageIndex(x, y, z)] = static_cast<u16>(block << 4);
    }

    /**
     * @brief Sets the block ID at the specified position.
     * @param pos The position.
     * @param block The block ID to set.
     */
    MU inline void setBlockId(const Pos3D &pos, c_u16 block) {
        this->setBlockId(pos.x, pos.y, pos.z, block);
    }

    ND inline u16* mutBlockPtr(c_i32 x, c_i32 y, c_i32 z) {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) return nullptr;
        return &blocks[getStorageIndex(x, y, z)];
    }

    /**
     * @brief Retrieves the skylight value at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @return The skylight value.
     */
    ND u16 getSkyLight(c_i32 x, c_i32 y, c_i32 z) const {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return 0; }
        return blocks[getStorageIndex(x, y, z)];
    }

    /**
     * @brief Sets the skylight value at the specified coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @param lightValue The skylight value to set.
     */
    void setSkyLight(c_i32 x, c_i32 y, c_i32 z, c_u8 lightValue) {
        if EXPECT_FALSE(isInvalidIndex(x, y, z)) { return; }
        // skyLight[getStorageIndex(x, y, z)] = lightValue;
    }

    /**
     * @brief Checks if the block at the specified coordinates is an air block.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @return True if the block is air, false otherwise.
     */
    ND bool isAirBlock(c_i32 x, c_i32 y, c_i32 z) const {
        return getBlockId(x, y, z) == lce::blocks::AIR_ID;
    }

    /**
     * @brief Checks if the block at the specified position is an air block.
     * @param pos The position.
     * @return True if the block is air, false otherwise.
     */
    ND bool isAirBlock(const Pos3D &pos) const {
        return isAirBlock(pos.x, pos.y, pos.z);
    }

    /**
     * @brief Retrieves the highest Y-coordinate of a non-air block in the chunk.
     * @return The highest Y-coordinate.
     */
    ND int getHighestYBlock() {
        if (highestYBlock != -1) return highestYBlock;

        for (int y = 255; y >= 0; y--) {
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    if (getBlockId(x, y, z) != lce::blocks::AIR_ID) {
                        highestYBlock = y;
                        return y;
                    }
                }
            }
        }
        return 0;
    }

    /**
     * @brief Retrieves the height of the terrain at the specified coordinates.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @return The height of the terrain.
     */
    ND int getHeight(c_i32 x, c_i32 z) const {
        for (int y = 255; y >= 0; y--) {
            if (lce::blocks::hasLightOpacity(getBlockId(x, y - 1, z))) return y;
        }
        return 0;
    }

    /**
     * @brief Retrieves the height of the terrain at the specified position.
     * @param pos The position.
     * @return The height of the terrain.
     */
    ND int getHeight(const Pos3D &pos) const {
        return getHeight(pos.getX(), pos.getZ());
    }

    /**
     * @brief Retrieves the position of the highest block at the specified position.
     * @param pos The position.
     * @return The position of the highest block.
     */
    ND Pos3D getHeightPos(const Pos3D &pos) const {
        return {pos.x, getHeight(pos), pos.z};
    }

    /**
     * @brief Retrieves the top solid or liquid block at the specified coordinates.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @return The Y-coordinate of the top solid or liquid block.
     */
    ND int getTopSolidOrLiquidBlock(c_i32 x, c_i32 z) const {
        for (int i = 255; i >= 0; i--) {
            c_int blockId = getBlockId(x, i - 1, z);
            if (lce::blocks::blocksMovement(blockId) && !lce::blocks::isLeavesBlock(blockId)) return i;
        }
        return 0;
    }


    /**
     * @brief Checks if the given block coordinates are invalid.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @return True if the coordinates are invalid, false otherwise.
     */
    MU ND static bool isInvalidIndex(c_i32 x, c_i32 y, c_i32 z) {
        return (((x | z) & ~15) || (y & ~255));
    }

    /**
     * @brief Calculates the storage index for the given block coordinates.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @return The storage index.
     */
    static size_t getStorageIndex(c_i32 x, c_i32 y, c_i32 z) {
        const auto value = static_cast<size_t>(x << 12 | z << 8 | y);
        // c_i32 value = y << 8 | x << 4 | z;
        return value;
    }

    /**
     * @brief Retrieves the light opacity of the specified block ID.
     * @param blockId The block ID.
     * @return The light opacity value.
     */
    static int getBlockLightOpacity(c_u16 blockId) {
        switch (blockId) {
            case lce::blocks::AIR_ID:
            case lce::blocks::SNOW_ID:
            case lce::blocks::CARPET_ID:
                return 0;
            case lce::blocks::COBWEB_ID:
                return 1;
            case lce::blocks::FLOWING_WATER_ID:
            case lce::blocks::STILL_WATER_ID:
            case lce::blocks::ICE_ID:
            case lce::blocks::FROSTED_ICE_ID:
                return 3;
            default:
                return 255;
        }
    }

    /**
     * @brief Generates the skylight map for the chunk.
     */
    void generateSkylightMap();

    /**
     * @brief Retrieves the precipitation height at the specified world coordinates.
     * @param wX World X-coordinate.
     * @param wZ World Z-coordinate.
     * @return The precipitation height.
     */
    int getPrecipitationHeight(int wX, int wZ);

    /**
     * @brief Checks if the block at the specified position can freeze.
     * @param pos The position.
     * @return True if the block can freeze, false otherwise.
     */
    ND bool canBlockFreeze(const Pos3D &pos) const;

    /**
     * @brief Checks if snow can accumulate at the specified position.
     * @param pos The position.
     * @return True if snow can accumulate, false otherwise.
     */
    ND bool canSnowAt(const Pos3D &pos) const;

    /**
     * @brief Calculates the memory footprint size of the ChunkPrimer in constant expression time.
     * @return The memory footprint size in bytes.
     */
    ND constexpr static size_t getFootprintSize() {
        constexpr size_t size = sizeof(ChunkPrimer) + sizeof(u16) * STORAGE_SIZE + 256 * sizeof(int);
        return size;
    }

    /**
     * @brief Overloads the output stream operator to print the chunk data.
     * @param out The output stream.
     * @param chunkPrimer The chunk primer to print.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &out, const ChunkPrimer &chunkPrimer) {
        for (c_u16 block: chunkPrimer.blocks) {
            out << std::hex << std::setw(2) << std::setfill('0') << (block & 0xff);
            out << std::hex << std::setw(2) << std::setfill('0') << (block >> 8);
        }
        return out;
    }

};

static_assert(BlockAccess<ChunkPrimer>, "ChunkPrimer must satisfy BlockAccess concept");
