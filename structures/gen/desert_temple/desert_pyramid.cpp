#include "structures/gen/ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/World.hpp"

#include "desert_pyramid.hpp"
#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    DesertPyramid::~DesertPyramid() = default;


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
                // int k = -5;
                this->replaceAirAndLiquidDownwards(worldIn, sandstone, i2, -5, j, chunkBB);
            }
        }

        lce::BlockState iblockstate1 = lce::BlocksInit::SANDSTONE_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::NORTH)));
        lce::BlockState iblockstate2 = lce::BlocksInit::SANDSTONE_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::SOUTH)));
        lce::BlockState iblockstate3 = lce::BlocksInit::SANDSTONE_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::EAST)));
        lce::BlockState iblockstate = lce::BlocksInit::SANDSTONE_STAIRS. getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::WEST)));



        this->fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 4, 9, 4, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 1, 10, 1, 3, 10, 3, sandstone, sandstone, false);
        this->setBlockState(worldIn, chunkBB, 2, 10, 0, iblockstate1);
        this->setBlockState(worldIn, chunkBB, 2, 10, 4, iblockstate2);
        this->setBlockState(worldIn, chunkBB, 0, 10, 2, iblockstate3);
        this->setBlockState(worldIn, chunkBB, 4, 10, 2, iblockstate);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 5, 0, 0, sFSX - 1, 9, 4, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 4, 10, 1, sFSX - 2, 10, 3, sandstone, sandstone, false);
        this->setBlockState(worldIn, chunkBB, sFSX - 3, 10, 0, iblockstate1);
        this->setBlockState(worldIn, chunkBB, sFSX - 3, 10, 4, iblockstate2);
        this->setBlockState(worldIn, chunkBB, sFSX - 5, 10, 2, iblockstate3);
        this->setBlockState(worldIn, chunkBB, sFSX - 1, 10, 2, iblockstate);
        this->fillWithBlocks(worldIn, chunkBB, 8, 0, 0, 12, 4, 4, sandstone, air, false);
        this->fillWithBlocks(worldIn, chunkBB, 9, 1, 0, 11, 3, 4, air, air, false);
        this->setBlockState(worldIn, chunkBB, 9, 1, 1, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 9, 2, 1, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 9, 3, 1, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 10, 3, 1, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 11, 3, 1, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 11, 2, 1, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 11, 1, 1, smooth_sandstone);
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
        this->setBlockState(worldIn, chunkBB, 5, 5, 10, air);
        this->setBlockState(worldIn, chunkBB, 5, 6, 10, air);
        this->setBlockState(worldIn, chunkBB, 6, 6, 10, air);
        this->setBlockState(worldIn, chunkBB, sFSX - 6, 5, 10, air);
        this->setBlockState(worldIn, chunkBB, sFSX - 6, 6, 10, air);
        this->setBlockState(worldIn, chunkBB, sFSX - 7, 6, 10, air);
        this->fillWithBlocks(worldIn, chunkBB, 2, 4, 4, 2, 6, 4, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 3, 4, 4, sFSX - 3, 6, 4, air, air, false);
        this->setBlockState(worldIn, chunkBB, 2, 4, 5, iblockstate1);
        this->setBlockState(worldIn, chunkBB, 2, 3, 4, iblockstate1);
        this->setBlockState(worldIn, chunkBB, sFSX - 3, 4, 5, iblockstate1);
        this->setBlockState(worldIn, chunkBB, sFSX - 3, 3, 4, iblockstate1);
        this->fillWithBlocks(worldIn, chunkBB, 1, 1, 3, 2, 2, 3, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 3, 1, 3, sFSX - 2, 2, 3, sandstone, sandstone, false);
        this->setBlockState(worldIn, chunkBB, 1, 1, 2, sandstone);
        this->setBlockState(worldIn, chunkBB, sFSX - 2, 1, 2, sandstone);
        this->setBlockState(worldIn, chunkBB, 1, 2, 2, slab_sandstone);
        this->setBlockState(worldIn, chunkBB, sFSX - 2, 2, 2, slab_sandstone);
        this->setBlockState(worldIn, chunkBB, 2, 1, 2, iblockstate);
        this->setBlockState(worldIn, chunkBB, sFSX - 3, 1, 2, iblockstate3);
        this->fillWithBlocks(worldIn, chunkBB, 4, 3, 5, 4, 3, 18, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 5, 3, 5, sFSX - 5, 3, 17, sandstone, sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 3, 1, 5, 4, 2, 16, air, air, false);
        this->fillWithBlocks(worldIn, chunkBB, sFSX - 6, 1, 5, sFSX - 5, 2, 16, air, air, false);

        for (int j1 = 5; j1 <= 17; j1 += 2) {
            this->setBlockState(worldIn, chunkBB, 4, 1, j1, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, 4, 2, j1, chiseled_sandstone);
            this->setBlockState(worldIn, chunkBB, sFSX - 5, 1, j1, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, sFSX - 5, 2, j1, chiseled_sandstone);
        }

        this->setBlockState(worldIn, chunkBB, 10, 0, 7, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 10, 0, 8, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 9, 0, 9, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 11, 0, 9, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 8, 0, 10, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 12, 0, 10, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 7, 0, 10, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 13, 0, 10, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 9, 0, 11, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 11, 0, 11, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 10, 0, 12, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 10, 0, 13, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 10, 0, 10, hd_clay_blue);

        for (int j2 = 0; j2 <= sFSX - 1; j2 += sFSX - 1) {
            this->setBlockState(worldIn, chunkBB, j2, 2, 1, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 2, 2, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 2, 3, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 3, 1, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 3, 2, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 3, 3, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 4, 1, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 4, 2, chiseled_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 4, 3, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 5, 1, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 5, 2, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 5, 3, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 6, 1, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 6, 2, chiseled_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 6, 3, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 7, 1, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 7, 2, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 7, 3, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, j2, 8, 1, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 8, 2, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, j2, 8, 3, smooth_sandstone);
        }

        for (int k2 = 2; k2 <= sFSX - 3; k2 += sFSX - 3 - 2) {
            this->setBlockState(worldIn, chunkBB, k2 - 1, 2, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2, 2, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 2, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2 - 1, 3, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2, 3, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 3, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2 - 1, 4, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2, 4, 0, chiseled_sandstone);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 4, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 - 1, 5, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2, 5, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 5, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2 - 1, 6, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2, 6, 0, chiseled_sandstone);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 6, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 - 1, 7, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2, 7, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 7, 0, hd_clay_orange);
            this->setBlockState(worldIn, chunkBB, k2 - 1, 8, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2, 8, 0, smooth_sandstone);
            this->setBlockState(worldIn, chunkBB, k2 + 1, 8, 0, smooth_sandstone);
        }

        this->fillWithBlocks(worldIn, chunkBB, 8, 4, 0, 12, 6, 0,  smooth_sandstone,  smooth_sandstone, false);
        this->setBlockState(worldIn, chunkBB, 8, 6, 0, air);
        this->setBlockState(worldIn, chunkBB, 12, 6, 0, air);
        this->setBlockState(worldIn, chunkBB, 9, 5, 0, hd_clay_orange);
        this->setBlockState(worldIn, chunkBB, 10, 5, 0, chiseled_sandstone);
        this->setBlockState(worldIn, chunkBB, 11, 5, 0, hd_clay_orange);
        this->fillWithBlocks(worldIn, chunkBB, 8, -14, 8, 12, -11, 12,  smooth_sandstone,  smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, -10, 8, 12, -10, 12,  chiseled_sandstone,  chiseled_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, -9, 8, 12, -9, 12,  smooth_sandstone,  smooth_sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 8, -8, 8, 12, -1, 12, sandstone,  sandstone, false);
        this->fillWithBlocks(worldIn, chunkBB, 9, -11, 9, 11, -1, 11, air,  air, false);
        this->setBlockState(worldIn, chunkBB, 10, -11, 10, lce::BlocksInit::STONE_PRESSURE_PLATE.getDefaultState());
        this->fillWithBlocks(worldIn, chunkBB, 9, -13, 9, 11, -13, 11, lce::BlocksInit::TNT.getDefaultState(),  air, false);
        this->setBlockState(worldIn, chunkBB, 8, -11, 10, air);
        this->setBlockState(worldIn, chunkBB, 8, -10, 10, air);
        this->setBlockState(worldIn, chunkBB, 7, -10, 10, chiseled_sandstone);
        this->setBlockState(worldIn, chunkBB, 7, -11, 10, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 12, -11, 10, air);
        this->setBlockState(worldIn, chunkBB, 12, -10, 10, air);
        this->setBlockState(worldIn, chunkBB, 13, -10, 10, chiseled_sandstone);
        this->setBlockState(worldIn, chunkBB, 13, -11, 10, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 10, -11, 8, air);
        this->setBlockState(worldIn, chunkBB, 10, -10, 8, air);
        this->setBlockState(worldIn, chunkBB, 10, -10, 7, chiseled_sandstone);
        this->setBlockState(worldIn, chunkBB, 10, -11, 7, smooth_sandstone);
        this->setBlockState(worldIn, chunkBB, 10, -11, 12, air);
        this->setBlockState(worldIn, chunkBB, 10, -10, 12, air);
        this->setBlockState(worldIn, chunkBB, 10, -10, 13, chiseled_sandstone);
        this->setBlockState(worldIn, chunkBB, 10, -11, 13, smooth_sandstone);

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
