#include "village.hpp"


namespace rolls {

    MU const lce::blocks::Block* VillagePiece::getBiomeSpecificBlockState(const lce::blocks::Block* blockPtr) const {

        if (this->structureType == 1) {
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
                return &lce::blocks::BlocksInit::SANDSTONE_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
            }

            if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE_STAIRS) {
                return &lce::blocks::BlocksInit::SANDSTONE_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
            }

            if (blockPtr == &lce::blocks::BlocksInit::GRAVEL) {
                return &lce::blocks::BlocksInit::SANDSTONE;
            }
        } else if (this->structureType == 3) {
            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD) {
                return &lce::blocks::BlocksInit::SPRUCE_WOOD; // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                return &lce::blocks::BlocksInit::SPRUCE_WOOD_PLANK;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                return &lce::blocks::BlocksInit::SPRUCE_WOOD_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_FENCE) {
                return &lce::blocks::BlocksInit::SPRUCE_FENCE;
            }
        } else if (this->structureType == 2) {
            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD) {
                return &lce::blocks::BlocksInit::ACACIA_WOOD; // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                return &lce::blocks::BlocksInit::ACACIA_WOOD_PLANK;
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                return &lce::blocks::BlocksInit::ACACIA_WOOD_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
            }

            if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE) {
                return &lce::blocks::BlocksInit::ACACIA_WOOD; // .withProperty(BlockLog.LOG_AXIS, BlockLog.EnumAxis.Y);
            }

            if (blockPtr == &lce::blocks::BlocksInit::OAK_FENCE) {
                return &lce::blocks::BlocksInit::ACACIA_FENCE;
            }
        }

        return blockPtr;
    }

    MU ND int VillagePiece::getAverageGroundLevel(World& worldIn, const BoundingBox& structureBB) const {
        int i = 0;
        int j = 0;
        Pos3D blockPos;

        for (int k = minZ; k <= maxZ; ++k) {
            for (int l = minX; l <= maxX; ++l) {
                blockPos.setPos(l, 64, k);

                if (structureBB.isVecInside(blockPos)) {
                    i += std::max(worldIn.getTopSolidOrLiquidBlock(blockPos).getY(), 64);
                    ++j;
                }
            }
        }

        if (j == 0) {
            return -1;
        } else {
            return i / j;
        }
    }









}