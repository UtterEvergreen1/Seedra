#include "WorldGenBigMushroom.hpp"

#include "lce/blocks/blocks.hpp"

bool WorldGenBigMushroom::generate(World * worldIn, RNG &rand, const Pos3D &position) const {
    using namespace lce::blocks;
    const Block *block = this->mushroomType;

    if (block == nullptr) {
        block = rand.nextBoolean() ? &BlocksInit::BROWN_MUSHROOM_BLOCK : &BlocksInit::RED_MUSHROOM_BLOCK;
    }

    int height = rand.nextInt(3) + 4;

    if (rand.nextInt(12) == 0) {
        height *= 2;
    }

    if (position.getY() < 1 || position.getY() + height >= 255) {
        return false;
    }

    for (int yPos = position.getY(); yPos <= position.getY() + 1 + height; ++yPos) {
        const int radius = (yPos <= position.getY() + 3) ? 0 : 3;

        for (int l = position.getX() - radius; l <= position.getX() + radius; ++l) {
            for (int i1 = position.getZ() - radius; i1 <= position.getZ() + radius; ++i1) {
                if (yPos >= 0 && yPos < 256) {
                    int blockId = worldIn->getBlockId(Pos3D(l, yPos, i1));
                    if (blockId != ids::AIR_ID && !ids::isLeavesBlock(blockId)) {
                        return false;
                    }
                } else {
                    return false;
                }
            }
        }
    }

    const int blockId = worldIn->getBlockId(position.down());

    if (blockId != ids::DIRT_ID && blockId != ids::GRASS_ID && blockId != ids::MYCELIUM_ID) {
        return false;
    }

    const int maxY = position.getY() + height;
    const int startY = block == &BlocksInit::RED_MUSHROOM_BLOCK ? maxY - 3 : maxY;

    for (int yPos = startY; yPos <= maxY; ++yPos) {
        int radius = 1;

        if (block == &BlocksInit::BROWN_MUSHROOM_BLOCK) {
            radius = 3;
        } else if (yPos < maxY) {
            ++radius;
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

                if (xPos == minX) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::WEST;
                } else if (xPos == maxX) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::EAST;
                }

                if (zPos == minZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH;
                } else if (zPos == maxZ) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH;
                }

                if (block == &BlocksInit::BROWN_MUSHROOM_BLOCK || yPos < maxY) {
                    // Skip corners
                    if ((xPos == minX || xPos == maxX) && (zPos == minZ || zPos == maxZ))
                        continue;

                    if ((xPos == edgeMinX && zPos == minZ) || (zPos == edgeMinZ && xPos == minX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH_WEST;
                    }

                    if ((xPos == edgeMaxX && zPos == minZ) || (zPos == edgeMinZ && xPos == maxX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::NORTH_EAST;
                    }

                    if ((xPos == edgeMinX && zPos == maxZ) || (xPos == edgeMaxZ && xPos == minX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH_WEST;
                    }

                    if ((xPos == edgeMaxX && zPos == maxZ) || (zPos == edgeMaxZ && xPos == maxX)) {
                        hugeMushroomMetaData = BlockHugeMushroom::EnumType::SOUTH_EAST;
                    }
                }

                if (hugeMushroomMetaData == BlockHugeMushroom::EnumType::CENTER && yPos < maxY) {
                    hugeMushroomMetaData = BlockHugeMushroom::EnumType::ALL_INSIDE;
                }

                if (position.getY() >= maxY - 1 || hugeMushroomMetaData !=
                    BlockHugeMushroom::EnumType::ALL_INSIDE) {
                    Pos3D blockPos(xPos, yPos, zPos);
                    if (!ids::isFullBlock(worldIn->getBlockId(blockPos))) {
                        worldIn->setBlock(position.up(yPos), block->getID(), (int) hugeMushroomMetaData);
                    }
                }
            }
        }
    }

    for (int y_off = 0; y_off < height; ++y_off) {
        if (ids::isFullBlock(!worldIn->getBlockId(position.up(y_off)))) {
            worldIn->setBlock(position.up(y_off), block->getID(), (int) BlockHugeMushroom::EnumType::STEM);
        }
    }

    return true;
}
