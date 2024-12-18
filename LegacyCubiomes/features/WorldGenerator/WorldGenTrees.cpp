#include "WorldGenTrees.hpp"
const lce::blocks::Block *WorldGenTrees::DEFAULT_TRUNK = &lce::blocks::OAK_WOOD;
const lce::blocks::Block *WorldGenTrees::DEFAULT_LEAF = &lce::blocks::OAK_LEAVES;

bool WorldGenTrees::generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const {
    const int height = rng.nextInt(3) + this->minTreeHeight;

    if (pos.getY() >= 1 && pos.getY() + height <= 255) {
        bool flag = true;
        for (int j = pos.getY(); j <= pos.getY() + 1 + height; ++j) {
            int k = 1;

            if (j == pos.getY()) {
                k = 0;
            }

            if (j >= pos.getY() + 1 + height - 2) {
                k = 2;
            }

            Pos3D mutablePosition;

            for (int l = pos.getX() - k; l <= pos.getX() + k && flag; ++l) {
                for (int i1 = pos.getZ() - k; i1 <= pos.getZ() + k && flag; ++i1) {
                    if (j >= 0 && j < 256) {
                        mutablePosition.setPos(l, j, i1);
                        if (!canGrowInto(worldIn->getBlockId(mutablePosition))) {
                            flag = false;
                        }
                    } else {
                        flag = false;
                    }
                }
            }
        }

        if (!flag) {
            return false;
        }

        const int belowBlockId = worldIn->getBlockId(pos.down());

        using namespace lce::blocks;
        if ((belowBlockId == ids::GRASS_ID || belowBlockId == ids::DIRT_ID || belowBlockId == ids::FARMLAND_ID)
            && pos.getY() < 256 - height - 1) {
            setDirtAt(worldIn, pos.down());
            //placeTrunk
            int fallen = placeTrunk(worldIn, rng, pos, height, this->metaWood, this->vinesGrow);
            if (fallen != 0) {
                int posi = pos.getY();
                int iVar2 = posi + height;
                int i3 = posi - 3 + height;
                if (i3 <= iVar2) {
                    int iVar5 = pos.getX();
                    int iVar9 = iVar2;
                    while (true) {
                        int i4 = iVar9 - iVar2;
                        int j1 = 1 - i4 / 2;

                        int iVar10 = iVar5 - j1;
                        iVar2 = iVar5 + j1;
                        if (iVar10 <= iVar2) {
                            posi = pos.getZ();
                            i3 = posi + j1;
                            do {
                                int iVar7 = posi - j1;
                                if (iVar7 <= i3) {
                                    int uVar1 = (iVar10 - iVar5) >> 31;
                                    if (((iVar10 - iVar5) ^ uVar1) - uVar1 == j1) {
                                        do {
                                            uVar1 = (iVar7 - posi) >> 31;
                                            if ((((iVar7 - posi) ^ uVar1) - uVar1 != j1) || (
                                                    rng.nextInt(2) != 0 && i4 != 0)) {
                                                Pos3D blockpos = {iVar10, iVar9, iVar7};
                                                int blockId = worldIn->getBlockId(blockpos);

                                                if (blockId == ids::AIR_ID || ids::isLeavesBlock(blockId) || blockId ==
                                                    ids::VINES_ID
                                                ) {
                                                    worldIn->setBlock(blockpos, this->metaLeaves);
                                                }
                                            }
                                            posi = pos.getZ();
                                            iVar7++;
                                            i3 = posi + j1;
                                        } while (iVar7 <= i3);
                                    } else {
                                        do {
                                            Pos3D blockpos = {iVar10, iVar9, iVar7};
                                            int blockId = worldIn->getBlockId(blockpos);

                                            if (blockId == ids::AIR_ID || ids::isLeavesBlock(blockId) || blockId ==
                                                ids::VINES_ID
                                            ) {
                                                worldIn->setBlock(blockpos, this->metaLeaves);
                                            }
                                            posi = pos.getZ();
                                            iVar7++;
                                            i3 = posi + j1;
                                        } while (iVar7 <= i3);
                                    }
                                    iVar5 = pos.getX();
                                    iVar2 = iVar5 + j1;
                                }
                                iVar10++;
                            } while (iVar10 <= iVar2);
                            posi = pos.getY();
                            i3 = posi - 3 + height;
                        }
                        iVar9--;
                        if (iVar9 < i3) break;
                        iVar2 = posi + height;
                    }
                }


                if (this->vinesGrow) {
                    for (int k3 = pos.getY() - 3 + height; k3 <= pos.getY() + height; ++k3) {
                        int j4 = k3 - (pos.getY() + height);
                        int k4 = 2 - j4 / 2;
                        Pos3D mutablePosition1;

                        for (int l4 = pos.getX() - k4; l4 <= pos.getX() + k4; ++l4) {
                            for (int i5 = pos.getZ() - k4; i5 <= pos.getZ() + k4; ++i5) {
                                mutablePosition1.setPos(l4, k3, i5);

                                if (int blockId = worldIn->getBlockId(mutablePosition1);
                                    ids::isLeavesBlock(blockId)) {
                                    Pos3D blockPos2 = mutablePosition1.west();
                                    Pos3D blockPos3 = mutablePosition1.east();
                                    Pos3D blockPos4 = mutablePosition1.north();
                                    Pos3D blockPos1 = mutablePosition1.south();

                                    if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos2)) {
                                        addHangingVine(worldIn, blockPos2, FACING::EAST);
                                    }

                                    if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos3)) {
                                        addHangingVine(worldIn, blockPos3, FACING::WEST);
                                    }

                                    if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos4)) {
                                        addHangingVine(worldIn, blockPos4, FACING::SOUTH);
                                    }

                                    if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos1)) {
                                        addHangingVine(worldIn, blockPos1, FACING::NORTH);
                                    }
                                }
                            }
                        }
                    }

                    if (rng.nextInt(5) == 0 && height > 5) {
                        for (int l3 = 0; l3 < 2; ++l3) {
                            for (FACING enumFacing: FACING_HORIZONTAL) {
                                if (rng.nextInt(4 - l3) == 0) {
                                    FACING enumFacing1 = getOppositeFacing(enumFacing);
                                    placeCocoa(worldIn, pos.add(getFrontOffsetX(enumFacing1), height - 5 + l3,
                                                                getFrontOffsetZ(enumFacing1)), enumFacing,
                                               rng.nextInt(3));
                                }
                            }
                        }
                    }
                }
            }
            return true;
        }
        return false;
    }
    return false;
}

void WorldGenTrees::placeCocoa(ChunkPrimer *worldIn, const Pos3D &pos, FACING side, int age) {
    worldIn->setBlockId(pos, lce::blocks::ids::VINES_ID); //TODO: set rotation data and age data
}

void WorldGenTrees::addHangingVine(ChunkPrimer *worldIn, const Pos3D &pos, const FACING direction) {
    addVine(worldIn, pos, direction);
    int i = 4;

    for (Pos3D blockPos = pos.down(); worldIn->getBlockId(blockPos) == lce::blocks::ids::AIR_ID && i > 0; --i) {
        addVine(worldIn, blockPos, direction);
        blockPos = blockPos.down();
    }
}
