#include "ChunkPrimer.hpp"

#include "lce/__include.hpp"


using namespace lce::blocks;

void ChunkPrimer::generateSkylightMap() {
    skyLight.resize(65536, 0);
    c_int topFilledSegment = getHighestYBlock();
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            u8 lightValue = 15;
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

int ChunkPrimer::getPrecipitationHeight(c_int wX, c_int wZ) {
    c_int x = wX & 15;
    c_int z = wZ & 15;
    const auto k = static_cast<size_t>(x | z << 4);

    using namespace lce::blocks;
    if (precipitationHeightMap[k] == -999) {
        int highestY = this->getHighestYBlock();
        int y = -1;

        while (highestY > 0 && y == -1) {
            int blockID = getBlockId(x, highestY, z);
            if (blocksMovement(blockID) || isLiquidBlock(blockID)) {
                y = highestY + 1;
            } else {
                highestY -= 1;
            }
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


static bool canPlaceSnowLayerAt(const ChunkPrimer* chunkPrimer, Pos3D pos) {
    const lce::BlockState block = chunkPrimer->getBlock(pos);

    if (block.getID() != lce::blocks::AIR_ID) { return false; }

    const lce::BlockState blockDown = chunkPrimer->getBlock(pos.down());

    if (blockDown == lce::BlocksInit::ICE || blockDown == lce::BlocksInit::PACKED_ICE) { return false; }

    // auto blockFaceShape = block.func_193401_d(chunkPrimer, pos.down(), EnumFacing::UP);
    return isFullBlock(blockDown.getID()) /* blockFaceShape == BlockFaceShape.SOLID || */
           || lce::blocks::isLeavesBlock(blockDown.getID())
           || (blockDown.getID() == lce::blocks::SNOW_ID && blockDown.getDataTag() == 8);
}



bool ChunkPrimer::canSnowAt(const Pos3D &pos) const {
    // if (getLightFor(EnumSkyBlock.BLOCK, pos) < 10*/) {
    if (canPlaceSnowLayerAt(this, pos)) { return true; }
    // }
    return false;
}
