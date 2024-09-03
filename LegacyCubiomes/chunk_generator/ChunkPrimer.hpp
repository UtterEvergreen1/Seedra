#pragma once

#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

class ChunkPrimer {
public:
    /// all the blocks along with data in the chunk
    u16 blocks[65536]{};
    std::vector<u8> skyLight;
    std::vector<int> precipitationHeightMap = std::vector(256, -999);

    ChunkPrimer() = default;

    /// do not allow copy
    ChunkPrimer(const ChunkPrimer&) = delete;

    ChunkPrimer& operator=(const ChunkPrimer&) = delete;

    ///do not allow move
    //ChunkPrimer(ChunkPrimer &&) = delete;
    //ChunkPrimer &operator=(ChunkPrimer &&) = delete;

    ND u16 getBlockAtIndex(c_i64 index) const {
        return index >= 0 && index < 65536 ? blocks[index] : 0; }

    ND u16 getBlockId(c_i64 x, c_i64 y, c_i64 z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z)) >> 4;
    }

    void setBlockId(c_i64 x, c_i64 y, c_i64 z, c_u16 block) {
        blocks[getStorageIndex(x, y, z)] = block << 4;
    }

    ND u16 getData(c_i64 x, c_i64 y, c_i64 z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z)) & 15;
    }

    void setData(c_i64 x, c_i64 y, c_i64 z, c_u8 data) {
        blocks[getStorageIndex(x, y, z)] |= data;
    }

    ND u16 getSkyLight(c_i64 x, c_i64 y, c_i64 z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z));
    }

    void setSkyLight(c_i64 x, c_i64 y, c_i64 z, c_u8 lightValue) {
        skyLight[getStorageIndex(x, y, z)] = lightValue;
    }

    void setBlockAndData(i64 x, i64 y, i64 z, const lce::blocks::Block& block) {
        blocks[getStorageIndex(x, y, z)] = ((block.getID() << 4) | block.getDataTag());
    }

    friend std::ostream& operator<<(std::ostream& out, const ChunkPrimer& chunkPrimer) {
        for (c_u16 block: chunkPrimer.blocks) {
            out << std::hex << std::setw(2) << std::setfill('0') << (block & 0xff);
            out << std::hex << std::setw(2) << std::setfill('0') << (block >> 8);
        }
        return out;
    }

    /// in block cords not chunk cords
    ND int getHighestYChunk() const {
        for (int y = 255; y >= 0; y--) {
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    if (getBlockId(x, y, z)) { return y; }
                }
            }
        }
        return 0;
    }

    static i64 getStorageIndex(c_i64 x, c_i64 y, c_i64 z) {
        c_i64 value = x << 12 | z << 8 | y;
        return value;
    }

    // TODO add all the blocks or make a block class and get the value from that
    static int getBlockLightOpacity(c_u16 blockId) {
        switch (blockId) {
            case lce::blocks::ids::AIR_ID:
            case lce::blocks::ids::SNOW_ID:
            case lce::blocks::ids::WHITE_CARPET_ID:
                return 0;
            case lce::blocks::ids::COBWEB_ID:
                return 1;
            case lce::blocks::ids::FLOWING_WATER_ID:
            case lce::blocks::ids::STILL_WATER_ID:
            case lce::blocks::ids::ICE_ID:
            case lce::blocks::ids::FROSTED_ICE_ID:
                return 3;
            default:
                return 255;
        }
    }

    void generateSkylightMap() {
        skyLight.resize(65536, 0);
        c_int topFilledSegment = getHighestYChunk();
        for (int x = 0; x < 16; ++x) {
            for (int z = 0; z < 16; ++z) {

                int lightValue = 15;
                int topPos = topFilledSegment + 15;
                while (true) {
                    int blockOpacity = getBlockLightOpacity(getBlockId(x, topPos, z));
                    if (blockOpacity == 0 && lightValue != 15) blockOpacity = 1;

                    lightValue -= blockOpacity;

                    if (lightValue > 0) setSkyLight(x, topPos, z, lightValue);

                    topPos--;

                    if (lightValue <= 0 || topPos <= 0) break;
                }
            }
        }
    }

    int getPrecipitationHeight(int x, int z) {
        c_int i = x & 15;
        c_int j = z & 15;
        c_int k = i | j << 4;

        if (precipitationHeightMap[k] == -999) {
            int highestY = getHighestYChunk() + 15;
            int i1 = -1;

            while (highestY > 0 && i1 == -1) {
                if (!getBlockId(i, highestY, j)) i1 = highestY + 1;
                else
                    highestY -= 1;
            }

            precipitationHeightMap[k] = i1;
        }
        return precipitationHeightMap[k];
    }

    ND bool canBlockFreeze(const Generator& g, const Pos3D pos, c_bool noWaterAdj) const {
        if (Biome::getBiomeForId(g.getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F)
            return false;

        if (pos.getY() >= 0 && pos.getY() < 256) {
            c_int x = pos.getX() & 15;
            c_int z = pos.getZ() & 15;
            c_u16 iBlockState = getBlockId(x, pos.getY(), z);
            c_u16 block = iBlockState;

            if (block == 8 || block == 9) {
                if (!noWaterAdj) return true;

                c_u16 flagBlockWest = getBlockId(x - 1, pos.getY(), z);
                c_u16 flagBlockEast = getBlockId(x + 1, pos.getY(), z);
                c_u16 flagBlockNorth = getBlockId(x, pos.getY(), z - 1);
                c_u16 flagBlockSouth = getBlockId(x, pos.getY(), z + 1);
                c_bool flag =
                        flagBlockWest == 9 && flagBlockEast == 9 && flagBlockNorth == 9 && flagBlockSouth == 9;

                if (!flag) return true;
            }
        }

        return false;
    }

    ND bool canSnowAt(const Generator& g, const Pos3D pos, c_bool checkLight) const {
        if (Biome::getBiomeForId(g.getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F) {
            return false;
        }

        if (!checkLight) { return true; }

        // needs to check block light later on to replace a perfect chunk
        if (pos.getY() >= 0 && pos.getY() < 256 /* && getLightFor(EnumSkyBlock.BLOCK, pos) < 10*/) {
            if (!getBlockId(pos.getX(), pos.getY(), pos.getZ())) return true;
        }
        return false;
    }
};
