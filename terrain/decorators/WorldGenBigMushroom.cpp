#include "WorldGenBigMushroom.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenBigMushroom::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    lce::BlockState block = this->m_mushroomType;

    if (block == lce::BlocksInit::AIR.getState()) {
        block = rand.nextBoolean() ? lce::BlocksInit::BROWN_MUSHROOM_BLOCK.getState()
                                   : lce::BlocksInit::RED_MUSHROOM_BLOCK.getState();
    }

    int height = rand.nextInt(3) + 4;

    if (rand.nextInt(12) == 0) { height *= 2; }

    if (position.getY() < 1 || position.getY() + height >= 255) { return false; }

    for (int yPos = position.getY(); yPos <= position.getY() + 1 + height; ++yPos) {
        const int radius = (yPos <= position.getY() + 3) ? 0 : 3;

        for (int l = position.getX() - radius; l <= position.getX() + radius; ++l) {
            for (int i1 = position.getZ() - radius; i1 <= position.getZ() + radius; ++i1) {
                if (yPos >= 0 && yPos < 256) {
                    const int blockId = worldIn->getBlockId(Pos3D(l, yPos, i1));
                    if (!lce::blocks::isAirOrLeavesBlock(blockId)) { return false; }
                } else {
                    return false;
                }
            }
        }
    }

    const int blockId = worldIn->getBlockId(position.down());

    if (blockId != DIRT_ID && blockId != GRASS_ID && blockId != MYCELIUM_ID) { return false; }

    const int maxY = position.getY() + height;
    const int startY = block == lce::BlocksInit::RED_MUSHROOM_BLOCK.getState() ? maxY - 3 : maxY;
    int radius;

    for (int yPos = startY; yPos <= maxY; ++yPos) {
        if (block == lce::BlocksInit::BROWN_MUSHROOM_BLOCK.getState()) {
            radius = 3;
        } else if (yPos < maxY) {
            radius = 2;
        } else {
            radius = 1;
        }

        c_int minX = position.getX() - radius;
        c_int maxX = position.getX() + radius;
        c_int minZ = position.getZ() - radius;
        c_int maxZ = position.getZ() + radius;

        c_int edgeMinX = minX + 1;
        c_int edgeMaxX = maxX - 1;
        c_int edgeMinZ = minZ + 1;
        c_int edgeMaxZ = maxZ - 1;

        for (int xPos = minX; xPos <= maxX; ++xPos) {
            for (int zPos = minZ; zPos <= maxZ; ++zPos) {
                auto hugeMushroomMetaData = BlockHugeMushroom::EnumType::CENTER;

                if (xPos == minX && zPos == minZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH_WEST;
                } else if (xPos == minX && zPos == maxZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH_WEST;
                } else if (xPos == maxX && zPos == minZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH_EAST;
                } else if (xPos == maxX && zPos == maxZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH_EAST;
                } else if (xPos == minX) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::WEST;
                } else if (xPos == maxX) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::EAST;
                } else if (zPos == minZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH;
                } else if (zPos == maxZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH;
                }

                if (block == lce::BlocksInit::BROWN_MUSHROOM_BLOCK.getState() || yPos < maxY) {
                    // Skip corners
                    if ((xPos == minX || xPos == maxX) && (zPos == minZ || zPos == maxZ)) continue;

                    if ((xPos == edgeMinX && zPos == minZ) || (zPos == edgeMinZ && xPos == minX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH_WEST;
                    }

                    if ((xPos == edgeMaxX && zPos == minZ) || (zPos == edgeMinZ && xPos == maxX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH_EAST;
                    }

                    if ((xPos == edgeMinX && zPos == maxZ) || (zPos == edgeMaxZ && xPos == minX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH_WEST;
                    }

                    if ((xPos == edgeMaxX && zPos == maxZ) || (zPos == edgeMaxZ && xPos == maxX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH_EAST;
                    }
                }

                if (hugeMushroomMetaData == BlockHugeMushroom::EnumType::CENTER && yPos < maxY) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::ALL_INSIDE;
                }

                if (position.getY() >= maxY - 1 || hugeMushroomMetaData != BlockHugeMushroom::EnumType::ALL_INSIDE) {
                    Pos3D blockPos(xPos, yPos, zPos);
                    if (!lce::blocks::isFullBlock(worldIn->getBlockId(blockPos))) {
                        worldIn->setBlockAndData(blockPos, block.getID(), static_cast<u8>(hugeMushroomMetaData));
                    }
                }
            }
        }
    }

    for (int y_off = 0; y_off < height; ++y_off) {
        if (lce::blocks::isFullBlock(!worldIn->getBlockId(position.up(y_off)))) {
            worldIn->setBlockAndData(position.up(y_off), block.getID(), static_cast<u8>(BlockHugeMushroom::EnumType::STEM));
        }
    }

    return true;
}
