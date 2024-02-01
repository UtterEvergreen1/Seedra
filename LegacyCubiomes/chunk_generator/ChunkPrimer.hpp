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
    uint16_t blocks[65536]{};
    std::vector<uint8_t> skyLight;
    std::vector<int> precipitationHeightMap = std::vector(256, -999);

    ChunkPrimer() = default;

    /// do not allow copy
    ChunkPrimer(const ChunkPrimer&) = delete;

    ChunkPrimer& operator=(const ChunkPrimer&) = delete;

    ///do not allow move
    //ChunkPrimer(ChunkPrimer &&) = delete;
    //ChunkPrimer &operator=(ChunkPrimer &&) = delete;

    ND uint16_t getBlockAtIndex(const int64_t index) const { return index >= 0 && index < 65536 ? blocks[index] : 0; }

    ND uint16_t getBlockId(const int64_t x, const int64_t y, const int64_t z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z)) >> 4;
    }

    void setBlockId(const int64_t x, const int64_t y, const int64_t z, const uint16_t block) {
        blocks[getStorageIndex(x, y, z)] = block << 4;
    }

    ND uint16_t getData(const int64_t x, const int64_t y, const int64_t z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z)) & 15;
    }

    void setData(const int64_t x, const int64_t y, const int64_t z, const uint8_t data) {
        blocks[getStorageIndex(x, y, z)] |= data;
    }

    ND uint16_t getSkyLight(const int64_t x, const int64_t y, const int64_t z) const {
        return getBlockAtIndex(getStorageIndex(x, y, z));
    }

    void setSkyLight(const int64_t x, const int64_t y, const int64_t z, const uint8_t lightValue) {
        skyLight[getStorageIndex(x, y, z)] = lightValue;
    }

    void setBlockAndData(int64_t x, int64_t y, int64_t z, uint16_t block, uint8_t data) {
        blocks[getStorageIndex(x, y, z)] = ((block << 4) | data);
    }

    void setBlockAndData(int64_t x, int64_t y, int64_t z, const Items::Item& item) {
        blocks[getStorageIndex(x, y, z)] = ((item.getID() << 4) | item.getDataTag());
    }

    friend std::ostream& operator<<(std::ostream& out, const ChunkPrimer& chunkPrimer) {
        for (const uint16_t block: chunkPrimer.blocks) {
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

    static int64_t getStorageIndex(const int64_t x, const int64_t y, const int64_t z) {
        const int64_t value = x << 12 | z << 8 | y;
        return value;
    }

    ///TODO add all the blocks or make a block class and get the value from that
    static int getBlockLightOpacity(const uint16_t blockId) {
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
        const int topFilledSegment = getHighestYChunk();
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
        const int i = x & 15;
        const int j = z & 15;
        const int k = i | j << 4;

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

    bool canBlockFreeze(const Generator& g, const Pos3D pos, const bool noWaterAdj) const {
        if (Biome::getBiomeForId(g.getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F)
            return false;

        if (pos.getY() >= 0 && pos.getY() < 256) {
            const int x = pos.getX() & 15;
            const int z = pos.getZ() & 15;
            const uint16_t iBlockState = getBlockId(x, pos.getY(), z);
            const uint16_t block = iBlockState;

            if (block == 8 || block == 9) {
                if (!noWaterAdj) return true;

                const uint16_t flagBlockWest = getBlockId(x - 1, pos.getY(), z);
                const uint16_t flagBlockEast = getBlockId(x + 1, pos.getY(), z);
                const uint16_t flagBlockNorth = getBlockId(x, pos.getY(), z - 1);
                const uint16_t flagBlockSouth = getBlockId(x, pos.getY(), z + 1);
                const bool flag =
                        flagBlockWest == 9 && flagBlockEast == 9 && flagBlockNorth == 9 && flagBlockSouth == 9;

                if (!flag) return true;
            }
        }

        return false;
    }

    ND bool canSnowAt(const Generator& g, const Pos3D pos, const bool checkLight) const {
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
