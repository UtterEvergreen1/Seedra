#include "WorldGenHugeTrees.hpp"

int WorldGenHugeTrees::getHeight(RNG &rand) const {
    int height = rand.nextInt(3) + this->baseHeight;
    if (this->extraRandomHeight > 1) {
        height += rand.nextInt(this->extraRandomHeight);
    }
    return height;
}

bool WorldGenHugeTrees::isSpaceAt(World *worldIn, const Pos3D &leavesPos, int height) {
    if (leavesPos.getY() < 1 || leavesPos.getY() + height > 255) {
        return false;
    }

    for (int y_off = 0; y_off <= 1 + height; ++y_off) {
        int radius = (y_off == 0) ? 1 : 2;
        for (int x_off = -radius; x_off <= radius; ++x_off) {
            for (int z_off = -radius; z_off <= radius; ++z_off) {
                if (leavesPos.getY() + y_off < 0 || leavesPos.getY() + y_off >= 256 || !canGrowInto(
                        worldIn->getBlockId(leavesPos.add(x_off, y_off, z_off)))) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool WorldGenHugeTrees::ensureDirtsUnderneath(const Pos3D &pos, World *worldIn) {
    Pos3D blockpos = pos.down();
    int block = worldIn->getBlockId(blockpos);

    if ((block == lce::blocks::ids::GRASS_ID || block == lce::blocks::ids::DIRT_ID) && pos.getY() >= 2) {
        setDirtAt(worldIn, blockpos);
        setDirtAt(worldIn, blockpos.east());
        setDirtAt(worldIn, blockpos.south());
        setDirtAt(worldIn, blockpos.south().east());
        return true;
    }
    return false;
}

bool WorldGenHugeTrees::ensureGrowable(World *worldIn, RNG &rng, const Pos3D &treePos, int height) {
    return isSpaceAt(worldIn, treePos, height) && ensureDirtsUnderneath(treePos, worldIn);
}

void WorldGenHugeTrees::growLeavesLayerStrict(World *worldIn, const Pos3D &layerCenter, int width) const {
    int i = width * width;
    for (int j = -width; j <= width + 1; ++j) {
        for (int k = -width; k <= width + 1; ++k) {
            int l = j - 1;
            int i1 = k - 1;
            if (j * j + k * k <= i || l * l + i1 * i1 <= i || j * j + i1 * i1 <= i || l * l + k * k <= i) {
                Pos3D blockpos = layerCenter.add(j, 0, k);
                int material = worldIn->getBlockId(blockpos);
                if (material == lce::blocks::ids::AIR_ID || lce::blocks::ids::isLeavesBlock(material)) {
                    worldIn->setBlock(blockpos, leavesMetadata);
                }
            }
        }
    }
}

void WorldGenHugeTrees::growLeavesLayer(World *worldIn, const Pos3D &layerCenter, int width) const {
    int i = width * width;
    for (int j = -width; j <= width; ++j) {
        for (int k = -width; k <= width; ++k) {
            if (j * j + k * k <= i) {
                Pos3D blockpos = layerCenter.add(j, 0, k);
                int material = worldIn->getBlockId(blockpos);
                if (material == lce::blocks::ids::AIR_ID || lce::blocks::ids::isLeavesBlock(material)) {
                    worldIn->setBlock(blockpos, leavesMetadata);
                }
            }
        }
    }
}
