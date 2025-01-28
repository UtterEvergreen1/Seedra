#include "village.hpp"

#include <algorithm>

#include "structures/placement/StaticStructures.hpp"

#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"




namespace build::village {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    MU void spawnVillagers(MU const World& worldIn, MU const BoundingBox& chunkBB,
                           MU int x, MU int y, MU int z, MU int count) {

    }

    lce::BlockState getDoorTypeFromBiome(const StructureComponent& piece) {
        switch (piece.variant) {
            case StructureVariant::SV_Village_Desert:
                return lce::BlocksInit::ACACIA_DOOR_BLOCK.getState();
            case StructureVariant::SV_Village_Taiga:
                return lce::BlocksInit::SPRUCE_DOOR_BLOCK.getState();
            default:
                return lce::BlocksInit::OAK_DOOR_BLOCK.getState();
        }
    }



    void PlaceDoorBlock(World& world, BoundingBox chunkBB,
                        MU RNG& rng,
                        StructureComponent& piece,
                        int x, int y, int z,
                        EnumFacing facing, const lce::BlockState doorBlock) {

        auto doorBlockLower = doorBlock.getStateFromMeta(states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, facing)));
        piece.setBlockState(world, doorBlockLower, x, y, z, chunkBB);

        auto doorBlockUpper = doorBlock.getStateFromMeta(states::DoorUpper::withProperty());
        piece.setBlockState(world, doorBlockUpper, x, y + 1, z, chunkBB);
    }

    // TODO: JAVA code sucks, maybe replace 'enumFacing::NORTH' with 'facing'?
    MU void placeDoor(World& world, /*gen::Village* villageIn,*/ BoundingBox chunkBB,
                      RNG& rng,
                      StructureComponent& piece,
                      int x, int y,
                      int z, MU EnumFacing facing) {
        // if (!villageIn->isZombieInfested) {
        PlaceDoorBlock(world, chunkBB, rng, piece, x, y, z, EnumFacing::NORTH, getDoorTypeFromBiome(piece));
        // }
    }

    MU void placeTorch(MU World& world, /*gen::Village* villageIn,*/ MU EnumFacing facing,
                       StructureComponent& piece,
                       MU int x, MU int y, MU int z, MU BoundingBox chunkBB) {
        // if (!villageIn->isZombieInfested) {
        auto torch = lce::BlocksInit::TORCH.getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, facing)));
        piece.setBlockState(world, torch, x, y, z, chunkBB);
        // }
    }



    MU const lce::BlockState getBiomeSpecificBlockState(const lce::BlockState blockPtr,
                                                    const StructureVariant structureType) {
        if (structureType == StructureVariant::SV_Village_Plains) {
            if (blockPtr == lce::BlocksInit::GRAVEL.getState())
                return lce::BlocksInit::GRASS_PATH.getState();
        } else if (structureType == StructureVariant::SV_Village_Desert) {
            if (blockPtr == lce::BlocksInit::OAK_WOOD.getState())
                return lce::BlocksInit::SANDSTONE.getState();
            if (blockPtr == lce::BlocksInit::COBBLESTONE.getState())
                return lce::BlocksInit::SANDSTONE.getState();
            if (blockPtr == lce::BlocksInit::OAK_WOOD_PLANK.getState())
                return lce::BlocksInit::SMOOTH_SANDSTONE.getState();
            if (blockPtr == lce::BlocksInit::OAK_WOOD_STAIRS.getState())
                return lce::BlocksInit::SANDSTONE_STAIRS.getState();
            if (blockPtr == lce::BlocksInit::COBBLESTONE_STAIRS.getState())
                return lce::BlocksInit::SANDSTONE_STAIRS.getState();
            if (blockPtr == lce::BlocksInit::GRAVEL.getState())
                return lce::BlocksInit::SANDSTONE.getState();
            if (blockPtr == lce::BlocksInit::GRASS_PATH.getState())
                return lce::BlocksInit::SANDSTONE.getState();
        } else if (structureType == StructureVariant::SV_Village_Taiga) {
            if (blockPtr == lce::BlocksInit::OAK_WOOD.getState())
                return lce::BlocksInit::SPRUCE_WOOD.getState();
            if (blockPtr == lce::BlocksInit::OAK_WOOD_PLANK.getState())
                return lce::BlocksInit::SPRUCE_WOOD_PLANK.getState();
            if (blockPtr == lce::BlocksInit::OAK_WOOD_STAIRS.getState())
                return lce::BlocksInit::SPRUCE_WOOD_STAIRS.getState();
            if (blockPtr == lce::BlocksInit::OAK_FENCE.getState())
                return lce::BlocksInit::SPRUCE_FENCE.getState();
        } else if (structureType == StructureVariant::SV_Village_Savanna) {
            if (blockPtr == lce::BlocksInit::OAK_WOOD.getState())
                return lce::BlocksInit::ACACIA_WOOD.getState();
            if (blockPtr == lce::BlocksInit::OAK_WOOD_PLANK.getState())
                return lce::BlocksInit::ACACIA_WOOD_PLANK.getState();
            if (blockPtr == lce::BlocksInit::OAK_WOOD_STAIRS.getState())
                return lce::BlocksInit::ACACIA_WOOD_STAIRS.getState();
            if (blockPtr == lce::BlocksInit::COBBLESTONE.getState())
                return lce::BlocksInit::COBBLESTONE.getState();
            if (blockPtr == lce::BlocksInit::OAK_FENCE.getState())
                return lce::BlocksInit::ACACIA_FENCE.getState();
            if (blockPtr == lce::BlocksInit::GRAVEL.getState())
                return lce::BlocksInit::GRASS_PATH.getState();
        }
        return blockPtr;
    }

    MU ND int getAverageGroundLevel(World& worldIn, const BoundingBox& chunkBB, const StructureComponent& piece) {
        int i = 0;
        int j = 0;
        Pos3D blockPos;

        for (int z = piece.minZ; z <= piece.maxZ; ++z) {
            for (int x = piece.minX; x <= piece.maxX; ++x) {

                blockPos.setPos(x, 64, z);

                if (chunkBB.isVecInside(blockPos)) {
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


    bool Church::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 12 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState cobblestone = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);

        c_auto cobbleStairsSouth = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE_STAIRS.getState(), piece.variant);
        const auto cobbleStairsNorth = cobbleStairsSouth.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const auto cobbleStairsWest = cobbleStairsSouth.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));
        const auto cobbleStairsEast = cobbleStairsSouth.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST)));
        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 3, 3, 7);
        piece.fillWithAir(worldIn, chunkBB, 1, 5, 1, 3, 9, 3);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 3, 0, 8, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 0, 3, 10, 0, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 1, 0, 10, 3, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 1, 4, 10, 3, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 4, 0, 4, 7, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 0, 4, 4, 4, 7, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 8, 3, 4, 8, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 5, 4, 3, 10, 4, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 5, 5, 3, 5, 7, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 9, 0, 4, 9, 4, cobblestone, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 0, 4, 4, 4, cobblestone, false);
        piece.setBlockState(worldIn, cobblestone, 0, 11, 2, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 4, 11, 2, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 2, 11, 0, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 2, 11, 4, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 1, 1, 6, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 1, 1, 7, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 2, 1, 7, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 3, 1, 6, chunkBB);
        piece.setBlockState(worldIn, cobblestone, 3, 1, 7, chunkBB);
        piece.setBlockState(worldIn, cobbleStairsNorth, 1, 1, 5, chunkBB);
        piece.setBlockState(worldIn, cobbleStairsNorth, 2, 1, 6, chunkBB);
        piece.setBlockState(worldIn, cobbleStairsNorth, 3, 1, 5, chunkBB);
        piece.setBlockState(worldIn, cobbleStairsWest, 1, 2, 7, chunkBB);
        piece.setBlockState(worldIn, cobbleStairsEast, 3, 2, 7, chunkBB);

        const lce::BlockState glassPane = lce::BlocksInit::GLASS_PANE.getState();
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 3, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 3, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 6, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 7, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 6, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 7, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 6, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 7, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 6, 4, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 7, 4, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 3, 6, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 3, 6, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 3, 8, chunkBB);

        placeTorch(worldIn, EnumFacing::SOUTH, piece, 2, 4, 7, chunkBB);
        placeTorch(worldIn, EnumFacing::EAST, piece, 1, 4, 6, chunkBB);
        placeTorch(worldIn, EnumFacing::WEST, piece, 3, 4, 6, chunkBB);
        placeTorch(worldIn, EnumFacing::NORTH, piece, 2, 4, 5, chunkBB);

        // .withProperty(BlockLadder.FACING, FACING::WEST);
        const lce::BlockState ladderWest = lce::BlocksInit::LADDER.getStateFromMeta(
                states::Ladder::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));

        for (int i=1; i <= 9; ++i) {
            piece.setBlockState(worldIn, ladderWest, 3, i, 3, chunkBB);
        }

        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 1, 0, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 2, 0, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 2, 1, 0, EnumFacing::NORTH);

        if (isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB).getID()) &&
            !isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB).getID())) {
            piece.setBlockState(worldIn, cobbleStairsNorth, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB) == lce::BlocksInit::GRASS_PATH.getState()) {
                piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), 2, -1, -1, chunkBB);
            }
        }

        for (int k=0; k < 9; ++k) {
            for (int j=0; j < 5; ++j) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, j, 12, k, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, cobblestone, j, -1, k, chunkBB);
            }
        }

        // spawnVillagers(worldIn, chunkBB, 2, 1, 2, 1);
        return true;
    }


    const lce::BlockState getRandomCropType(const int data) {
        switch (data) {
            case 0:
            case 1:
                return lce::BlocksInit::CARROTS.getState();
            case 2:
            case 3:
                return lce::BlocksInit::POTATOES.getState();
            case 4:
                return lce::BlocksInit::BEETROOT_BLOCK.getState();
            default:
                return lce::BlocksInit::WHEAT_CROPS.getState();
        }
    }


    int getMaxAgeFromCrop(const int id) {
        switch (id) {
            case CARROTS_ID:
            case POTATOES_ID:
            case BEETROOT_BLOCK_ID:
            default:
                return 4;
            case WHEAT_CROPS_ID:
                return 8;
        }
    }


    bool Field1::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 4 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);
        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, 12, 4, 8);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 2, 0, 7, lce::BlocksInit::FARMLAND.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 0, 1, 5, 0, 7, lce::BlocksInit::FARMLAND.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 7, 0, 1, 8, 0, 7, lce::BlocksInit::FARMLAND.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 10, 0, 1, 11, 0, 7, lce::BlocksInit::FARMLAND.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 0, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6, 0, 0, 6, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 12, 0, 0, 12, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 11, 0, 0, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 8, 11, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 0, 1, 3, 0, 7, lce::BlocksInit::FLOWING_WATER.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 9, 0, 1, 9, 0, 7, lce::BlocksInit::STILL_WATER.getState(),
                             lce::BlocksInit::FLOWING_WATER.getState(), false);

        const lce::BlockState cropTypeA = getRandomCropType(piece.data >> 12 & 0xF);
        const lce::BlockState cropTypeB = getRandomCropType(piece.data >> 8 & 0xF);
        const lce::BlockState cropTypeC = getRandomCropType(piece.data >> 4 & 0xF);
        const lce::BlockState cropTypeD = getRandomCropType(piece.data & 0xF);
        for (int i = 1; i <= 7; ++i) {
            c_int j = getMaxAgeFromCrop(cropTypeA.getID());
            c_int k = j / 3;
            piece.setBlockState(worldIn, cropTypeA.getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeA.getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, chunkBB);
            c_int l = getMaxAgeFromCrop(cropTypeB.getID());
            c_int i1 = l / 3;
            piece.setBlockState(worldIn, cropTypeB.getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeB.getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, chunkBB);
            c_int j1 = getMaxAgeFromCrop(cropTypeC.getID());
            c_int k1 = j1 / 3;
            piece.setBlockState(worldIn, cropTypeC.getStateFromMeta(rng.nextInt(k1, j1)), 7, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeC.getStateFromMeta(rng.nextInt(k1, j1)), 8, 1, i, chunkBB);
            c_int l1 = getMaxAgeFromCrop(cropTypeD.getID());
            c_int i2 = l1 / 3;
            piece.setBlockState(worldIn, cropTypeD.getStateFromMeta(rng.nextInt(i2, l1)), 10, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeD.getStateFromMeta(rng.nextInt(i2, l1)), 11, 1, i, chunkBB);
        }


        for (int j2 = 0; j2 < 9; ++j2) {
            for (int k2 = 0; k2 < 13; ++k2) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, k2, 4, j2, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, lce::BlocksInit::DIRT.getState(), k2, -1, j2, chunkBB);
            }
        }

        return true;
    }



    bool Field2::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 4 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);
        constexpr lce::BlockState farmLand = lce::BlocksInit::FARMLAND.getState();
        constexpr lce::BlockState stillWater = lce::BlocksInit::STILL_WATER.getState();
        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, 6, 4, 8);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 2, 0, 7, farmLand, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 0, 1, 5, 0, 7, farmLand, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 0, 0, 8, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6, 0, 0, 6, 0, 8, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 5, 0, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 8, 5, 0, 8, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 0, 1, 3, 0, 7, stillWater, false);

        const lce::BlockState cropTypeA = getRandomCropType(piece.data >> 4 & 0xF);
        const lce::BlockState cropTypeB = getRandomCropType(piece.data & 0xF);
        for (int i = 1; i <= 7; ++i) {
            c_int j = getMaxAgeFromCrop(cropTypeA.getID());
            c_int k = j / 3;
            piece.setBlockState(worldIn, cropTypeA.getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeA.getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, chunkBB);
            c_int l = getMaxAgeFromCrop(cropTypeB.getID());
            c_int i1 = l / 3;
            piece.setBlockState(worldIn, cropTypeB.getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeB.getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, chunkBB);
        }

        for (int j1=0; j1 < 9; ++j1) {
            for (int k1=0; k1 < 7; ++k1) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, k1, 4, j1, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, lce::BlocksInit::DIRT.getState(), k1, -1, j1, chunkBB);
            }
        }

        return true;
    }


    bool Hall::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 7 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);

        c_auto woodStairs = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant);
        const lce::BlockState iBlockState1 = woodStairs.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState2 = woodStairs.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));
        const lce::BlockState iBlockState3 = woodStairs.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));

        const lce::BlockState iBlockState4 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);
        const lce::BlockState iBlockState5 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);
        const lce::BlockState iBlockState6 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);

        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 7, 4, 4);
        piece.fillWithAir(worldIn, chunkBB, 2, 1, 6, 8, 4, 10);
        piece.fillWithBlocks(worldIn, chunkBB, 2, 0, 6, 8, 0, 10, lce::BlocksInit::DIRT.getState(), false);
        piece.setBlockState(worldIn, iBlockState, 6, 0, 6, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 2, 1, 6, 2, 1, 10, iBlockState6, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 1, 6, 8, 1, 10, iBlockState6, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 10, 7, 1, 10, iBlockState6, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 7, 0, 4, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 0, 3, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 0, 0, 8, 3, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 7, 1, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 5, 7, 1, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 2, 0, 7, 3, 0, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 2, 5, 7, 3, 5, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 1, 8, 4, 1, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 4, 8, 4, 4, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 5, 2, 8, 5, 3, iBlockState4, false);
        piece.setBlockState(worldIn, iBlockState4, 0, 4, 2, chunkBB);
        piece.setBlockState(worldIn, iBlockState4, 0, 4, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState4, 8, 4, 2, chunkBB);
        piece.setBlockState(worldIn, iBlockState4, 8, 4, 3, chunkBB);
        const lce::BlockState iBlockState7 = iBlockState1;
        const lce::BlockState iBlockState8 = iBlockState2;

        for (int i=-1; i <= 2; ++i) {
            for (int j=0; j <= 8; ++j) {
                piece.setBlockState(worldIn, iBlockState7, j, 4 + i, i, chunkBB);
                piece.setBlockState(worldIn, iBlockState8, j, 4 + i, 5 - i, chunkBB);
            }
        }

        piece.setBlockState(worldIn, iBlockState5, 0, 2, 1, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 0, 2, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 8, 2, 1, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 8, 2, 4, chunkBB);
        constexpr lce::BlockState glassPane = lce::BlocksInit::GLASS_PANE.getState();
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 3, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 2, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 6, 2, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 2, 1, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::BROWN_CARPET.getState(), 2, 2, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState4, 1, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState7, 2, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState3, 1, 1, 3, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 0, 1, 7, 0, 3, lce::BlocksInit::DOUBLE_STONE_SLAB.getState(), false);
        piece.setBlockState(worldIn, lce::BlocksInit::DOUBLE_STONE_SLAB.getState(), 6, 1, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::DOUBLE_STONE_SLAB.getState(), 6, 1, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 1, 0, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 2, 0, chunkBB);

        placeTorch(worldIn, EnumFacing::NORTH, piece, 2, 3, 1, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 2, 1, 0, EnumFacing::NORTH);

        if (isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB).getID()) &&
            !isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB).getID())) {
            piece.setBlockState(worldIn, iBlockState7, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB) == lce::BlocksInit::GRASS_PATH.getState()) {
                piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), 2, -1, -1, chunkBB);
            }
        }

        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 6, 1, 5, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 6, 2, 5, chunkBB);

        placeTorch(worldIn, EnumFacing::SOUTH, piece, 6, 3, 4, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 6, 1, 5, EnumFacing::SOUTH);

        for (int k=0; k < 5; ++k) {
            for (int l=0; l < 9; ++l) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, l, 7, k, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iBlockState, l, -1, k, chunkBB);
            }
        }

        // spawnVillagers(worldIn, chunkBB, 4, 1, 2, 2);
        return true;
    }


    bool House1::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 9 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);

        const auto wood_stairs = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant);
        const lce::BlockState iBlockState1 = wood_stairs.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState2 = wood_stairs.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));
        const lce::BlockState iBlockState3 = wood_stairs.getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST)));

        const lce::BlockState iBlockState4 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);


        const lce::BlockState iBlockState5 = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState6 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);
        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 7, 5, 4);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 8, 0, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 5, 0, 8, 5, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 6, 1, 8, 6, 4, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 7, 2, 8, 7, 3, iBlockState, false);

        for (int i=-1; i <= 2; ++i) {
            for (int j=0; j <= 8; ++j) {
                piece.setBlockState(worldIn, iBlockState1, j, 6 + i, i, chunkBB);
                piece.setBlockState(worldIn, iBlockState2, j, 6 + i, 5 - i, chunkBB);
            }
        }

        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 0, 0, 1, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 5, 8, 1, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 1, 0, 8, 1, 4, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2, 1, 0, 7, 1, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 2, 0, 0, 4, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 2, 5, 0, 4, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 2, 5, 8, 4, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 2, 0, 8, 4, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 2, 1, 0, 4, 4, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 2, 5, 7, 4, 5, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 2, 1, 8, 4, 4, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 2, 0, 7, 4, 0, iBlockState4, false);
        constexpr lce::BlockState glassPane = lce::BlocksInit::GLASS_PANE.getState();
        piece.setBlockState(worldIn, glassPane, 4, 2, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 2, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 6, 2, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 3, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 3, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 6, 3, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 3, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 3, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 3, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 3, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 3, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 6, 2, 5, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 4, 1, 7, 4, 1, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 4, 4, 7, 4, 4, iBlockState4, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 3, 4, 7, 3, 4, lce::BlocksInit::BOOKSHELF.getState(), false);
        piece.setBlockState(worldIn, iBlockState4, 7, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState3, 7, 1, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 6, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 5, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 4, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 3, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 6, 1, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::BROWN_CARPET.getState(), 6, 2, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 4, 1, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::BROWN_CARPET.getState(), 4, 2, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::CRAFTING_TABLE.getState(), 7, 1, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 1, 1, 0, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 1, 2, 0, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 1, 1, 0, EnumFacing::NORTH);

        if (isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, 0, -1, chunkBB).getID()) &&
            !isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, -1, -1, chunkBB).getID())) {
            piece.setBlockState(worldIn, iBlockState5, 1, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 1, -1, -1, chunkBB) == lce::BlocksInit::GRASS_PATH.getState()) {
                piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), 1, -1, -1, chunkBB);
            }
        }

        for (int l=0; l < 6; ++l) {
            for (int k=0; k < 9; ++k) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, k, 9, l, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iBlockState, k, -1, l, chunkBB);
            }
        }

        // spawnVillagers(worldIn, chunkBB, 2, 1, 2, 1);
        return true;
    }


    bool House2::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 6 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);
        const lce::BlockState iBlockState1 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState2 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));
        const lce::BlockState iBlockState3 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);
        const lce::BlockState iBlockState4 = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState5 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);
        const lce::BlockState iBlockState6 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);

        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, 9, 4, 6);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 9, 0, 6, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 0, 9, 4, 6, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 5, 0, 9, 5, 6, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithAir(worldIn, chunkBB, 1, 5, 1, 8, 5, 5);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 0, 2, 3, 0, iBlockState3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 0, 0, 4, 0, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 0, 3, 4, 0, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 6, 0, 4, 6, iBlockState5, false);
        piece.setBlockState(worldIn, iBlockState3, 3, 3, 1, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 2, 3, 3, 2, iBlockState3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 3, 5, 3, 3, iBlockState3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 1, 0, 3, 5, iBlockState3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 6, 5, 3, 6, iBlockState3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 1, 0, 5, 3, 0, iBlockState6, false);
        piece.fillWithBlocks(worldIn, chunkBB, 9, 1, 0, 9, 3, 0, iBlockState6, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6, 1, 4, 9, 4, 6, iBlockState, false);
        piece.setBlockState(worldIn, lce::BlocksInit::FLOWING_LAVA.getState(), 7, 1, 5, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::FLOWING_LAVA.getState(), 8, 1, 5, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), 9, 2, 5, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), 9, 2, 4, chunkBB);
        piece.fillWithAir(worldIn, chunkBB, 7, 2, 4, 8, 2, 5);
        piece.setBlockState(worldIn, iBlockState, 6, 1, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::FURNACE.getState(), 6, 2, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::FURNACE.getState(), 6, 3, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::DOUBLE_STONE_SLAB.getState(), 8, 1, 1, chunkBB);
        const lce::BlockState glassPane = lce::BlocksInit::GLASS_PANE.getState();
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 4, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 6, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 2, 6, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 2, 1, 4, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::BROWN_CARPET.getState(), 2, 2, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState3, 1, 1, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 2, 1, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState2, 1, 1, 4, chunkBB);

        bool hasMadeChest = false;
        Pos3D chestPos = piece.getWorldXYZ(5, 1, 5);
        if (!hasMadeChest && chunkBB.isVecInside(chestPos)) {
            piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(), 5, 1, 5, chunkBB);
            rng.nextLong();
            // generateChest(worldIn, chunkBB, rng, 5, 1, 5, LootTableList.CHESTS_VILLAGE_BLACKSMITH);
        }


        for (int i=6; i <= 8; ++i) {
            if (isReplaceableBlock(piece.getBlockStateFromPos(worldIn, i, 0, -1, chunkBB).getID()) &&
                !isReplaceableBlock(piece.getBlockStateFromPos(worldIn, i, -1, -1, chunkBB).getID())) {
                piece.setBlockState(worldIn, iBlockState4, i, 0, -1, chunkBB);

                if (piece.getBlockStateFromPos(worldIn, i, -1, -1, chunkBB) == lce::BlocksInit::GRASS_PATH.getState()) {
                    piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), i, -1, -1, chunkBB);
                }
            }
        }

        for (int k=0; k < 7; ++k) {
            for (int j=0; j < 10; ++j) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, j, 6, k, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iBlockState, j, -1, k, chunkBB);
            }
        }

        // spawnVillagers(worldIn, chunkBB, 7, 1, 1, 1);
        return true;
    }


    bool House3::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 7 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);
        const lce::BlockState iBlockState1 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState2 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));
        const lce::BlockState iBlockState3 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST)));
        const lce::BlockState iBlockState4 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));
        const lce::BlockState iBlockState5 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);
        const lce::BlockState iBlockState6 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);

        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 7, 4, 4);
        piece.fillWithAir(worldIn, chunkBB, 2, 1, 6, 8, 4, 10);
        piece.fillWithBlocks(worldIn, chunkBB, 2, 0, 5, 8, 0, 10, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 7, 0, 4, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 0, 3, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 0, 0, 8, 3, 10, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 7, 2, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 5, 2, 1, 5, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2, 0, 6, 2, 3, 10, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 0, 10, 7, 3, 10, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 2, 0, 7, 3, 0, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 2, 5, 2, 3, 5, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 1, 8, 4, 1, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 4, 3, 4, 4, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 5, 2, 8, 5, 3, iBlockState5, false);
        piece.setBlockState(worldIn, iBlockState5, 0, 4, 2, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 0, 4, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 8, 4, 2, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 8, 4, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 8, 4, 4, chunkBB);
        const lce::BlockState iBlockState7 = iBlockState1;
        const lce::BlockState iBlockState8 = iBlockState2;
        const lce::BlockState iBlockState9 = iBlockState4;
        const lce::BlockState iBlockState10 = iBlockState3;

        for (int i=-1; i <= 2; ++i) {
            for (int j=0; j <= 8; ++j) {
                piece.setBlockState(worldIn, iBlockState7, j, 4 + i, i, chunkBB);

                if ((i > -1 || j <= 1) && (i > 0 || j <= 3) && (i > 1 || j <= 4 || j >= 6)) {
                    piece.setBlockState(worldIn, iBlockState8, j, 4 + i, 5 - i, chunkBB);
                }
            }
        }

        piece.fillWithBlocks(worldIn, chunkBB, 3, 4, 5, 3, 4, 10, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 7, 4, 2, 7, 4, 10, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 5, 4, 4, 5, 10, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6, 5, 4, 6, 5, 10, iBlockState5, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 6, 3, 5, 6, 10, iBlockState5, false);

        for (int k=4; k >= 1; --k) {
            piece.setBlockState(worldIn, iBlockState5, k, 2 + k, 7 - k, chunkBB);

            for (int k1=8 - k; k1 <= 10; ++k1) {
                piece.setBlockState(worldIn, iBlockState10, k, 2 + k, k1, chunkBB);
            }
        }

        piece.setBlockState(worldIn, iBlockState5, 6, 6, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 7, 5, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState4, 6, 6, 4, chunkBB);

        for (int l=6; l <= 8; ++l) {
            for (int l1=5; l1 <= 10; ++l1) {
                piece.setBlockState(worldIn, iBlockState9, l, 12 - l, l1, chunkBB);
            }
        }

        const lce::BlockState glassPane = lce::BlocksInit::GLASS_PANE.getState();
        piece.setBlockState(worldIn, iBlockState6, 0, 2, 1, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 0, 2, 4, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 4, 2, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 2, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 6, 2, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 8, 2, 1, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 8, 2, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 8, 2, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 8, 2, 6, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 7, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 8, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 8, 2, 9, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 2, 2, 6, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 7, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 8, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 2, 2, 9, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 4, 4, 10, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 4, 10, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 6, 4, 10, chunkBB);
        piece.setBlockState(worldIn, iBlockState5, 5, 5, 10, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 1, 0, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 2, 0, chunkBB);
        placeTorch(worldIn, EnumFacing::NORTH, piece, 2, 3, 1, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 2, 1, 0, EnumFacing::NORTH);
        piece.fillWithAir(worldIn, chunkBB, 1, 0, -1, 3, 2, -1);

        if (isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB).getID()) &&
            !isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB).getID())) {
            piece.setBlockState(worldIn, iBlockState7, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB) == lce::BlocksInit::GRASS_PATH.getState()) {
                piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), 2, -1, -1, chunkBB);
            }
        }

        for (int i1=0; i1 < 5; ++i1) {
            for (int i2=0; i2 < 9; ++i2) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, i2, 7, i1, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iBlockState, i2, -1, i1, chunkBB);
            }
        }

        for (int j1=5; j1 < 11; ++j1) {
            for (int j2=2; j2 < 9; ++j2) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, j2, 7, j1, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iBlockState, j2, -1, j1, chunkBB);
            }
        }

        // spawnVillagers(worldIn, chunkBB, 4, 1, 2, 2);
        return true;
    }


    static const lce::BlockState getUnderPathBlock(StructureVariant variant) {
        switch(variant) {
            case StructureVariant::SV_Village_Plains:
            case StructureVariant::SV_Village_Savanna:
            case StructureVariant::SV_Village_Taiga:
            default:
                return lce::BlocksInit::GRASS.getState();
            case StructureVariant::SV_Village_Desert:
                return lce::BlocksInit::SANDSTONE.getState();
        }
    }


    bool Path::addComponentParts(World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {

        const lce::BlockState pathPrimary = getBiomeSpecificBlockState(lce::BlocksInit::GRASS_PATH.getState(), piece.variant);
        const lce::BlockState pathSecondary = getBiomeSpecificBlockState(lce::BlocksInit::GRAVEL.getState(), piece.variant);

        const lce::BlockState overWaterBlock = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);
        // LCE only
        const lce::BlockState underPathPrimary = getUnderPathBlock(piece.variant);
        const lce::BlockState underPathSecondary = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);

        for (int x = piece.minX; x <= piece.maxX; ++x) {
            for (int z = piece.minZ; z <= piece.maxZ; ++z) {
                Pos3D topBlockPos(x, 64, z);

                if (chunkBB.isVecInside(topBlockPos)) {
                    topBlockPos = worldIn.getTopSolidOrLiquidBlock(topBlockPos).down();

                    if (topBlockPos.getY() < World::getSeaLevel()) {
                        topBlockPos = {topBlockPos.getX(), World::getSeaLevel() - 1, topBlockPos.getZ()};
                    }

                    while (topBlockPos.getY() >= World::getSeaLevel() - 1) {
                        const lce::BlockState topBlock = worldIn.getBlock(topBlockPos);

                        if (topBlock == lce::BlocksInit::GRASS.getState() && worldIn.isAirBlock(topBlockPos.up())) {
                            worldIn.setBlock(topBlockPos, pathPrimary);
                            // LCE only
                            worldIn.setBlock(topBlockPos.down(), underPathPrimary);
                            break;
                        }

                        if (isLiquidBlock(topBlock.getID())) {
                            worldIn.setBlock(topBlockPos, overWaterBlock);
                            break;
                        }

                        if (topBlock == lce::BlocksInit::SAND.getState() ||
                            topBlock == lce::BlocksInit::SANDSTONE.getState() ||
                            topBlock == lce::BlocksInit::RED_SANDSTONE.getState()) {
                            worldIn.setBlock(topBlockPos, pathSecondary);
                            worldIn.setBlock(topBlockPos.down(), underPathSecondary);
                            break;
                        }

                        topBlockPos = topBlockPos.down();
                    }
                }
            }
        }

        return true;
    }


    bool Torch::addComponentParts(World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 4 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState oakFence = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);

        piece.fillWithAir(worldIn, chunkBB, 0, 0, 0, 2, 3, 1);
        piece.setBlockState(worldIn, oakFence, 1, 0, 0, chunkBB);
        piece.setBlockState(worldIn, oakFence, 1, 1, 0, chunkBB);
        piece.setBlockState(worldIn, oakFence, 1, 2, 0, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::BLACK_WOOL.getState(), 1, 3, 0, chunkBB);
        placeTorch(worldIn, EnumFacing::EAST, piece, 2, 3, 0, chunkBB);
        placeTorch(worldIn, EnumFacing::NORTH, piece, 1, 3, 1, chunkBB);
        placeTorch(worldIn, EnumFacing::WEST, piece, 0, 3, 0, chunkBB);
        placeTorch(worldIn, EnumFacing::SOUTH, piece, 1, 3, -1, chunkBB);
        return true;
    }


    bool WoodHut::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 6 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iblockstate = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);
        const lce::BlockState iblockstate1 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);
        const lce::BlockState iblockstate2 = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iblockstate3 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);
        const lce::BlockState iblockstate4 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);

        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 3, 5, 4);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 3, 0, 4, iblockstate, iblockstate, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 2, 0, 3, lce::BlocksInit::DIRT.getState(), false);

        if (piece.data >> 8 & 1) { // isTallHouse
            piece.fillWithBlocks(worldIn, chunkBB, 1, 4, 1, 2, 4, 3, iblockstate3, false);
        } else {
            piece.fillWithBlocks(worldIn, chunkBB, 1, 5, 1, 2, 5, 3, iblockstate3, false);
        }

        piece.setBlockState(worldIn, iblockstate3, 1, 4, 0,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 2, 4, 0,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 1, 4, 4,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 2, 4, 4,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 0, 4, 1,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 0, 4, 2,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 0, 4, 3,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 3, 4, 1,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 3, 4, 2,  chunkBB);
        piece.setBlockState(worldIn, iblockstate3, 3, 4, 3,  chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 0, 0, 3, 0, iblockstate3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 0, 3, 3, 0, iblockstate3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 4, 0, 3, 4, iblockstate3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 4, 3, 3, 4, iblockstate3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 1, 0, 3, 3, iblockstate1, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 1, 3, 3, 3, iblockstate1, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 0, 2, 3, 0, iblockstate1, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 4, 2, 3, 4, iblockstate1, false);
        piece.setBlockState(worldIn, lce::BlocksInit::GLASS_PANE.getState(), 0, 2, 2,  chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::GLASS_PANE.getState(), 3, 2, 2,  chunkBB);

        int tablePosition = piece.data & 3;
        if (tablePosition > 0) {
            piece.setBlockState(worldIn, iblockstate4, tablePosition, 1, 3,  chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::WOODEN_PRESSURE_PLATE.getState(), tablePosition, 2, 3,  chunkBB);
        }

        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 1, 1, 0,  chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 1, 2, 0,  chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 1, 1, 0, EnumFacing::NORTH);

        if (isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, 0, -1,  chunkBB).getID()) &&
            !isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, -1, -1,  chunkBB).getID())) {
            piece.setBlockState(worldIn, iblockstate2, 1, 0, -1,  chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 1, -1, -1,  chunkBB) == lce::BlocksInit::GRASS_PATH.getState()) {
                piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), 1, -1, -1,  chunkBB);
            }
        }

        for (int i=0; i < 5; ++i) {
            for (int j=0; j < 4; ++j) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, j, 6, i,  chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iblockstate, j, -1, i,  chunkBB);
            }
        }

        // spawnVillagers(worldIn,  chunkBB, 1, 1, 2, 1);
        return true;
    }


    bool Well::addComponentParts(World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 3, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState bottomRing = piece.variant == StructureVariant::SV_Village_Desert ? lce::BlocksInit::SANDSTONE.getState() : lce::BlocksInit::GRASS_PATH.getState();
        const lce::BlockState cobblestone = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);
        const lce::BlockState fence = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);

        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 4, 12, 4, cobblestone, lce::BlocksInit::FLOWING_WATER.getState(), false);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 12, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 3, 12, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, 12, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 3, 12, 3, chunkBB);

        piece.setBlockState(worldIn, fence, 1, 13, 1, chunkBB);
        piece.setBlockState(worldIn, fence, 1, 14, 1, chunkBB);
        piece.setBlockState(worldIn, fence, 4, 13, 1, chunkBB);
        piece.setBlockState(worldIn, fence, 4, 14, 1, chunkBB);
        piece.setBlockState(worldIn, fence, 1, 13, 4, chunkBB);
        piece.setBlockState(worldIn, fence, 1, 14, 4, chunkBB);
        piece.setBlockState(worldIn, fence, 4, 13, 4, chunkBB);
        piece.setBlockState(worldIn, fence, 4, 14, 4, chunkBB);
        // Top piece
        piece.fillWithBlocks(worldIn, chunkBB, 1, 15, 1, 4, 15, 4, cobblestone, false);

        for (int z = 0; z <= 5; ++z) {
            for (int x = 0; x <= 5; ++x) {
                if (x == 0 || x == 5 || z == 0 || z == 5) {
                    piece.setBlockState(worldIn, bottomRing, x, 11, z, chunkBB);
                    piece.clearCurrentPositionBlocksUpwards(worldIn, x, 12, z, chunkBB);
                }
            }
        }

        return true;
    }


    bool House4Garden::addComponentParts(World& worldIn, MU RNG& rng, BoundingBox& chunkBB,
                                         StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 6 - 1, 0);
            piece.data |= 1 << 16;
        }

        const lce::BlockState iBlockState = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE.getState(), piece.variant);
        const lce::BlockState iBlockState1 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD_PLANK.getState(), piece.variant);
        const lce::BlockState iBlockState2 = getBiomeSpecificBlockState(lce::BlocksInit::COBBLESTONE_STAIRS.getState(), piece.variant)
                                                .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
        const lce::BlockState iBlockState3 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_WOOD.getState(), piece.variant);
        const lce::BlockState iBlockState4 = getBiomeSpecificBlockState(lce::BlocksInit::OAK_FENCE.getState(), piece.variant);

        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 4, 0, 4, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 0, 4, 4, 4, iBlockState3, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 4, 1, 3, 4, 3, iBlockState1, false);
        piece.setBlockState(worldIn, iBlockState, 0, 1, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 0, 2, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 0, 3, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 4, 1, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 4, 2, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 4, 3, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 0, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 0, 2, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 0, 3, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 4, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 4, 2, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState, 4, 3, 4, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 1, 1, 0, 3, 3, iBlockState1, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 1, 4, 3, 3, iBlockState1, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 4, 3, 3, 4, iBlockState1, false);
        constexpr lce::BlockState glassPane = lce::BlocksInit::GLASS_PANE.getState();
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 4, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 2, 2, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 1, 1, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 1, 2, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 1, 3, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 2, 3, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 3, 3, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 3, 2, 0, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 3, 1, 0, chunkBB);

        if (piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB).getID() != AIR_ID &&
            piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB).getID() != AIR_ID) { // material air
            piece.setBlockState(worldIn, iBlockState2, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB).getID() == GRASS_PATH_ID) {
                piece.setBlockState(worldIn, lce::BlocksInit::GRASS.getState(), 2, -1, -1, chunkBB);
            }
        }

        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 3, 3, 3);

        if (piece.data & 1) /* isRoofAccessible */ {
            piece.setBlockState(worldIn, iBlockState4, 0, 5, 0, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 1, 5, 0, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 2, 5, 0, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 3, 5, 0, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 4, 5, 0, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 0, 5, 4, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 1, 5, 4, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 2, 5, 4, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 3, 5, 4, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 4, 5, 4, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 4, 5, 1, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 4, 5, 2, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 4, 5, 3, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 0, 5, 1, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 0, 5, 2, chunkBB);
            piece.setBlockState(worldIn, iBlockState4, 0, 5, 3, chunkBB);
        }

        if (piece.data & 1) /* isRoofAccessible */ {
            // .withProperty(BlockLadder.FACING, FACING::SOUTH);
            c_auto ladderSouth = lce::BlocksInit::LADDER.getStateFromMeta(
                    states::Ladder::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));
            piece.setBlockState(worldIn, ladderSouth, 3, 1, 3, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 3, 2, 3, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 3, 3, 3, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 3, 4, 3, chunkBB);
        }

        placeTorch(worldIn, EnumFacing::NORTH, piece, 2, 3, 1, chunkBB);

        for (int j=0; j < 5; ++j) {
            for (int i=0; i < 5; ++i) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, i, 6, j, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, iBlockState, i, -1, j, chunkBB);
            }
        }

        // spawnVillagers(worldIn, chunkBB, 1, 1, 2, 1);
        return true;
    }


    bool addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        bool result = false;

        switch (piece.type) {
            case PT_Village_House4Garden:
                result = House4Garden::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Church:
                result = Church::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_House1:
                result = House1::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Hall:
                result = Hall::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Field1:
                result = Field1::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Field2:
                result = Field2::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_House2:
                result = House2::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_House3:
                result = House3::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Torch:
                result = Torch::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_WoodHut:
                result = WoodHut::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Road:
                result = Path::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Village_Start:
                result = Well::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            default:;
        }

        return result;
    }
}




