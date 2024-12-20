#include "WorldGenTaiga1.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenTaiga1::generate(World * worldIn, RNG& rand, const Pos3D& position) const {
    const int height = rand.nextInt(5) + 7;
    int j = height - rand.nextInt(2) - 3;
    int k = height - j;
    int l = 1 + rand.nextInt(k + 1);

    if (position.getY() >= 1 && position.getY() + height <= 255) {
        bool flag = true;

        for (int i1 = position.getY(); i1 <= position.getY() + 1 + height && flag; ++i1) {
            int j1 = (i1 - position.getY() < j) ? 0 : l;

            for (int k1 = position.getX() - j1; k1 <= position.getX() + j1 && flag; ++k1) {
                for (int l1 = position.getZ() - j1; l1 <= position.getZ() + j1 && flag; ++l1) {
                    if (i1 >= 0 && i1 < 256) {
                        if (!canGrowInto(worldIn->getBlockId(Pos3D(k1, i1, l1)))) {
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

        int block = worldIn->getBlockId(position.down());

        if ((block == lce::blocks::ids::GRASS_ID || block == lce::blocks::ids::DIRT_ID) && position.getY() + height < 255) {
            setDirtAt(worldIn, position.down());
            int k2 = 0;

            for (int l2 = position.getY() + height; l2 >= position.getY() + j; --l2) {
                for (int j3 = position.getX() - k2; j3 <= position.getX() + k2; ++j3) {
                    int k3 = j3 - position.getX();

                    for (int i2 = position.getZ() - k2; i2 <= position.getZ() + k2; ++i2) {
                        int j2 = i2 - position.getZ();

                        if (std::abs(k3) != k2 || std::abs(j2) != k2 || k2 <= 0) {
                            Pos3D blockpos(j3, l2, i2);

                            if (!lce::blocks::ids::isFullBlock(worldIn->getBlockId(blockpos))) {
                                worldIn->setBlock(blockpos, &lce::blocks::BlocksInit::SPRUCE_LEAVES);
                            }
                        }
                    }
                }

                if (k2 >= 1 && l2 == position.getY() + j + 1) {
                    --k2;
                } else if (k2 < l) {
                    ++k2;
                }
            }

            for (int i3 = 0; i3 < height - 1; ++i3) {
                if (worldIn->isAirBlock(position.up(i3)) || lce::blocks::ids::isLeavesBlock(worldIn->getBlockId(position.up(i3)))) {
                    worldIn->setBlock(position.up(i3), &lce::blocks::BlocksInit::SPRUCE_WOOD);
                }
            }

            return true;
        }
    }
    return false;
}
