#include "structures/gen/ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    bool DesertPyramid::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) {

        int sFSX = this->scatteredFeatureSizeX;
        int sFSZ = this->scatteredFeatureSizeZ;

        constexpr lce::BlockState air = lce::BlocksInit::AIR.getDefaultState();
        constexpr lce::BlockState sandstone = lce::BlocksInit::SANDSTONE.getDefaultState();
        constexpr lce::BlockState smooth_sandstone = lce::BlocksInit::SMOOTH_SANDSTONE.getState();
        constexpr lce::BlockState chiseled_sandstone = lce::BlocksInit::CHISELED_SANDSTONE.getState();
        constexpr lce::BlockState hd_clay_orange = lce::BlocksInit::ORANGE_HARDENED_CLAY.getState();
        constexpr lce::BlockState hd_clay_blue = lce::BlocksInit::BLUE_HARDENED_CLAY.getState();
        constexpr lce::BlockState slab_sandstone = lce::BlocksInit::LOWER_SANDSTONE_SLAB.getState();

        this->fillWithBlocks(worldIn, chunkBB, 0, -4, 0, sFSX - 1, 0, sFSZ - 1,  sandstone, sandstone, false);

        for (int i = 1; i <= 9; ++i) {
            this->fillWithBlocks(worldIn, chunkBB, i, i, i, sFSX - 1 - i, i, sFSZ - 1 - i, sandstone, sandstone, false);
            this->fillWithAir(worldIn, chunkBB, i + 1, i, i + 1, sFSX - 2 - i, i, sFSZ - 2 - i);
        }

        for (int i2 = 0; i2 < sFSX; ++i2) {
            for (int j = 0; j < sFSZ; ++j) {
                int k = -5;
                this->replaceAirAndLiquidDownwards(worldIn, sandstone, i2, -5, j, chunkBB);
            }
        }

        lce::BlockState iblockstate1 = lce::BlocksInit::SANDSTONE_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::NORTH)));
        lce::BlockState iblockstate2 = lce::BlocksInit::SANDSTONE_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::SOUTH)));
        lce::BlockState iblockstate3 = lce::BlocksInit::SANDSTONE_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::EAST)));
        lce::BlockState iblockstate = lce::BlocksInit::SANDSTONE_STAIRS. getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::WEST)));



        this->fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 4, 9, 4, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 1, 10, 1, 3, 10, 3, sandstone, sandstone, false);
        this->setBlockState(worldIn, iblockstate1, 2, 10, 0, chunkBB);
        this->setBlockState(worldIn, iblockstate2, 2, 10, 4, chunkBB);
        this->setBlockState(worldIn, iblockstate3, 0, 10, 2, chunkBB);
        this->setBlockState(worldIn, iblockstate, 4, 10, 2, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 5, 0, 0, sFSX - 1, 9, 4, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 4, 10, 1, sFSX - 2, 10, 3, sandstone, sandstone, false);
        this->setBlockState(worldIn, iblockstate1, sFSX - 3, 10, 0, chunkBB);
        this->setBlockState(worldIn, iblockstate2, sFSX - 3, 10, 4, chunkBB);
        this->setBlockState(worldIn, iblockstate3, sFSX - 5, 10, 2, chunkBB);
        this->setBlockState(worldIn, iblockstate, sFSX - 1, 10, 2, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 8, 0, 0, 12, 4, 4, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 9, 1, 0, 11, 3, 4, air, air, false);
        this->setBlockState(worldIn, smooth_sandstone, 9, 1, 1, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 9, 2, 1, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 9, 3, 1, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 10, 3, 1, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 11, 3, 1, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 11, 2, 1, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 11, 1, 1, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 4, 1, 1, 8, 3, 3, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 4, 1, 2, 8, 2, 2, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 12, 1, 1, 16, 3, 3, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 12, 1, 2, 16, 2, 2, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 5, 4, 5, sFSX - 6, 4, sFSZ - 6, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 9, 4, 9, 11, 4, 11, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, 1, 8, 8, 3, 8, smooth_sandstone, smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 12, 1, 8, 12, 3, 8, smooth_sandstone, smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, 1, 12, 8, 3, 12, smooth_sandstone, smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 12, 1, 12, 12, 3, 12, smooth_sandstone, smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 1, 1, 5, 4, 4, 11, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 5, 1, 5, sFSX - 2, 4, 11, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 6, 7, 9, 6, 7, 11, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 7, 7, 9, sFSX - 7, 7, 11, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 5, 5, 9, 5, 7, 11, smooth_sandstone, smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 6, 5, 9, sFSX - 6, 7, 11, smooth_sandstone, smooth_sandstone, false);
        this->setBlockState(worldIn, air, 5, 5, 10, chunkBB);
        this->setBlockState(worldIn, air, 5, 6, 10, chunkBB);
        this->setBlockState(worldIn, air, 6, 6, 10, chunkBB);
        this->setBlockState(worldIn, air, sFSX - 6, 5, 10, chunkBB);
        this->setBlockState(worldIn, air, sFSX - 6, 6, 10, chunkBB);
        this->setBlockState(worldIn, air, sFSX - 7, 6, 10, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 2, 4, 4, 2, 6, 4, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 3, 4, 4, sFSX - 3, 6, 4, air, air, false);
        this->setBlockState(worldIn, iblockstate1, 2, 4, 5, chunkBB);
        this->setBlockState(worldIn, iblockstate1, 2, 3, 4, chunkBB);
        this->setBlockState(worldIn, iblockstate1, sFSX - 3, 4, 5, chunkBB);
        this->setBlockState(worldIn, iblockstate1, sFSX - 3, 3, 4, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 1, 1, 3, 2, 2, 3, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 3, 1, 3, sFSX - 2, 2, 3, sandstone, sandstone, false);
        this->setBlockState(worldIn, sandstone, 1, 1, 2, chunkBB);
        this->setBlockState(worldIn, sandstone, sFSX - 2, 1, 2, chunkBB);
        this->setBlockState(worldIn, slab_sandstone, 1, 2, 2, chunkBB);
        this->setBlockState(worldIn, slab_sandstone, sFSX - 2, 2, 2, chunkBB);
        this->setBlockState(worldIn, iblockstate, 2, 1, 2, chunkBB);
        this->setBlockState(worldIn, iblockstate3, sFSX - 3, 1, 2, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 4, 3, 5, 4, 3, 18, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 5, 3, 5, sFSX - 5, 3, 17, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 3, 1, 5, 4, 2, 16, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 6, 1, 5, sFSX - 5, 2, 16, air, air, false);

        for (int j1 = 5; j1 <= 17; j1 += 2) {
            this->setBlockState(worldIn, smooth_sandstone, 4, 1, j1, chunkBB);
            this->setBlockState(worldIn, chiseled_sandstone, 4, 2, j1, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, sFSX - 5, 1, j1, chunkBB);
            this->setBlockState(worldIn, chiseled_sandstone, sFSX - 5, 2, j1, chunkBB);
        }

        this->setBlockState(worldIn, hd_clay_orange, 10, 0, 7, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 10, 0, 8, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 9, 0, 9, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 11, 0, 9, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 8, 0, 10, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 12, 0, 10, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 7, 0, 10, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 13, 0, 10, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 9, 0, 11, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 11, 0, 11, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 10, 0, 12, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 10, 0, 13, chunkBB);
        this->setBlockState(worldIn, hd_clay_blue, 10, 0, 10, chunkBB);

        for (int j2 = 0; j2 <= sFSX - 1; j2 += sFSX - 1) {
            this->setBlockState(worldIn, smooth_sandstone, j2, 2, 1, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 2, 2, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 2, 3, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 3, 1, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 3, 2, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 3, 3, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 4, 1, chunkBB);
            this->setBlockState(worldIn, chiseled_sandstone, j2, 4, 2, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 4, 3, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 5, 1, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 5, 2, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 5, 3, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 6, 1, chunkBB);
            this->setBlockState(worldIn, chiseled_sandstone, j2, 6, 2, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 6, 3, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 7, 1, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 7, 2, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, j2, 7, 3, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 8, 1, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 8, 2, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, j2, 8, 3, chunkBB);
        }

        for (int k2 = 2; k2 <= sFSX - 3; k2 += sFSX - 3 - 2) {
            this->setBlockState(worldIn, smooth_sandstone, k2 - 1, 2, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2, 2, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 + 1, 2, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 - 1, 3, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2, 3, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 + 1, 3, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2 - 1, 4, 0, chunkBB);
            this->setBlockState(worldIn, chiseled_sandstone, k2, 4, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2 + 1, 4, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 - 1, 5, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2, 5, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 + 1, 5, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2 - 1, 6, 0, chunkBB);
            this->setBlockState(worldIn, chiseled_sandstone, k2, 6, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2 + 1, 6, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2 - 1, 7, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2, 7, 0, chunkBB);
            this->setBlockState(worldIn, hd_clay_orange, k2 + 1, 7, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 - 1, 8, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2, 8, 0, chunkBB);
            this->setBlockState(worldIn, smooth_sandstone, k2 + 1, 8, 0, chunkBB);
        }

        this->fillWithBlocks(worldIn, chunkBB, 8, 4, 0, 12, 6, 0,  smooth_sandstone,  smooth_sandstone, false);
        this->setBlockState(worldIn, air, 8, 6, 0, chunkBB);
        this->setBlockState(worldIn, air, 12, 6, 0, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 9, 5, 0, chunkBB);
        this->setBlockState(worldIn, chiseled_sandstone, 10, 5, 0, chunkBB);
        this->setBlockState(worldIn, hd_clay_orange, 11, 5, 0, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 8, -14, 8, 12, -11, 12,  smooth_sandstone,  smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, -10, 8, 12, -10, 12,  chiseled_sandstone,  chiseled_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, -9, 8, 12, -9, 12,  smooth_sandstone,  smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, -8, 8, 12, -1, 12, sandstone,  sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 9, -11, 9, 11, -1, 11, air,  air, false);
        this->setBlockState(worldIn, lce::BlocksInit::STONE_PRESSURE_PLATE.getDefaultState(), 10, -11, 10, chunkBB);
        this->fillWithBlocks(worldIn, chunkBB, 9, -13, 9, 11, -13, 11, lce::BlocksInit::TNT.getDefaultState(),  air, false);
        this->setBlockState(worldIn, air, 8, -11, 10, chunkBB);
        this->setBlockState(worldIn, air, 8, -10, 10, chunkBB);
        this->setBlockState(worldIn, chiseled_sandstone, 7, -10, 10, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 7, -11, 10, chunkBB);
        this->setBlockState(worldIn, air, 12, -11, 10, chunkBB);
        this->setBlockState(worldIn, air, 12, -10, 10, chunkBB);
        this->setBlockState(worldIn, chiseled_sandstone, 13, -10, 10, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 13, -11, 10, chunkBB);
        this->setBlockState(worldIn, air, 10, -11, 8, chunkBB);
        this->setBlockState(worldIn, air, 10, -10, 8, chunkBB);
        this->setBlockState(worldIn, chiseled_sandstone, 10, -10, 7, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 10, -11, 7, chunkBB);
        this->setBlockState(worldIn, air, 10, -11, 12, chunkBB);
        this->setBlockState(worldIn, air, 10, -10, 12, chunkBB);
        this->setBlockState(worldIn, chiseled_sandstone, 10, -10, 13, chunkBB);
        this->setBlockState(worldIn, smooth_sandstone, 10, -11, 13, chunkBB);

        // for (EnumFacing enumfacing: FACING_HORIZONTAL) {
        //     if (!this.hasPlacedChest[EnumFacing::getHorizontalIndex()]) {
        //         int k1 = EnumFacing::getFrontOffsetX() * 2;
        //         int l1 = EnumFacing::getFrontOffsetZ() * 2;
        //         this.hasPlacedChest[EnumFacing::getHorizontalIndex()] =
        //                 this.generateChest(worldIn, chunkBB, randomIn, 10 + k1, -11, 10 + l1,
        //                                    LootTableList.CHESTS_DESERT_PYRAMID);
        //     }
        // }

        return true;
    }
}
