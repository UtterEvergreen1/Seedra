#pragma once

#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

class ChunkPrimer {
public:
    /// all the blocks along with data in the chunk
    uint16_t blocks[65536]{};
    std::vector<uint8_t> skyLight;
    std::vector<int> precipitationHeightMap = std::vector<int>(256, -999);

    ChunkPrimer() = default;

    /// do not allow copy
    ChunkPrimer(const ChunkPrimer&) = delete;

    ChunkPrimer& operator=(const ChunkPrimer&) = delete;

    ///do not allow move
    //ChunkPrimer(ChunkPrimer &&) = delete;
    //ChunkPrimer &operator=(ChunkPrimer &&) = delete;

    inline uint16_t getBlockAtIndex(int64_t index) const { return index >= 0 && index < 65536 ? blocks[index] : 0; }

    uint16_t getBlock(int64_t x, int64_t y, int64_t z) const { return getBlockAtIndex(getStorageIndex(x, y, z)) >> 4; }

    void setBlock(int64_t x, int64_t y, int64_t z, uint16_t block) { blocks[getStorageIndex(x, y, z)] = block << 4; }

    uint16_t getData(int64_t x, int64_t y, int64_t z) const { return getBlockAtIndex(getStorageIndex(x, y, z)) & 15; }

    void setData(int64_t x, int64_t y, int64_t z, uint8_t data) { blocks[getStorageIndex(x, y, z)] |= data; }

    uint16_t getSkyLight(int64_t x, int64_t y, int64_t z) const { return getBlockAtIndex(getStorageIndex(x, y, z)); }

    void setSkyLight(int64_t x, int64_t y, int64_t z, uint8_t lightValue) {
        skyLight[getStorageIndex(x, y, z)] = lightValue;
    }

    void setBlockAndData(int64_t x, int64_t y, int64_t z, uint16_t block, uint8_t data) {
        blocks[getStorageIndex(x, y, z)] = ((block << 4) | data);
    }

    void setBlockAndData(int64_t x, int64_t y, int64_t z, const Items::Item& item) {
        blocks[getStorageIndex(x, y, z)] = ((item.getID() << 4) | item.getDataTag());
    }

    friend std::ostream& operator<<(std::ostream& out, const ChunkPrimer& chunkPrimer) {
        for (unsigned short block: chunkPrimer.blocks) {
            out << std::hex << std::setw(2) << std::setfill('0') << (block & 0xff);
            out << std::hex << std::setw(2) << std::setfill('0') << (block >> 8);
        }
        return out;
    }

    /// in block cords not chunk cords
    int getHighestYChunk() const {
        for (int y = 255; y >= 0; y--) {
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    if (getBlock(x, y, z)) { return y; }
                }
            }
        }
        return 0;
    }

    static inline int64_t getStorageIndex(int64_t x, int64_t y, int64_t z) {
        int64_t value = (x << 12) | (z << 8) | y;
        return value;
    }

    ///TODO add all the blocks or make a block class and get the value from that
    static int getBlockLightOpacity(uint16_t blockId) {
        switch (blockId) {
            case 0:
            case 78:
            case 171:
                return 0;
            case 30:
                return 1;
            case 8:
            case 9:
            case 79:
            case 212:
                return 3;
            default:
                return 255;
        }
    }

    void generateSkylightMap() {
        skyLight.resize(65536, 0);
        int topFilledSegment = getHighestYChunk();
        for (int x = 0; x < 16; ++x) {
            for (int z = 0; z < 16; ++z) {

                int lightValue = 15;
                int topPos = topFilledSegment + 15;
                while (true) {
                    int blockOpacity = getBlockLightOpacity(getBlock(x, topPos, z));
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
        int i = x & 15;
        int j = z & 15;
        int k = i | j << 4;

        if (precipitationHeightMap[k] == -999) {
            int highestY = getHighestYChunk() + 15;
            int i1 = -1;

            while (highestY > 0 && i1 == -1) {
                if (!getBlock(i, highestY, j)) i1 = highestY + 1;
                else
                    highestY -= 1;
            }

            precipitationHeightMap[k] = i1;
        }
        return precipitationHeightMap[k];
    }

    bool canBlockFreeze(const Generator& g, Pos3D pos, bool noWaterAdj) const {
        Biome* biome = Biome::getBiomeForId(g.getBiomeAt(1, pos.getX(), pos.getZ()));
        float f = biome->getFloatTemperature(pos);
        if (f >= 0.15F) return false;
        else {
            if (pos.getY() >= 0 && pos.getY() < 256) {
                int x = pos.getX() & 15;
                int z = pos.getZ() & 15;
                uint16_t iBlockState = getBlock(x, pos.getY(), z);
                uint16_t block = iBlockState;

                if ((block == 8 || block == 9)) {
                    if (!noWaterAdj) return true;

                    uint16_t flagBlockWest = getBlock(x - 1, pos.getY(), z);
                    uint16_t flagBlockEast = getBlock(x + 1, pos.getY(), z);
                    uint16_t flagBlockNorth = getBlock(x, pos.getY(), z - 1);
                    uint16_t flagBlockSouth = getBlock(x, pos.getY(), z + 1);
                    bool flag = flagBlockWest == 9 && flagBlockEast == 9 && flagBlockNorth == 9 && flagBlockSouth == 9;

                    if (!flag) return true;
                }
            }

            return false;
        }
    }

    bool canSnowAt(const Generator& g, Pos3D pos, bool checkLight) {
        Biome* biome = Biome::getBiomeForId(g.getBiomeAt(1, pos.getX(), pos.getZ()));
        float temp = biome->getFloatTemperature(pos);
        if (temp >= 0.15F) return false;
        else if (!checkLight)
            return true;
        else {
            // needs to check block light later on to replace a perfect chunk
            if (pos.getY() >= 0 && pos.getY() < 256 /* && getLightFor(EnumSkyBlock.BLOCK, pos) < 10*/) {
                uint16_t iBlockState = getBlock(pos.getX(), pos.getY(), pos.getZ());
                if (!iBlockState) return true;
            }
            return false;
        }
    }
};
