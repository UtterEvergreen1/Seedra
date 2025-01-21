#pragma once

#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

#include "lce/blocks/__include.hpp"

#include "lce/registry/blockRegistry.hpp"

#include "components/BoundingBox.hpp"
#include "generator.hpp"
#include "common/Pos3DTemplate.hpp"


enum class Stage : u8 {
    STAGE_TERRAIN = 0,
    STAGE_CAVES = 1,
    STAGE_STRUCTURE = 2,
    STAGE_DECORATE = 4,
    STAGE_DONE = 8
};

class ChunkPrimer {
public:
    /// all the blocks along with data in the chunk
    u16 blocks[65536]{};
    Stage stage = Stage::STAGE_TERRAIN;
    std::vector<u8> skyLight;
    std::vector<int> precipitationHeightMap = std::vector(256, -999);
    int highestYBlock = -1;
    RNG decorateRng;


    ChunkPrimer() = default;

    /// do not allow copy
    ChunkPrimer(const ChunkPrimer &) = delete;

    ChunkPrimer &operator=(const ChunkPrimer &) = delete;

    ///do not allow move
    //ChunkPrimer(ChunkPrimer &&) = delete;
    //ChunkPrimer &operator=(ChunkPrimer &&) = delete;

    ND u16 getBlockAtIndex(c_i64 index) const {
        return index >= 0 && index < 65536 ? blocks[index] : 0;
    }

    ND u16 getBlockId(c_i64 x, c_i64 y, c_i64 z) const {
        if (x < 0 || y < 0 || z < 0 || x >= 16 || y >= 256 || z >= 16) {/*std::cout << "Invalid coords: " << x << ", " << y << ", " << z << std::endl;*/ return 0;}
        return getBlockAtIndex(getStorageIndex(x, y, z)) >> 4;
    }

    ND u16 getBlockId(const Pos3D &pos) const {
        return this->getBlockId(pos.getX(), pos.getY(), pos.getZ());
    }

    void setBlockId(c_i64 x, c_i64 y, c_i64 z, c_u16 block) {
        if (x < 0 || y < 0 || z < 0 || x >= 16 || y >= 256 || z >= 16) {/*std::cout << "Invalid coords: " << x << ", " << y << ", " << z << std::endl;*/ return;}
        blocks[getStorageIndex(x, y, z)] = block << 4;
    }

    void setBlockId(const Pos3D &pos, c_u16 block) {
        this->setBlockId(pos.getX(), pos.getY(), pos.getZ(), block);
    }

    ND u16 getData(c_i64 x, c_i64 y, c_i64 z) const {
        if (x < 0 || y < 0 || z < 0 || x >= 16 || y >= 256 || z >= 16) {/*std::cout << "Invalid coords: " << x << ", " << y << ", " << z << std::endl;*/ return 0;}
        return getBlockAtIndex(getStorageIndex(x, y, z)) & 15;
    }

    ND u16 getData(const Pos3D &pos) const {
        return this->getData(pos.getX(), pos.getY(), pos.getZ());
    }

    void setData(c_i64 x, c_i64 y, c_i64 z, c_u8 data) {
        if (x < 0 || y < 0 || z < 0 || x >= 16 || y >= 256 || z >= 16) {/*std::cout << "Invalid coords: " << x << ", " << y << ", " << z << std::endl;*/ return;}
        blocks[getStorageIndex(x, y, z)] |= data;
    }

    void setData(const Pos3D &pos, c_u8 data) {
        this->setData(pos.getX(), pos.getY(), pos.getZ(), data);
    }

    ND const lce::Block* getBlock(c_i64 x, c_i64 y, c_i64 z) const {
        return lce::registry::BlockRegistry::getBlock(getBlockId(x, y, z), getData(x, y, z));
    }

    ND const lce::Block* getBlock(const Pos3D &pos) const {
        return this->getBlock(pos.getX(), pos.getY(), pos.getZ());
    }

    ND u16 getSkyLight(c_i64 x, c_i64 y, c_i64 z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z));
    }

    void setSkyLight(c_i64 x, c_i64 y, c_i64 z, c_u8 lightValue) {
        skyLight[getStorageIndex(x, y, z)] = lightValue;
    }

    void setBlockAndData(c_i64 x, c_i64 y, c_i64 z, c_int id, c_int data) {
        if (x < 0 || y < 0 || z < 0 || x >= 16 || y >= 256 || z >= 16) {/*std::cout << "Invalid coords: " << x << ", " << y << ", " << z << std::endl;*/ return;}
        blocks[getStorageIndex(x, y, z)] = ((id << 4) | data);
    }

    void setBlockAndData(const Pos3D &pos, c_int id, c_int data) {
        this->setBlockAndData(pos.getX(), pos.getY(), pos.getZ(), id, data);
    }

    void setBlock(c_i64 x, c_i64 y, c_i64 z, const lce::Block *block) {
        this->setBlockAndData(x, y, z, block->getID(), block->getDataTag());
    }

    void setBlock(const Pos3D &pos, const lce::Block *block) {
        this->setBlock(pos.getX(), pos.getY(), pos.getZ(), block);
    }

    ND bool isAirBlock(c_i64 x, c_i64 y, c_i64 z) const {
        return getBlockId(x, y, z) == lce::blocks::AIR_ID;
    }

    ND bool isAirBlock(const Pos3D &pos) const {
        return isAirBlock(pos.getX(), pos.getY(), pos.getZ());
    }

    friend std::ostream &operator<<(std::ostream &out, const ChunkPrimer &chunkPrimer) {
        for (c_u16 block: chunkPrimer.blocks) {
            out << std::hex << std::setw(2) << std::setfill('0') << (block & 0xff);
            out << std::hex << std::setw(2) << std::setfill('0') << (block >> 8);
        }
        return out;
    }

    /// in block cords not chunk cords
    ND int getHighestYBlock() {
        if (highestYBlock != -1) return highestYBlock;
        for (int y = 255; y >= 0; y--) {
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    if (getBlockId(x, y, z) != lce::blocks::AIR_ID) { highestYBlock = y; return y; }
                }
            }
        }
        return 0;
    }

    ND int getHeight(c_i64 x, c_i64 z) const {
        for (int y = 255; y >= 0; y--) {
            if (lce::blocks::hasLightOpacity(getBlockId(x, y - 1, z))) return y;
        }
        return 0;
    }

    ND int getHeight(const Pos3D &pos) const {
        return getHeight(pos.getX(), pos.getZ());
    }

    ND Pos3D getHeightPos(const Pos3D &pos) const {
        return {pos.x, getHeight(pos), pos.z};
    }

    ND int getTopSolidOrLiquidBlock(c_i64 x, c_i64 z) const {
        for (int i = 255; i >= 0; i--) {
            int blockId = getBlockId(x, i - 1, z);
            if (lce::blocks::blocksMovement(blockId) && !lce::blocks::isLeavesBlock(blockId)) return i;
        }
        return 0;
    }

    static i64 getStorageIndex(c_i64 x, c_i64 y, c_i64 z) {
        c_i64 value = x << 12 | z << 8 | y;
        return value;
    }

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

    void generateSkylightMap();

    int getPrecipitationHeight(int x, int z);

    ND bool canBlockFreeze(const Pos3D &pos) const;

    ND bool canSnowAt(const Pos3D &pos) const;
};
