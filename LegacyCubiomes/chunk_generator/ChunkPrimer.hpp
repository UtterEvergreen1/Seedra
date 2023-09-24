#pragma once

#include <vector>
#include <cmath>
#include <string>

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"
#include "LegacyCubiomes/utils/pos3D.hpp"

class ChunkPrimer
{
public:
    uint16_t blocks[65536]; // all the blocks along with data in the chunk
    std::vector<int> precipitationHeightMap = std::vector<int>(256, -999);
    inline uint16_t getBlockAtIndex(int64_t index) {
        return index >= 0 && index < 65536 ? blocks[index] : 0;
    }
    uint16_t getBlock(int64_t x, int64_t y, int64_t z) {
        return getBlockAtIndex(getBlockIndex(x, y, z)) >> 4;
    }
    void setBlock(int64_t x, int64_t y, int64_t z, uint16_t block)
    {
        this->blocks[getBlockIndex(x, y, z)] = block << 4;
    }

    uint16_t getData(int64_t x, int64_t y, int64_t z)
    {
        return getBlockAtIndex(getBlockIndex(x, y, z)) & 15;
    }
    void setData(int64_t x, int64_t y, int64_t z, uint8_t data)
    {
        this->blocks[getBlockIndex(x, y, z)] |= data;
    }

    void setBlockAndData(int64_t x, int64_t y, int64_t z, uint16_t block, uint8_t data)
    {
        this->blocks[getBlockIndex(x, y, z)] = ((block << 4) | data);
    }

    void setBlockAndData(int64_t x, int64_t y, int64_t z, const Items::Item& item)
    {
        this->blocks[getBlockIndex(x, y, z)] = ((item.getID() << 4) | item.getDataTag());
    }

    //in block cords not chunk cords
    int getHighestYChunk() {
        for (int y = 255; y >= 0; y--) {
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    if (getBlock(x, y, z)) {
                        return y;
                    }
                }
            }
        }
        return 0;
    }

    static inline int64_t getBlockIndex(int64_t x, int64_t y, int64_t z)
    {
        int64_t value = (x << 12) | (z << 8) | y;
        return value;
    }

    Pos3D getPrecipitationHeight(Pos2D pos)
    {
        int i = pos.x & 15;
        int j = pos.z & 15;
        int k = i | j << 4;

        if (precipitationHeightMap[k] == -999)
        {
            int highestY = getHighestYChunk() + 15;
            int i1 = -1;

            while (highestY > 0 && i1 == -1)
            {
                if (!getBlock(i, highestY, j))
                    i1 = highestY + 1;
                else
                    highestY -= 1;
            }

            precipitationHeightMap[k] = i1;
        }

        return Pos3D(pos.x, precipitationHeightMap[k], pos.z);
    }

    bool canBlockFreeze(const Generator& g, Pos3D pos, bool noWaterAdj){
        Biome* biome = Biome::getBiomeForId(g.getBiomeAt(1, pos.x, pos.z));
        float f = biome->getFloatTemperature(pos);
        if (f >= 0.15F)
            return false;
        else
        {
            if (pos.y >= 0 && pos.y < 256)
            {
                int x = pos.x & 15;
                int z = pos.z & 15;
                uint16_t iblockstate = getBlock(x, pos.y, z);
                uint16_t block = iblockstate;

                if ((block == 8 || block == 9))
                {
                    if (!noWaterAdj)
                        return true;

                    uint16_t flagBlockWest = getBlock(x - 1, pos.y, z);
                    uint16_t flagBlockEast = getBlock(x + 1, pos.y, z);
                    uint16_t flagBlockNorth = getBlock(x, pos.y, z - 1);
                    uint16_t flagBlockSouth = getBlock(x, pos.y, z + 1);
                    bool flag = flagBlockWest == 9 && flagBlockEast == 9 && flagBlockNorth == 9 && flagBlockSouth == 9;

                    if (!flag)
                        return true;
                }
            }

            return false;
        }
    }

    bool canSnowAt(const Generator& g, Pos3D pos, bool checkLight){
        Biome* biome = Biome::getBiomeForId(g.getBiomeAt(1, pos.x, pos.z));
        float temp = biome->getFloatTemperature(pos);
        if (temp >= 0.15F)
            return false;
        else if (!checkLight)
            return true;
        else
        {
            //TODO: check light
            if (pos.y >= 0 && pos.y < 256 /* && this.getLightFor(EnumSkyBlock.BLOCK, pos) < 10*/)
            {
                uint16_t iblockstate = getBlock(pos.x, pos.y, pos.z);
                if (!iblockstate)
                    return true;
            }
            return false;
        }
    }

};
