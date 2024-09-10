#pragma once

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/cubiomes/BiomeID.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "lce/blocks/block.hpp"
#include "lce/blocks/blocks.hpp"

namespace Features {


    class World {
    public:
        MU ND static bool isAirBlock(MU const Pos3D& position) { return false; }
        MU ND static lce::blocks::Block const* getBlock(MU const Pos3D& position) { return &lce::blocks::AIR; }
        void setBlock(MU const Pos3D& position, MU lce::blocks::Block const* block, MU int num) {}
        MU ND static bool canBlockFreezeWater(MU const Pos3D& position) { return false; }
        MU ND Biome* getBiome(MU const Pos3D& position) { return nullptr; }
    };


    class WorldGenLakes {
        lce::blocks::Block const* block;

    public:
        MU explicit WorldGenLakes(lce::blocks::Block const* blockIn) {
            this->block = blockIn;
        }

        bool generate(World worldIn, RNG rng, Pos3D position)  {
            for (position=position.add(-8, 0, -8); position.getY() > 5 && worldIn.isAirBlock(position);
                 position=position.down()) {
                ;
            }

            bool aboolean[16 * 16 * 8] = {};

            if (position.getY() <= 4) {
                return false;
            } else {
                position=position.down(4);
                int i=rng.nextInt(4) + 4;

                for (int j=0; j < i; ++j) {
                    double d0=rng.nextDouble() * 6.0 + 3.0;
                    double d1=rng.nextDouble() * 4.0 + 2.0;
                    double d2=rng.nextDouble() * 6.0 + 3.0;
                    double d3=rng.nextDouble() * (16.0 - d0 - 2.0) + 1.0 + d0 / 2.0;
                    double d4=rng.nextDouble() * (8.0 - d1 - 4.0) + 2.0 + d1 / 2.0;
                    double d5=rng.nextDouble() * (16.0 - d2 - 2.0) + 1.0 + d2 / 2.0;

                    for (int l=1; l < 15; ++l) {
                        for (int i1=1; i1 < 15; ++i1) {
                            for (int j1=1; j1 < 7; ++j1) {
                                double d6=((double) l - d3) / (d0 / 2.0);
                                double d7=((double) j1 - d4) / (d1 / 2.0);
                                double d8=((double) i1 - d5) / (d2 / 2.0);
                                double d9=d6 * d6 + d7 * d7 + d8 * d8;

                                if (d9 < 1.0) {
                                    aboolean[(l * 16 + i1) * 8 + j1]=true;
                                }
                            }
                        }
                    }
                }

                for (int k1=0; k1 < 16; ++k1) {
                    for (int l2=0; l2 < 16; ++l2) {
                        for (int k=0; k < 8; ++k) {
                            bool flag=!aboolean[(k1 * 16 + l2) * 8 + k] && (k1 < 15
                                    && aboolean[((k1 + 1) * 16 + l2) * 8 + k] || k1 > 0
                                    && aboolean[((k1 - 1) * 16 + l2) * 8 + k] || l2 < 15
                                    && aboolean[(k1 * 16 + l2 + 1) * 8 + k] || l2 > 0
                                    && aboolean[(k1 * 16 + (l2 - 1)) * 8 + k] || k < 7
                                    && aboolean[(k1 * 16 + l2) * 8 + k + 1] || k > 0
                                    && aboolean[(k1 * 16 + l2) * 8 + (k - 1)]);

                            if (flag) {
                                // Material material=worldIn.getBlock(position.add(k1, k, l2)).getMaterial();

                                if (k >= 4 /*&& material.isLiquid()*/) {
                                    return false;
                                }

                                if (k < 4 /*&& !material.isSolid() */
                                    && worldIn.getBlock(position.add(k1, k, l2)) != this->block) {
                                    return false;
                                }
                            }
                        }
                    }
                }

                for (int l1=0; l1 < 16; ++l1) {
                    for (int i3=0; i3 < 16; ++i3) {
                        for (int i4=0; i4 < 8; ++i4) {
                            if (aboolean[(l1 * 16 + i3) * 8 + i4]) {
                                worldIn.setBlock(position.add(l1, i4, i3), i4 >= 4 ? &lce::blocks::AIR : this->block, 2);
                            }
                        }
                    }
                }

                for (int i2=0; i2 < 16; ++i2) {
                    for (int j3=0; j3 < 16; ++j3) {
                        for (int j4=4; j4 < 8; ++j4) {
                            if (aboolean[(i2 * 16 + j3) * 8 + j4]) {
                                Pos3D blockPos = position.add(i2, j4 - 1, j3);

                                if (worldIn.getBlock(blockPos) == &lce::blocks::DIRT /*&&
                                    worldIn.getLightFor(EnumSkyBlock.SKY, position.add(i2, j4, j3)) > 0*/) {
                                    Biome* biome=worldIn.getBiome(blockPos);

                                    if (biome->topBlock == &lce::blocks::MYCELIUM) {
                                        worldIn.setBlock(blockPos, &lce::blocks::MYCELIUM, 2);
                                    } else {
                                        worldIn.setBlock(blockPos, &lce::blocks::GRASS, 2);
                                    }
                                }
                            }
                        }
                    }
                }

                if (this->block == &lce::blocks::FLOWING_LAVA || this->block == &lce::blocks::STILL_LAVA) {
                    for (int j2=0; j2 < 16; ++j2) {
                        for (int k3=0; k3 < 16; ++k3) {
                            for (int k4=0; k4 < 8; ++k4) {
                                bool flag1=!aboolean[(j2 * 16 + k3) * 8 + k4] && (j2 < 15 && aboolean[((j2 + 1) * 16 + k3) * 8 + k4] || j2 > 0 && aboolean[((j2 - 1) * 16 + k3) * 8 + k4] || k3 < 15 && aboolean[(j2 * 16 + k3 + 1) * 8 + k4] || k3 > 0 && aboolean[(j2 * 16 + (k3 - 1)) * 8 + k4] || k4 < 7 && aboolean[(j2 * 16 + k3) * 8 + k4 + 1] || k4 > 0 && aboolean[(j2 * 16 + k3) * 8 + (k4 - 1)]);

                                if (flag1 && (k4 < 4 || rng.nextInt(2) != 0)
                                    /*&& worldIn.getBlock(position.add(j2, k4, k3)).getMaterial().isSolid()*/) {
                                    worldIn.setBlock(position.add(j2, k4, k3), &lce::blocks::STONE, 2);
                                }
                            }
                        }
                    }
                }

                if (this->block == &lce::blocks::FLOWING_WATER || this->block == &lce::blocks::STILL_WATER) {
                    for (int k2=0; k2 < 16; ++k2) {
                        for (int l3=0; l3 < 16; ++l3) {
                            int l4=4;

                            if (worldIn.canBlockFreezeWater(position.add(k2, 4, l3))) {
                                worldIn.setBlock(position.add(k2, 4, l3), &lce::blocks::ICE, 2);
                            }
                        }
                    }
                }

                return true;
            }
        }









    };







    Pos3D waterLake(Generator* g, int chunkX, int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if (biomeAt == desert || biomeAt == desert_hills || rng.nextInt(4) != 0) return {0, 0, 0};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        return {xPos, yPos, zPos};
    }

    Pos3D lavaLake(Generator* g, int chunkX, int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if ((biomeAt != desert && biomeAt != desert_hills) && rng.nextInt(4) == 0) // water lake in chunk
            return {0, 0, 0};

        if (rng.nextInt(8) != 0) // no lava lake in chunk
            return {0, 0, 0};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(rng.nextInt(120) + 8);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        if (yPos >= 63 && rng.nextInt(10) != 0) // no lava lake above sea level
            return {0, 0, 0};

        return {xPos, yPos, zPos};
    }
} // namespace Features
