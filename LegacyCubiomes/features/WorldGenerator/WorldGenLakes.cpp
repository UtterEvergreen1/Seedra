#include "WorldGenLakes.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenLakes::generate(World *worldIn, RNG &rng, const Pos3D &pos) const {
    Pos3D position = pos;
    for (position = position.add(-8, 0, -8); position.getY() > 5 && worldIn->isAirBlock(position);
         position = position.down()) {
        ;
    }

    if (position.getY() <= 4) {
        return false;
    }

    position = position.down(4);
    bool aboolean[16 * 16 * 8] = {};
    int i = rng.nextInt(4) + 4;

    for (int j = 0; j < i; ++j) {
        const double d0 = rng.nextDouble() * 6.0 + 3.0;
        const double d1 = rng.nextDouble() * 4.0 + 2.0;
        const double d2 = rng.nextDouble() * 6.0 + 3.0;
        const double d3 = rng.nextDouble() * (14.0 - d0) + 1.0 + d0 / 2.0;
        const double d4 = rng.nextDouble() * (4.0 - d1) + 2.0 + d1 / 2.0;
        const double d5 = rng.nextDouble() * (14.0 - d2) + 1.0 + d2 / 2.0;

        for (int l = 1; l < 15; ++l) {
            for (int i1 = 1; i1 < 15; ++i1) {
                for (int j1 = 1; j1 < 7; ++j1) {
                    const double d6 = ((double) l - d3) / (d0 / 2.0);
                    const double d7 = ((double) j1 - d4) / (d1 / 2.0);
                    const double d8 = ((double) i1 - d5) / (d2 / 2.0);
                    const double d9 = d6 * d6 + d7 * d7 + d8 * d8;

                    if (d9 < 1.0) {
                        aboolean[(l * 16 + i1) * 8 + j1] = true;
                    }
                }
            }
        }
    }

    for (int k1 = 0; k1 < 16; ++k1) {
        for (int l2 = 0; l2 < 16; ++l2) {
            for (int k = 0; k < 8; ++k) {
                const bool flag = !aboolean[(k1 * 16 + l2) * 8 + k] &&
                                  (k1 < 15 && aboolean[((k1 + 1) * 16 + l2) * 8 + k] || k1 > 0 && aboolean[
                                       ((k1 - 1) * 16 + l2) * 8 + k] || l2 < 15 && aboolean[(k1 * 16 + l2 + 1) * 8 + k]
                                   || l2 > 0 && aboolean[(k1 * 16 + (l2 - 1)) * 8 + k] || k < 7 && aboolean[
                                       (k1 * 16 + l2) * 8 + k + 1] || k > 0 && aboolean[(k1 * 16 + l2) * 8 + (k - 1)]);

                if (flag) {
                    Pos3D materialPos = position.add(k1, k, l2);

                    if (k >= 4 && lce::blocks::ids::isLiquidBlock(worldIn->getBlockId(materialPos))) {
                        return false;
                    }

                    if (k < 4 && !lce::blocks::ids::isSolidBlock(worldIn->getBlockId(materialPos))
                        && worldIn->getBlockId(position.add(k1, k, l2)) != this->block->getID()) {
                        return false;
                    }
                }
            }
        }
    }

    for (int l1 = 0; l1 < 16; ++l1) {
        for (int i3 = 0; i3 < 16; ++i3) {
            for (int i4 = 0; i4 < 8; ++i4) {
                if (aboolean[(l1 * 16 + i3) * 8 + i4]) {
                    worldIn->setBlock(position.add(l1, i4, i3),
                                      i4 >= 4 ? &lce::blocks::BlocksInit::AIR : this->block);
                }
            }
        }
    }

    for (int i2 = 0; i2 < 16; ++i2) {
        for (int j3 = 0; j3 < 16; ++j3) {
            for (int j4 = 4; j4 < 8; ++j4) {
                if (aboolean[(i2 * 16 + j3) * 8 + j4]) {
                    Pos3D blockPos = position.add(i2, j4 - 1, j3);

                    if (worldIn->getBlockId(blockPos) == lce::blocks::ids::DIRT_ID && !lce::blocks::ids::isFullyOpaqueBlock(worldIn->getBlockId(blockPos.up()))) {
                        Biome *biome = Biome::getBiomeForId(g->getBiomeAt(1, blockPos.convert2D()));

                        if (biome->topBlock == &lce::blocks::BlocksInit::MYCELIUM) {
                            worldIn->setBlock(blockPos, lce::blocks::ids::MYCELIUM_ID);
                        } else {
                            worldIn->setBlock(blockPos, lce::blocks::ids::GRASS_ID);
                        }
                    }
                }
            }
        }
    }

    if (this->block == &lce::blocks::BlocksInit::STILL_LAVA) {
        for (int j2 = 0; j2 < 16; ++j2) {
            for (int k3 = 0; k3 < 16; ++k3) {
                for (int k4 = 0; k4 < 8; ++k4) {
                    bool flag1 = !aboolean[(j2 * 16 + k3) * 8 + k4] && (
                                     j2 < 15 && aboolean[((j2 + 1) * 16 + k3) * 8 + k4] || j2 > 0 && aboolean[
                                         ((j2 - 1) * 16 + k3) * 8 + k4] || k3 < 15 && aboolean[
                                         (j2 * 16 + k3 + 1) * 8 + k4] || k3 > 0 && aboolean[
                                         (j2 * 16 + (k3 - 1)) * 8 + k4] || k4 < 7 && aboolean[
                                         (j2 * 16 + k3) * 8 + k4 + 1] || k4 > 0 && aboolean[
                                         (j2 * 16 + k3) * 8 + (k4 - 1)]);

                    if (flag1 && (k4 < 4 || rng.nextInt(2) != 0)
                        && lce::blocks::ids::isSolidBlock(worldIn->getBlockId(position.add(j2, k4, k3)))) {
                        worldIn->setBlock(position.add(j2, k4, k3), lce::blocks::ids::STONE_ID);
                    }
                }
            }
        }
    }

    if (this->block == &lce::blocks::BlocksInit::STILL_WATER) {
        for (int k2 = 0; k2 < 16; ++k2) {
            for (int l3 = 0; l3 < 16; ++l3) {
                if (worldIn->canBlockFreezeWater(position.add(k2, 4, l3))) {
                    worldIn->setBlock(position.add(k2, 4, l3), lce::blocks::ids::ICE_ID);
                }
            }
        }
    }

    return true;
}
