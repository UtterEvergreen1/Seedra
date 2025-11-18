#include "WorldGenHugeTrees.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


WorldGenHugeTrees::~WorldGenHugeTrees() = default;


int WorldGenHugeTrees::getHeight(RNG &rand) const {
    int height = rand.nextInt(3) + this->m_baseHeight;
    if (this->m_extraRandomHeight > 1) {
        height += rand.nextInt(this->m_extraRandomHeight);
    }
    return height;
}

bool WorldGenHugeTrees::isSpaceAt(World *worldIn, const Pos3D &leavesPos, const int height) {
    if (leavesPos.getY() < 1 || leavesPos.getY() + height > 255) {
        return false;
    }

    for (int y_off = 0; y_off <= 1 + height; ++y_off) {
        const int radius = y_off == 0 ? 1 : 2;
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

bool WorldGenHugeTrees::ensureDirtUnderneath(const Pos3D &pos, World *worldIn) {
    const Pos3D blockPos = pos.down();
    const int block = worldIn->getBlockId(blockPos);

    if ((block == GRASS_ID || block == DIRT_ID) && pos.getY() >= 2) {
        setDirtAt(worldIn, blockPos);
        setDirtAt(worldIn, blockPos.east());
        setDirtAt(worldIn, blockPos.south());
        setDirtAt(worldIn, blockPos.south().east());
        return true;
    }
    return false;
}

bool WorldGenHugeTrees::ensureGrowable(World *worldIn, MU RNG &rng, const Pos3D &treePos, const int height) {
    return isSpaceAt(worldIn, treePos, height) && ensureDirtUnderneath(treePos, worldIn);
}

void WorldGenHugeTrees::growLeavesLayerStrict(World *worldIn, const Pos3D &layerCenter, const int width) const {
    const int i = width * width;
    for (int j = -width; j <= width + 1; ++j) {
        for (int k = -width; k <= width + 1; ++k) {
            const int l = j - 1;
            const int i1 = k - 1;
            if (j * j + k * k <= i || l * l + i1 * i1 <= i || j * j + i1 * i1 <= i || l * l + k * k <= i) {
                Pos3D blockPos = layerCenter.add(j, 0, k);
                const int material = worldIn->getBlockId(blockPos);
                if (lce::blocks::isAirOrLeavesBlock(material)) {
                    worldIn->setBlock(blockPos, m_leavesMetadata);
                }
            }
        }
    }
}

void WorldGenHugeTrees::growLeavesLayer(World *worldIn, const Pos3D &layerCenter, const int width) const {
    const int i = width * width;
    for (int j = -width; j <= width; ++j) {
        for (int k = -width; k <= width; ++k) {
            if (j * j + k * k <= i) {
                Pos3D blockPos = layerCenter.add(j, 0, k);
                const int material = worldIn->getBlockId(blockPos);
                if (lce::blocks::isAirOrLeavesBlock(material)) {
                    worldIn->setBlock(blockPos, m_leavesMetadata);
                }
            }
        }
    }
}
