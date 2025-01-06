#include "ChunkPrimer.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
void ChunkPrimer::generateSkylightMap() {
    skyLight.resize(65536, 0);
    c_int topFilledSegment = getHighestYBlock();
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

    using namespace lce::blocks;
    if (precipitationHeightMap[k] == -999) {
        int highestY = this->getHighestYBlock();
        int y = -1;

        while (highestY > 0 && y == -1) {
            int blockID = getBlockId(i, highestY, j);
            if (ids::blocksMovement(blockID) || ids::isLiquidBlock(blockID)) y = highestY + 1;
            else
                highestY -= 1;
        }

        precipitationHeightMap[k] = y;
    }
    return precipitationHeightMap[k];
}

bool ChunkPrimer::canBlockFreeze(const Pos3D &pos) const {
    if (pos.getY() >= 0 && pos.getY() < 256) {
        c_u16 iBlockState = getBlockId(pos);
        c_u16 block = iBlockState;

        if (block == 8 || block == 9) {
            return true;
        }
    }

    return false;
}

bool ChunkPrimer::canSnowAt(const Pos3D &pos) const {
    if (pos.getY() >= 0 && pos.getY() < 256 /* && getLightFor(EnumSkyBlock.BLOCK, pos) < 10*/) {
        if (getBlockId(pos.getX(), pos.getY(), pos.getZ()) == lce::blocks::ids::AIR_ID) return true;
    }
    return false;
}
