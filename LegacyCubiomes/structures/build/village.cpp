#include "LegacyCubiomes/structures/build/village.hpp"


namespace build::village {

    MU void func_189927_a(MU const World& p_189927_1_, gen::Village* villageIn, MU BoundingBox p_189927_2_, MU RNG& p_189927_3_, MU int p_189927_4_, MU int p_189927_5_, MU int p_189927_6_, MU FACING p_189927_7_) {
        if (!villageIn->isZombieInfested) {
            // func_189915_a(p_189927_1_, p_189927_2_, p_189927_3_, p_189927_4_, p_189927_5_, p_189927_6_, FACING::NORTH, func_189925_i());
        }
    }

    MU void func_189926_a(MU const World& p_189926_1_, gen::Village* villageIn, MU FACING p_189926_2_, MU int p_189926_3_, MU int p_189926_4_, MU int p_189926_5_, MU BoundingBox p_189926_6_) {
        if (!villageIn->isZombieInfested) {
            // .withProperty(BlockTorch.FACING, p_189926_2_)
            // piece.setBlockState(p_189926_1_, Blocks.TORCH.getDefaultState(), p_189926_3_, p_189926_4_, p_189926_5_, p_189926_6_);
        }
    }



    MU const lce::blocks::Block* getBiomeSpecificBlockState(const lce::blocks::Block* blockPtr,
                                                            const int structureType) {

        if (structureType == 1) {
            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD) {
                return &lce::blocks::BlocksInit::SANDSTONE;
            }

            if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE) {
                return &lce::blocks::BlocksInit::SANDSTONE;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                return &lce::blocks::BlocksInit::SMOOTH_SANDSTONE;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &lce::blocks::BlocksInit::SANDSTONE_STAIRS;
            }

            if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &lce::blocks::BlocksInit::SANDSTONE_STAIRS;
            }

            if (blockPtr == &lce::blocks::BlocksInit::GRAVEL) {
                return &lce::blocks::BlocksInit::SANDSTONE;
            }
        } else if (structureType == 3) {
            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD) {
                // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                return &lce::blocks::BlocksInit::SPRUCE_WOOD;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                return &lce::blocks::BlocksInit::SPRUCE_WOOD_PLANK;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &lce::blocks::BlocksInit::SPRUCE_WOOD_STAIRS;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_FENCE) {
                return &lce::blocks::BlocksInit::SPRUCE_FENCE;
            }
        } else if (structureType == 2) {
            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD) {
                // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                return &lce::blocks::BlocksInit::ACACIA_WOOD;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                return &lce::blocks::BlocksInit::ACACIA_WOOD_PLANK;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &lce::blocks::BlocksInit::ACACIA_WOOD_STAIRS;
            }

            if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE) {
                // .withProperty(BlockLog.LOG_AXIS, BlockLog.EnumAxis.Y);
                return &lce::blocks::BlocksInit::ACACIA_WOOD;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_FENCE) {
                return &lce::blocks::BlocksInit::ACACIA_FENCE;
            }
        }

        return blockPtr;
    }

    MU ND int getAverageGroundLevel(World& worldIn, const BoundingBox& structureBB, const StructureComponent& piece) {
        int i = 0;
        int j = 0;
        Pos3D blockPos;

        for (int k = piece.minZ; k <= piece.maxZ; ++k) {
            for (int l = piece.minX; l <= piece.maxX; ++l) {
                blockPos.setPos(l, 64, k);

                if (structureBB.isVecInside(blockPos)) {
                    i += std::max(worldIn.getTopSolidOrLiquidBlock(blockPos).getY(), 64);
                    ++j;
                }
            }
        }

        if (j == 0) {
            return -1;
        }
        return i / j;
    }









}
