#include "structures/gen/ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    bool JunglePyramid::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) {
        if (!this->offsetToAverageGroundLevel(worldIn, chunkBB, 0)) {
            return false;
        } else {
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 0, -4, 0, this->scatteredFeatureSizeX - 1, 0, this->scatteredFeatureSizeZ - 1, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 1, 2, 9, 2, 2, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 1, 12, 9, 2, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 1, 3, 2, 2, 11, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 9, 1, 3, 9, 2, 11, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 1, 3, 1, 10, 6, 1, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 1, 3, 13, 10, 6, 13, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 1, 3, 2, 1, 6, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 10, 3, 2, 10, 6, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 3, 2, 9, 3, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 6, 2, 9, 6, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 3, 7, 3, 8, 7, 11, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 4, 8, 4, 7, 8, 10, false, rng);
            this->fillWithAir(worldIn, chunkBB, 3, 1, 3, 8, 2, 11);
            this->fillWithAir(worldIn, chunkBB, 4, 3, 6, 7, 3, 9);
            this->fillWithAir(worldIn, chunkBB, 2, 4, 2, 9, 5, 12);
            this->fillWithAir(worldIn, chunkBB, 4, 6, 5, 7, 6, 9);
            this->fillWithAir(worldIn, chunkBB, 5, 7, 6, 6, 7, 8);
            this->fillWithAir(worldIn, chunkBB, 5, 1, 2, 6, 2, 2);
            this->fillWithAir(worldIn, chunkBB, 5, 2, 12, 6, 2, 12);
            this->fillWithAir(worldIn, chunkBB, 5, 5, 1, 6, 5, 1);
            this->fillWithAir(worldIn, chunkBB, 5, 5, 13, 6, 5, 13);
            this->setBlockState(worldIn, lce::BlocksInit::AIR.getDefaultState(), 1, 5, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::AIR.getDefaultState(), 10, 5, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::AIR.getDefaultState(), 1, 5, 9, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::AIR.getDefaultState(), 10, 5, 9, chunkBB);
            
            for (int i = 0; i <= 14; i += 14) {
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 4, i, 2, 5, i, false, rng);
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 4, 4, i, 4, 5, i, false, rng);
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 7, 4, i, 7, 5, i, false, rng);
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 9, 4, i, 9, 5, i, false, rng);
            }

            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 5, 6, 0, 6, 6, 0, false, rng);

            for (int l = 0; l <= 11; l += 11) {
                for (int j = 2; j <= 12; j += 2) {
                    this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, l, 4, j, l, 5, j, false, rng);
                }

                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, l, 6, 5, l, 6, 5, false, rng);
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, l, 6, 9, l, 6, 9, false, rng);
            }

            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 7, 2, 2, 9, 2, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 9, 7, 2, 9, 9, 2, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, 7, 12, 2, 9, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 9, 7, 12, 9, 9, 12, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 4, 9, 4, 4, 9, 4, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 7, 9, 4, 7, 9, 4, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 4, 9, 10, 4, 9, 10, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 7, 9, 10, 7, 9, 10, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 5, 9, 7, 6, 9, 7, false, rng);

            lce::BlockState iblockstate2 = lce::BlocksInit::STONE_STAIRS.getStateFromMeta(Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::EAST)));
            lce::BlockState iblockstate3 = lce::BlocksInit::STONE_STAIRS.getStateFromMeta(Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::WEST)));
            lce::BlockState iblockstate = lce::BlocksInit::STONE_STAIRS.getStateFromMeta(Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::SOUTH)));
            lce::BlockState iblockstate1 = lce::BlocksInit::STONE_STAIRS.getStateFromMeta(Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::NORTH)));
            this->setBlockState(worldIn, iblockstate1, 5, 9, 6, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 6, 9, 6, chunkBB);
            this->setBlockState(worldIn, iblockstate, 5, 9, 8, chunkBB);
            this->setBlockState(worldIn, iblockstate, 6, 9, 8, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 4, 0, 0, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 5, 0, 0, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 6, 0, 0, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 7, 0, 0, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 4, 1, 8, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 4, 2, 9, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 4, 3, 10, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 7, 1, 8, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 7, 2, 9, chunkBB);
            this->setBlockState(worldIn, iblockstate1, 7, 3, 10, chunkBB);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 4, 1, 9, 4, 1, 9, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 7, 1, 9, 7, 1, 9, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 4, 1, 10, 7, 2, 10, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 5, 4, 5, 6, 4, 5, false, rng);
            this->setBlockState(worldIn, iblockstate2, 4, 4, 5, chunkBB);
            this->setBlockState(worldIn, iblockstate3, 7, 4, 5, chunkBB);

            for (int k = 0; k < 4; ++k) {
                this->setBlockState(worldIn, iblockstate, 5, 0 - k, 6 + k, chunkBB);
                this->setBlockState(worldIn, iblockstate, 6, 0 - k, 6 + k, chunkBB);
                this->fillWithAir(worldIn, chunkBB, 5, 0 - k, 7 + k, 6, 0 - k, 9 + k);
            }

            this->fillWithAir(worldIn, chunkBB, 1, -3, 12, 10, -1, 13);
            this->fillWithAir(worldIn, chunkBB, 1, -3, 1, 3, -1, 13);
            this->fillWithAir(worldIn, chunkBB, 1, -3, 1, 9, -1, 5);

            for (int i1 = 1; i1 <= 13; i1 += 2) {
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 1, -3, i1, 1, -2, i1, false, rng);
            }

            for (int j1 = 2; j1 <= 12; j1 += 2) {
                this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 1, -1, j1, 3, -1, j1, false, rng);
            }

            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 2, -2, 1, 5, -2, 1, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 7, -2, 1, 9, -2, 1, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 6, -3, 1, 6, -3, 1, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 6, -1, 1, 6, -1, 1, false, rng);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE_HOOK.getDefaultState().withProperty(BlockTripWireHook.FACING, EnumFacing.EAST).withProperty(BlockTripWireHook.ATTACHED, Boolean.valueOf(true)), 1, -3, 8, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE_HOOK.getDefaultState().withProperty(BlockTripWireHook.FACING, EnumFacing.WEST).withProperty(BlockTripWireHook.ATTACHED, Boolean.valueOf(true)), 4, -3, 8, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE.getDefaultState().withProperty(BlockTripWire.ATTACHED, Boolean.valueOf(true)), 2, -3, 8, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE.getDefaultState().withProperty(BlockTripWire.ATTACHED, Boolean.valueOf(true)), 3, -3, 8, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 7, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 6, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 4, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 3, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 2, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 5, -3, 1, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 4, -3, 1, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 3, -3, 1, chunkBB);

            if (!this->placedTrap1) {
                // this->placedTrap1 = this->createDispenser(worldIn, chunkBB, rng, 3, -2, 1, EnumFacing.NORTH, LootTableList.CHESTS_JUNGLE_TEMPLE_DISPENSER);
            }

            this->setBlockState(worldIn, lce::BlocksInit::VINES.getStateFromMeta(states::Vine::withProperty(this->rotation.apply(this->mirror, EnumFacing::SOUTH))), 3, -2, 2, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE_HOOK.getDefaultState().withProperty(BlockTripWireHook.FACING, EnumFacing.NORTH).withProperty(BlockTripWireHook.ATTACHED, Boolean.valueOf(true)), 7, -3, 1, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE_HOOK.getDefaultState().withProperty(BlockTripWireHook.FACING, EnumFacing.SOUTH).withProperty(BlockTripWireHook.ATTACHED, Boolean.valueOf(true)), 7, -3, 5, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE.getDefaultState().withProperty(BlockTripWire.ATTACHED, Boolean.valueOf(true)), 7, -3, 2, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE.getDefaultState().withProperty(BlockTripWire.ATTACHED, Boolean.valueOf(true)), 7, -3, 3, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::TRIPWIRE.getDefaultState().withProperty(BlockTripWire.ATTACHED, Boolean.valueOf(true)), 7, -3, 4, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 8, -3, 6, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 9, -3, 6, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 9, -3, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 9, -3, 4, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 9, -2, 4, chunkBB);

            if (!this->placedTrap2) {
                // this->placedTrap2 = this->createDispenser(worldIn, chunkBB, rng, 9, -2, 3, EnumFacing.WEST, LootTableList.CHESTS_JUNGLE_TEMPLE_DISPENSER);
            }

            this->setBlockState(worldIn, lce::BlocksInit::VINES.getStateFromMeta(states::Vine::withProperty(this->rotation.apply(this->mirror, EnumFacing::EAST))), 8, -1, 3, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::VINES.getStateFromMeta(states::Vine::withProperty(this->rotation.apply(this->mirror, EnumFacing::EAST))), 8, -2, 3, chunkBB);

            if (!this->placedMainChest) {
                // this->placedMainChest = this->generateChest(worldIn, chunkBB, rng, 8, -3, 3, LootTableList.CHESTS_JUNGLE_TEMPLE);
            }

            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 9, -3, 2, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 8, -3, 1, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 4, -3, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 5, -2, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 5, -1, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 6, -3, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 7, -2, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 7, -1, 5, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 8, -3, 5, chunkBB);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 9, -1, 1, 9, -1, 5, false, rng);
            this->fillWithAir(worldIn, chunkBB, 8, -3, 8, 10, -1, 10);
            this->setBlockState(worldIn, lce::BlocksInit::CHISELED_STONE_BRICKS.getDefaultState(), 8, -2, 11, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::CHISELED_STONE_BRICKS.getDefaultState(), 9, -2, 11, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::CHISELED_STONE_BRICKS.getDefaultState(), 10, -2, 11, chunkBB);
            lce::BlockState iblockstate4 = lce::BlocksInit::LEVER.getStateFromMeta(Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::NORTH)));
            this->setBlockState(worldIn, iblockstate4, 8, -2, 12, chunkBB);
            this->setBlockState(worldIn, iblockstate4, 9, -2, 12, chunkBB);
            this->setBlockState(worldIn, iblockstate4, 10, -2, 12, chunkBB);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 8, -3, 8, 8, -3, 10, false, rng);
            this->fillWithRandomizedJunglePyramidStones(worldIn, chunkBB, 10, -3, 8, 10, -3, 10, false, rng);
            this->setBlockState(worldIn, lce::BlocksInit::MOSS_STONE.getDefaultState(), 10, -2, 9, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 8, -2, 9, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 8, -2, 10, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::REDSTONE_WIRE.getDefaultState(), 10, -1, 9, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::STICKY_PISTON.getStateFromMeta(Piston::withProperty(this->rotation.apply(this->mirror, EnumFacing::UP))), 9, -2, 8, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::STICKY_PISTON.getStateFromMeta(Piston::withProperty(this->rotation.apply(this->mirror, EnumFacing::WEST))), 10, -2, 8, chunkBB);
            this->setBlockState(worldIn, lce::BlocksInit::STICKY_PISTON.getStateFromMeta(Piston::withProperty(this->rotation.apply(this->mirror, EnumFacing::WEST))), 10, -1, 8, chunkBB);
            // this->setBlockState(worldIn, lce::BlocksInit::OFF_REDSTONE_REPEATER_BLOCK.getStateFromMeta( withProperty(BlockRedstoneRepeater.FACING, EnumFacing.NORTH), 10, -2, 10, chunkBB);

            if (!this->placedHiddenChest) {
                // this->placedHiddenChest = this->generateChest(worldIn, chunkBB, rng, 9, -3, 10, LootTableList.CHESTS_JUNGLE_TEMPLE);
            }
        }
        return true;
    }
}
