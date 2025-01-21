#include "WorldGenSwamp.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenSwamp::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    const int height = rand.nextInt(4) + 5;
    Pos3D pos = position;

    while (isWaterMaterial(worldIn->getBlockId(pos.down()))) { pos = pos.down(); }

    if (pos.getY() >= 1 && pos.getY() + height + 1 <= 256) {
        bool flag = true;

        for (int j = pos.getY(); j <= pos.getY() + 1 + height; ++j) {
            int k = 1;

            if (j == pos.getY()) { k = 0; }

            if (j >= pos.getY() + 1 + height - 2) { k = 3; }

            for (int l = pos.getX() - k; l <= pos.getX() + k && flag; ++l) {
                for (int i1 = pos.getZ() - k; i1 <= pos.getZ() + k && flag; ++i1) {
                    if (j >= 0 && j < 256) {
                        const int blockId = worldIn->getBlockId(Pos3D(l, j, i1));
                        if (blockId != AIR_ID && !isLeavesBlock(blockId)) {
                            if (!isWaterMaterial(blockId) || j > pos.getY()) { flag = false; }
                        }
                    } else {
                        flag = false;
                    }
                }
            }
        }

        if (!flag) { return false; }
        const int block1 = worldIn->getBlockId(pos.down());

        if ((block1 == GRASS_ID || block1 == DIRT_ID) && pos.getY() < 256 - height - 1) {
            setDirtAt(worldIn, pos.down());

            for (int k1 = pos.getY() - 3 + height; k1 <= pos.getY() + height; ++k1) {
                const int j2 = k1 - (pos.getY() + height);
                int l2 = 2 - j2 / 2;

                for (int j3 = pos.getX() - l2; j3 <= pos.getX() + l2; ++j3) {
                    int k3 = j3 - pos.getX();

                    for (int i4 = pos.getZ() - l2; i4 <= pos.getZ() + l2; ++i4) {
                        int j1 = i4 - pos.getZ();

                        if (std::abs(k3) != l2 || std::abs(j1) != l2 || rand.nextInt(2) != 0 && j2 != 0) {
                            Pos3D blockPos(j3, k1, i4);

                            if (!isFullBlock(worldIn->getBlockId(blockPos))) {
                                worldIn->setBlock(blockPos, &lce::BlocksInit::OAK_LEAVES);
                            }
                        }
                    }
                }
            }

            for (int l1 = 0; l1 < height; ++l1) {
                const int blockId = worldIn->getBlockId(pos.up(l1));
                if (lce::blocks::isAirOrLeavesBlock(blockId) || isWaterMaterial(blockId)) {
                    worldIn->setBlock(pos.up(l1), &lce::BlocksInit::OAK_WOOD);
                }
            }

            for (int i2 = pos.getY() - 3 + height; i2 <= pos.getY() + height; ++i2) {
                const int k2 = i2 - (pos.getY() + height);
                const int i3 = 2 - k2 / 2;

                for (int l3 = pos.getX() - i3; l3 <= pos.getX() + i3; ++l3) {
                    for (int j4 = pos.getZ() - i3; j4 <= pos.getZ() + i3; ++j4) {
                        Pos3D blockPos(l3, i2, j4);

                        if (isLeavesBlock(worldIn->getBlockId(blockPos))) {
                            if (rand.nextInt(4) == 0 && worldIn->isAirBlock(blockPos.west())) {
                                addVine(worldIn, blockPos.west(), EnumFacing::EAST);
                            }

                            if (rand.nextInt(4) == 0 && worldIn->isAirBlock(blockPos.east())) {
                                addVine(worldIn, blockPos.east(), EnumFacing::WEST);
                            }

                            if (rand.nextInt(4) == 0 && worldIn->isAirBlock(blockPos.north())) {
                                addVine(worldIn, blockPos.north(), EnumFacing::SOUTH);
                            }

                            if (rand.nextInt(4) == 0 && worldIn->isAirBlock(blockPos.south())) {
                                addVine(worldIn, blockPos.south(), EnumFacing::NORTH);
                            }
                        }
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void WorldGenSwamp::addVine(World* worldIn, const Pos3D& pos, const EnumFacing facing) {
    c_int vineMetaData = states::Vine::withProperty(facing);
    worldIn->setBlock(pos, VINES_ID, vineMetaData);

    int i = 4;
    for (Pos3D blockPos = pos.down(); worldIn->isAirBlock(blockPos) && i > 0; --i) {
        worldIn->setBlock(pos, VINES_ID, vineMetaData);
        blockPos = blockPos.down();
    }
}
