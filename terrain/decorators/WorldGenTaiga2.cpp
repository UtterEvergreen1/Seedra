#include "WorldGenTaiga2.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenTaiga2::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    const int height = rng.nextInt(4) + 6;
    const int j = 1 + rng.nextInt(2);
    const int k = height - j;
    const int l = 2 + rng.nextInt(2);

    if (pos.getY() >= 1 && pos.getY() + height + 1 <= 256) {
        bool flag = true;
        for (int i1 = pos.getY(); i1 <= pos.getY() + 1 + height && flag; ++i1) {
            int j1;

            if (i1 - pos.getY() < j) {
                j1 = 0;
            } else {
                j1 = l;
            }

            for (int k1 = pos.getX() - j1; k1 <= pos.getX() + j1 && flag; ++k1) {
                for (int l1 = pos.getZ() - j1; l1 <= pos.getZ() + j1 && flag; ++l1) {
                    if (i1 >= 0 && i1 < 256) {
                        const int blockId = worldIn->getBlockId(Pos3D(k1, i1, l1));
                        if (!lce::blocks::isAirOrLeavesBlock(blockId)) { flag = false; }
                    } else {
                        flag = false;
                    }
                }
            }
        }

        if (!flag) { return false; }

        const int block = worldIn->getBlockId(pos.down());

        if ((block == GRASS_ID || block == DIRT_ID || block == FARMLAND_ID) &&
            pos.getY() < 255 - height) {
            setDirtAt(worldIn, pos.down());
            const int i4 = rng.nextInt(3);
            const int fallen = placeTrunk(worldIn, rng, pos, height - i4, lce::BlocksInit::SPRUCE_WOOD.getState(), false);
            if (fallen != 0) {
                int i3 = rng.nextInt(2);
                int j3 = 1;
                int k3 = 0;

                for (int l3 = 0; l3 <= k; ++l3) {
                    const int j4 = pos.getY() + height - l3;

                    for (int i2 = pos.getX() - i3; i2 <= pos.getX() + i3; ++i2) {
                        int j2 = i2 - pos.getX();

                        for (int k2 = pos.getZ() - i3; k2 <= pos.getZ() + i3; ++k2) {
                            int l2 = k2 - pos.getZ();

                            if (std::abs(j2) != i3 || std::abs(l2) != i3 || i3 <= 0) {
                                Pos3D blockpos(i2, j4, k2);

                                const int blockId = worldIn->getBlockId(blockpos);
                                if (!isFullBlock(blockId)) {
                                    worldIn->setBlock(blockpos, lce::BlocksInit::SPRUCE_LEAVES.getState());
                                }
                            }
                        }
                    }

                    if (i3 >= j3) {
                        i3 = k3;
                        k3 = 1;
                        ++j3;

                        if (j3 > l) { j3 = l; }
                    } else {
                        ++i3;
                    }
                }
            }

            return true;
        }
    }

    return false;
}
