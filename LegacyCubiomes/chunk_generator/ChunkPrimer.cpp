#include "ChunkPrimer.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
void ChunkPrimer::generateSkylightMap() {
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

int ChunkPrimer::getPrecipitationHeight(c_int x, c_int z) {
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

bool ChunkPrimer::canBlockFreeze(const Generator &g, const Pos3D &pos, c_bool noWaterAdj) const {
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

bool ChunkPrimer::canSnowAt(const Generator &g, const Pos3D &pos, c_bool checkLight) const {
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
