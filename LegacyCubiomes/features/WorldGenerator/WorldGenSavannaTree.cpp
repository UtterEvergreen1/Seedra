#include "WorldGenSavannaTree.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"

using namespace lce::blocks;


bool WorldGenSavannaTree::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    c_int height = rand.nextInt(3) + rand.nextInt(3) + 5;

    if (position.getY() >= 1 && position.getY() + height <= 255) {
        bool flag = true;
        for (int j = position.getY(); j <= position.getY() + 1 + height; ++j) {
            int k = 1;

            if (j == position.getY()) { k = 0; }

            if (j >= position.getY() + 1 + height - 2) { k = 2; }

            Pos3D mutablePos;

            for (int l = position.getX() - k; l <= position.getX() + k && flag; ++l) {
                for (int i1 = position.getZ() - k; i1 <= position.getZ() + k && flag; ++i1) {
                    if (j >= 0 && j < 256) {
                        mutablePos.setPos(l, j, i1);
                        if (!canGrowInto(worldIn->getBlockId(mutablePos))) { flag = false; }
                    } else {
                        flag = false;
                    }
                }
            }
        }

        if (!flag) { return false; }

        c_int block = worldIn->getBlockId(position.down());

        if ((block == ids::GRASS_ID || block == ids::DIRT_ID) && position.getY() + height < 255) {
            setDirtAt(worldIn, position.down());
            const enumFacing enumFacing0 = FACING_HORIZONTAL[rand.nextInt(4)];
            c_int k2 = height - rand.nextInt(4) - 1;
            int l2 = 3 - rand.nextInt(3);
            int i3 = position.getX();

            int j1 = position.getZ();
            int k1 = 0;

            for (int l1 = 0; l1 < height; ++l1) {
                c_int i2 = position.getY() + l1;

                if (l1 >= k2 && l2 > 0) {
                    i3 += getFrontOffsetX(enumFacing0);
                    j1 += getFrontOffsetZ(enumFacing0);
                    --l2;
                }

                Pos3D blockPos(i3, i2, j1);
                c_int material = worldIn->getBlockId(blockPos);

                if (ids::isAirOrLeavesBlock(material)) {
                    placeLogAt(worldIn, blockPos);
                    k1 = i2;
                }
            }

            Pos3D blockPos2(i3, k1, j1);

            for (int j3 = -3; j3 <= 3; ++j3) {
                for (int i4 = -3; i4 <= 3; ++i4) {
                    if (std::abs(j3) != 3 || std::abs(i4) != 3) { placeLeafAt(worldIn, blockPos2.add(j3, 0, i4)); }
                }
            }

            blockPos2 = blockPos2.up();

            for (int k3 = -1; k3 <= 1; ++k3) {
                for (int j4 = -1; j4 <= 1; ++j4) { placeLeafAt(worldIn, blockPos2.add(k3, 0, j4)); }
            }

            placeLeafAt(worldIn, blockPos2.east(2));
            placeLeafAt(worldIn, blockPos2.west(2));
            placeLeafAt(worldIn, blockPos2.south(2));
            placeLeafAt(worldIn, blockPos2.north(2));
            i3 = position.getX();
            j1 = position.getZ();
            const enumFacing enumFacing1 = FACING_HORIZONTAL[rand.nextInt(4)];

            if (enumFacing1 != enumFacing0) {
                c_int l3 = k2 - rand.nextInt(2) - 1;
                int k4 = 1 + rand.nextInt(3);
                k1 = 0;

                for (int l4 = l3; l4 < height && k4 > 0; --k4) {
                    if (l4 >= 1) {
                        c_int j2 = position.getY() + l4;
                        i3 += getFrontOffsetX(enumFacing1);
                        j1 += getFrontOffsetZ(enumFacing1);
                        Pos3D blockPos1(i3, j2, j1);
                        c_int material1 = worldIn->getBlockId(blockPos1);

                        if (ids::isAirOrLeavesBlock(material1)) {
                            placeLogAt(worldIn, blockPos1);
                            k1 = j2;
                        }
                    }

                    ++l4;
                }

                if (k1 > 0) {
                    Pos3D blockPos3(i3, k1, j1);

                    for (int i5 = -2; i5 <= 2; ++i5) {
                        for (int k5 = -2; k5 <= 2; ++k5) {
                            if (std::abs(i5) != 2 || std::abs(k5) != 2) {
                                placeLeafAt(worldIn, blockPos3.add(i5, 0, k5));
                            }
                        }
                    }

                    blockPos3 = blockPos3.up();

                    for (int j5 = -1; j5 <= 1; ++j5) {
                        for (int l5 = -1; l5 <= 1; ++l5) { placeLeafAt(worldIn, blockPos3.add(j5, 0, l5)); }
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void WorldGenSavannaTree::placeLogAt(World* worldIn, const Pos3D& pos) {
    worldIn->setBlock(pos, &BlocksInit::ACACIA_WOOD);
}

void WorldGenSavannaTree::placeLeafAt(World* worldIn, const Pos3D& pos) {
    if (ids::isAirOrLeavesBlock(worldIn->getBlockId(pos))) { worldIn->setBlock(pos, &BlocksInit::ACACIA_LEAVES); }
}
