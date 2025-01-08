#include "WorldGenCanopyTree.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenCanopyTree::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    const int height = rand.nextInt(3) + rand.nextInt(2) + 6;
    const int j = position.getX();
    const int k = position.getY();
    const int l = position.getZ();

    if (k >= 1 && k + height + 1 < 256) {
        const Pos3D blockPos = position.down();
        const int block = worldIn->getBlockId(blockPos);

        if (block != lce::blocks::ids::GRASS_ID && block != lce::blocks::ids::DIRT_ID) { return false; }
        if (!placeTreeOfHeight(worldIn, position, height)) { return false; }

        setDirtAt(worldIn, blockPos);
        setDirtAt(worldIn, blockPos.east());
        setDirtAt(worldIn, blockPos.south());
        setDirtAt(worldIn, blockPos.south().east());
        const FACING facing = FACING_HORIZONTAL[rand.nextInt(4)];
        const int i1 = height - rand.nextInt(4);
        int j1 = 2 - rand.nextInt(3);
        int k1 = j;
        int l1 = l;
        const int i2 = k + height - 1;

        for (int j2 = 0; j2 < height; ++j2) {
            if (j2 >= i1 && j1 > 0) {
                k1 += getFrontOffsetX(facing);
                l1 += getFrontOffsetZ(facing);
                --j1;
            }

            const int k2 = k + j2;
            Pos3D blockpos1(k1, k2, l1);
            const int material = worldIn->getBlockId(blockpos1);

            if (lce::blocks::ids::isAirOrLeavesBlock(material)) {
                placeLogAt(worldIn, blockpos1);
                placeLogAt(worldIn, blockpos1.east());
                placeLogAt(worldIn, blockpos1.south());
                placeLogAt(worldIn, blockpos1.east().south());
            }
        }

        for (int i3 = -2; i3 <= 0; ++i3) {
            for (int l3 = -2; l3 <= 0; ++l3) {
                int k4 = -1;
                placeLeafAt(worldIn, k1 + i3, i2 + k4, l1 + l3);
                placeLeafAt(worldIn, 1 + k1 - i3, i2 + k4, l1 + l3);
                placeLeafAt(worldIn, k1 + i3, i2 + k4, 1 + l1 - l3);
                placeLeafAt(worldIn, 1 + k1 - i3, i2 + k4, 1 + l1 - l3);

                if ((i3 > -2 || l3 > -1) && (i3 != -1 || l3 != -2)) {
                    k4 = 1;
                    placeLeafAt(worldIn, k1 + i3, i2 + k4, l1 + l3);
                    placeLeafAt(worldIn, 1 + k1 - i3, i2 + k4, l1 + l3);
                    placeLeafAt(worldIn, k1 + i3, i2 + k4, 1 + l1 - l3);
                    placeLeafAt(worldIn, 1 + k1 - i3, i2 + k4, 1 + l1 - l3);
                }
            }
        }

        if (rand.nextBoolean()) {
            placeLeafAt(worldIn, k1, i2 + 2, l1);
            placeLeafAt(worldIn, k1 + 1, i2 + 2, l1);
            placeLeafAt(worldIn, k1 + 1, i2 + 2, l1 + 1);
            placeLeafAt(worldIn, k1, i2 + 2, l1 + 1);
        }

        for (int j3 = -3; j3 <= 4; ++j3) {
            for (int i4 = -3; i4 <= 4; ++i4) {
                if ((j3 != -3 || i4 != -3) && (j3 != -3 || i4 != 4) && (j3 != 4 || i4 != -3) && (j3 != 4 || i4 != 4) &&
                    (std::abs(j3) < 3 || std::abs(i4) < 3)) {
                    placeLeafAt(worldIn, k1 + j3, i2, l1 + i4);
                }
            }
        }

        for (int k3 = -1; k3 <= 2; ++k3) {
            for (int j4 = -1; j4 <= 2; ++j4) {
                if ((k3 < 0 || k3 > 1 || j4 < 0 || j4 > 1) && rand.nextInt(3) <= 0) {
                    const int l4 = rand.nextInt(3) + 2;

                    for (int i5 = 0; i5 < l4; ++i5) { placeLogAt(worldIn, {j + k3, i2 - i5 - 1, l + j4}); }

                    for (int j5 = -1; j5 <= 1; ++j5) {
                        for (int l2 = -1; l2 <= 1; ++l2) { placeLeafAt(worldIn, k1 + k3 + j5, i2, l1 + j4 + l2); }
                    }

                    for (int k5 = -2; k5 <= 2; ++k5) {
                        for (int l5 = -2; l5 <= 2; ++l5) {
                            if (std::abs(k5) != 2 || std::abs(l5) != 2) {
                                placeLeafAt(worldIn, k1 + k3 + k5, i2 - 1, l1 + j4 + l5);
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

bool WorldGenCanopyTree::placeTreeOfHeight(World* worldIn, const Pos3D& pos, const int height) {
    c_int x = pos.getX();
    c_int y = pos.getY();
    c_int z = pos.getZ();
    Pos3D mutablePos;

    for (int l = 0; l <= height + 1; ++l) {
        int i1 = 1;

        if (l == 0) { i1 = 0; }

        if (l >= height - 1) { i1 = 2; }

        for (int j1 = -i1; j1 <= i1; ++j1) {
            for (int k1 = -i1; k1 <= i1; ++k1) {
                mutablePos.setPos(x + j1, y + l, z + k1);
                if (!canGrowInto(worldIn->getBlockId(mutablePos))) { return false; }
            }
        }
    }

    return true;
}

void WorldGenCanopyTree::placeLogAt(World* worldIn, const Pos3D& pos) {
    if (canGrowInto(worldIn->getBlockId(pos))) { worldIn->setBlock(pos, &lce::blocks::BlocksInit::DARK_OAK_WOOD); }
}

void WorldGenCanopyTree::placeLeafAt(World* worldIn, const int x, const int y, const int z) {
    const Pos3D blockPos(x, y, z);
    const int material = worldIn->getBlockId(blockPos);

    if (material == lce::blocks::ids::AIR_ID) {
        worldIn->setBlock(blockPos, &lce::blocks::BlocksInit::DARK_OAK_LEAVES);
    }
}
