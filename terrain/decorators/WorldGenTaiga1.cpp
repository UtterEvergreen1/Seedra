#include "WorldGenTaiga1.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenTaiga1::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    const int height = rng.nextInt(5) + 7;
    const int j = height - rng.nextInt(2) - 3;
    const int k = height - j;
    const int l = 1 + rng.nextInt(k + 1);

    if (pos.getY() >= 1 && pos.getY() + height <= 255) {
        bool flag = true;

        for (int i1 = pos.getY(); i1 <= pos.getY() + 1 + height && flag; ++i1) {
            const int j1 = (i1 - pos.getY() < j) ? 0 : l;

            for (int k1 = pos.getX() - j1; k1 <= pos.getX() + j1 && flag; ++k1) {
                for (int l1 = pos.getZ() - j1; l1 <= pos.getZ() + j1 && flag; ++l1) {
                    if (i1 >= 0 && i1 < 256) {
                        if (!canGrowInto(worldIn->getBlockId(Pos3D(k1, i1, l1)))) { flag = false; }
                    } else {
                        flag = false;
                    }
                }
            }
        }

        if (!flag) { return false; }

        const int block = worldIn->getBlockId(pos.down());

        if ((block == GRASS_ID || block == DIRT_ID) && pos.getY() + height < 255) {
            setDirtAt(worldIn, pos.down());
            int k2 = 0;

            for (int l2 = pos.getY() + height; l2 >= pos.getY() + j; --l2) {
                for (int j3 = pos.getX() - k2; j3 <= pos.getX() + k2; ++j3) {
                    int k3 = j3 - pos.getX();

                    for (int i2 = pos.getZ() - k2; i2 <= pos.getZ() + k2; ++i2) {
                        int j2 = i2 - pos.getZ();

                        if (std::abs(k3) != k2 || std::abs(j2) != k2 || k2 <= 0) {
                            Pos3D blockpos(j3, l2, i2);

                            if (!isFullBlock(worldIn->getBlockId(blockpos))) {
                                worldIn->setBlock(blockpos, lce::BlocksInit::SPRUCE_LEAVES.getState());
                            }
                        }
                    }
                }

                if (k2 >= 1 && l2 == pos.getY() + j + 1) {
                    --k2;
                } else if (k2 < l) {
                    ++k2;
                }
            }

            for (int i3 = 0; i3 < height - 1; ++i3) {
                if (worldIn->isAirBlock(pos.up(i3)) ||
                    isLeavesBlock(worldIn->getBlockId(pos.up(i3)))) {
                    worldIn->setBlock(pos.up(i3), lce::BlocksInit::SPRUCE_WOOD.getState());
                }
            }

            return true;
        }
    }
    return false;
}
