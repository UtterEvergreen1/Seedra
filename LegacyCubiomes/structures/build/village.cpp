#include "LegacyCubiomes/structures/build/village.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"


namespace build::village {

    using namespace lce::blocks;
    using namespace states;


    MU void spawnVillagers(const World& worldIn, const BoundingBox& chunkBB, int x, int y, int z, int count) {

    }


    const Block* getDoorTypeFromBiome(StructureComponent& piece) {
        switch (piece.structureType) {
            case 2:
                return &BlocksInit::ACACIA_DOOR_BLOCK;

            case 3:
                return &BlocksInit::SPRUCE_DOOR_BLOCK;

            default:
                return &BlocksInit::OAK_DOOR_BLOCK;
        }
    }



    void PlaceDoorBlock(World& world, BoundingBox chunkBB,
                        RNG& rng,
                        StructureComponent& piece,
                        int x, int y, int z,
                        enumFacing facing, const Block* doorBlock) {

        auto doorBlockLower = doorBlock->getStateFromMeta(states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, facing)));
        piece.setBlockState(world, doorBlockLower, x, y, z, chunkBB);

        auto doorBlockUpper = doorBlock->getStateFromMeta(states::DoorUpper::withProperty());
        piece.setBlockState(world, doorBlockUpper, x, y + 1, z, chunkBB);
    }

    // TODO: JAVA code sucks, maybe replace 'enumFacing::NORTH' with 'facing'?
    MU void placeDoor(World& world, /*gen::Village* villageIn,*/ BoundingBox chunkBB,
                      RNG& rng,
                      StructureComponent& piece,
                      int x, int y,
                      int z, enumFacing facing) {
        // if (!villageIn->isZombieInfested) {
        PlaceDoorBlock(world, chunkBB, rng, piece, x, y, z, enumFacing::NORTH, getDoorTypeFromBiome(piece));
        // }
    }

    MU void placeTorch(MU World& world, /*gen::Village* villageIn,*/ MU enumFacing facing,
                       StructureComponent& piece,
                       MU int x, MU int y, MU int z, MU BoundingBox chunkBB) {
        // if (!villageIn->isZombieInfested) {
        auto Torch = BlocksInit::TORCH.getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, facing)));
        piece.setBlockState(world, BlocksInit::TORCH, x, y, z, chunkBB);
        // }
    }



    MU const Block* getBiomeSpecificBlockState(const Block* blockPtr,
                                               const int structureType) {

        if (structureType == 1) {
            if (blockPtr == &BlocksInit::OAK_WOOD) {
                return &BlocksInit::SANDSTONE;
            }

            if (blockPtr == &BlocksInit::COBBLESTONE) {
                return &BlocksInit::SANDSTONE;
            }

            if (blockPtr == &BlocksInit::OAK_WOOD_PLANK) {
                return &BlocksInit::SMOOTH_SANDSTONE;
            }

            if (blockPtr == &BlocksInit::OAK_WOOD_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &BlocksInit::SANDSTONE_STAIRS;
            }

            if (blockPtr == &BlocksInit::COBBLESTONE_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &BlocksInit::SANDSTONE_STAIRS;
            }

            if (blockPtr == &BlocksInit::GRAVEL) {
                return &BlocksInit::SANDSTONE;
            }
        } else if (structureType == 3) {
            if (blockPtr == &BlocksInit::OAK_WOOD) {
                // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                return &BlocksInit::SPRUCE_WOOD;
            }

            if (blockPtr == &BlocksInit::OAK_WOOD_PLANK) {
                return &BlocksInit::SPRUCE_WOOD_PLANK;
            }

            if (blockPtr == &BlocksInit::OAK_WOOD_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &BlocksInit::SPRUCE_WOOD_STAIRS;
            }

            if (blockPtr == &BlocksInit::OAK_FENCE) {
                return &BlocksInit::SPRUCE_FENCE;
            }
        } else if (structureType == 2) {
            if (blockPtr == &BlocksInit::OAK_WOOD) {
                // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                return &BlocksInit::ACACIA_WOOD;
            }

            if (blockPtr == &BlocksInit::OAK_WOOD_PLANK) {
                return &BlocksInit::ACACIA_WOOD_PLANK;
            }

            if (blockPtr == &BlocksInit::OAK_WOOD_STAIRS) {
                // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                return &BlocksInit::ACACIA_WOOD_STAIRS;
            }

            if (blockPtr == &BlocksInit::COBBLESTONE) {
                // .withProperty(BlockLog.LOG_AXIS, BlockLog.EnumAxis.Y);
                return &BlocksInit::COBBLESTONE;
            }

            if (blockPtr == &BlocksInit::OAK_FENCE) {
                return &BlocksInit::ACACIA_FENCE;
            }
            if (blockPtr == &BlocksInit::GRAVEL) {
                return &BlocksInit::GRASS_PATH;
            }
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

                // if (chunkBB.isVecInside(blockPos)) {
                i += std::max(worldIn.getTopSolidOrLiquidBlock(blockPos).getY(), 64);
                ++j;
                // }
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

        const Block* cobblestone = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);

        c_auto cobbleStairsSouth = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE_STAIRS, piece.structureType);
        const auto cobbleStairsNorth = cobbleStairsSouth->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const auto cobbleStairsWest = cobbleStairsSouth->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));
        const auto cobbleStairsEast = cobbleStairsSouth->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::EAST)));
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

        const Block* glassPane = &BlocksInit::GLASS_PANE;
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

        placeTorch(worldIn, enumFacing::SOUTH, piece, 2, 4, 7, chunkBB);
        placeTorch(worldIn, enumFacing::EAST, piece, 1, 4, 6, chunkBB);
        placeTorch(worldIn, enumFacing::WEST, piece, 3, 4, 6, chunkBB);
        placeTorch(worldIn, enumFacing::NORTH, piece, 2, 4, 5, chunkBB);

        // .withProperty(BlockLadder.FACING, FACING::WEST);
        const Block ladderWest = BlocksInit::LADDER.getStateFromMeta(
                states::Ladder::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));

        for (int i=1; i <= 9; ++i) {
            piece.setBlockState(worldIn, ladderWest, 3, i, 3, chunkBB);
        }

        piece.setBlockState(worldIn, &BlocksInit::AIR, 2, 1, 0, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 2, 2, 0, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 2, 1, 0, enumFacing::NORTH);

        if (ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB)->getID()) &&
            !ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB)->getID())) {
            piece.setBlockState(worldIn, cobbleStairsNorth, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB) == &BlocksInit::GRASS_PATH) {
                piece.setBlockState(worldIn, &BlocksInit::GRASS, 2, -1, -1, chunkBB);
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


    const Block* getRandomCropType(const int data) {
        switch (data) {
            case 0:
            case 1:
                return &BlocksInit::CARROTS;
            case 2:
            case 3:
                return &BlocksInit::POTATOES;
            case 4:
                return &BlocksInit::BEETROOT_BLOCK;
            default:
                return &BlocksInit::WHEAT_CROPS;
        }
    }


    int getMaxAgeFromCrop(const int id) {
        switch (id) {
            case ids::CARROTS_ID:
            case ids::POTATOES_ID:
            case ids::BEETROOT_BLOCK_ID:
            default:
                return 4;
            case ids::WHEAT_CROPS_ID:
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

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);
        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, 12, 4, 8);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 2, 0, 7, &BlocksInit::FARMLAND, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 0, 1, 5, 0, 7, &BlocksInit::FARMLAND, false);
        piece.fillWithBlocks(worldIn, chunkBB, 7, 0, 1, 8, 0, 7, &BlocksInit::FARMLAND, false);
        piece.fillWithBlocks(worldIn, chunkBB, 10, 0, 1, 11, 0, 7, &BlocksInit::FARMLAND, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 0, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6, 0, 0, 6, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 12, 0, 0, 12, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 11, 0, 0, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 8, 11, 0, 8, iBlockState, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 0, 1, 3, 0, 7, &BlocksInit::FLOWING_WATER, false);
        piece.fillWithBlocks(worldIn, chunkBB, 9, 0, 1, 9, 0, 7, &BlocksInit::STILL_WATER, &BlocksInit::FLOWING_WATER, false);

        const Block* cropTypeA = getRandomCropType(piece.data >> 12 & 0xF);
        const Block* cropTypeB = getRandomCropType(piece.data >> 8 & 0xF);
        const Block* cropTypeC = getRandomCropType(piece.data >> 4 & 0xF);
        const Block* cropTypeD = getRandomCropType(piece.data & 0xF);
        for (int i = 1; i <= 7; ++i) {
            c_int j = getMaxAgeFromCrop(cropTypeA->getID());
            c_int k = j / 3;
            piece.setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, chunkBB);
            c_int l = getMaxAgeFromCrop(cropTypeB->getID());
            c_int i1 = l / 3;
            piece.setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, chunkBB);
            c_int j1 = getMaxAgeFromCrop(cropTypeC->getID());
            c_int k1 = j1 / 3;
            piece.setBlockState(worldIn, cropTypeC->getStateFromMeta(rng.nextInt(k1, j1)), 7, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeC->getStateFromMeta(rng.nextInt(k1, j1)), 8, 1, i, chunkBB);
            c_int l1 = getMaxAgeFromCrop(cropTypeD->getID());
            c_int i2 = l1 / 3;
            piece.setBlockState(worldIn, cropTypeD->getStateFromMeta(rng.nextInt(i2, l1)), 10, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeD->getStateFromMeta(rng.nextInt(i2, l1)), 11, 1, i, chunkBB);
        }

        for (int j2 = 0; j2 < 9; ++j2) {
            for (int k2 = 0; k2 < 13; ++k2) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, k2, 4, j2, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, &BlocksInit::DIRT, k2, -1, j2, chunkBB);
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

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);
        const Block* farmLand = &BlocksInit::FARMLAND;
        const Block* stillWater = &BlocksInit::STILL_WATER;
        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, 6, 4, 8);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 2, 0, 7, farmLand, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 0, 1, 5, 0, 7, farmLand, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 0, 0, 8, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6, 0, 0, 6, 0, 8, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 0, 5, 0, 0, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 8, 5, 0, 8, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 0, 1, 3, 0, 7, stillWater, false);

        const Block* cropTypeA = getRandomCropType(piece.data >> 4 & 0xF);
        const Block* cropTypeB = getRandomCropType(piece.data & 0xF);
        for (int i = 1; i <= 7; ++i) {
            c_int j = getMaxAgeFromCrop(cropTypeA->getID());
            c_int k = j / 3;
            piece.setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, chunkBB);
            c_int l = getMaxAgeFromCrop(cropTypeB->getID());
            c_int i1 = l / 3;
            piece.setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, chunkBB);
            piece.setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, chunkBB);
        }

        for (int j1=0; j1 < 9; ++j1) {
            for (int k1=0; k1 < 7; ++k1) {
                piece.clearCurrentPositionBlocksUpwards(worldIn, k1, 4, j1, chunkBB);
                piece.replaceAirAndLiquidDownwards(worldIn, &BlocksInit::DIRT, k1, -1, j1, chunkBB);
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

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);

        c_auto woodStairs = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType);
        c_auto iBlockState1 = woodStairs->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        c_auto iBlockState2 = woodStairs->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));
        c_auto iBlockState3 = woodStairs->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));

        const Block* iBlockState4 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);
        const Block* iBlockState5 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);
        const Block* iBlockState6 = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);

        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 7, 4, 4);
        piece.fillWithAir(worldIn, chunkBB, 2, 1, 6, 8, 4, 10);
        piece.fillWithBlocks(worldIn, chunkBB, 2, 0, 6, 8, 0, 10, &BlocksInit::DIRT, false);
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
        const Block& iBlockState7 = iBlockState1;
        const Block& iBlockState8 = iBlockState2;

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
        const Block* glassPane = &BlocksInit::GLASS_PANE;
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 8, 2, 3, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 3, 2, 5, chunkBB);
        piece.setBlockState(worldIn, glassPane, 5, 2, 0, chunkBB);
        piece.setBlockState(worldIn, glassPane, 6, 2, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 2, 1, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::WOODEN_PRESSURE_PLATE, 2, 2, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState4, 1, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState7, 2, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState3, 1, 1, 3, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 0, 1, 7, 0, 3, &BlocksInit::DOUBLE_STONE_SLAB, false);
        piece.setBlockState(worldIn, &BlocksInit::DOUBLE_STONE_SLAB, 6, 1, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::DOUBLE_STONE_SLAB, 6, 1, 2, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 2, 1, 0, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 2, 2, 0, chunkBB);

        placeTorch(worldIn, enumFacing::NORTH, piece, 2, 3, 1, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 2, 1, 0, enumFacing::NORTH);

        if (ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB)->getID()) &&
            !ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB)->getID())) {
            piece.setBlockState(worldIn, iBlockState7, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB) == &BlocksInit::GRASS_PATH) {
                piece.setBlockState(worldIn, &BlocksInit::GRASS, 2, -1, -1, chunkBB);
            }
        }

        piece.setBlockState(worldIn, &BlocksInit::AIR, 6, 1, 5, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 6, 2, 5, chunkBB);

        placeTorch(worldIn, enumFacing::SOUTH, piece, 6, 3, 4, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 6, 1, 5, enumFacing::SOUTH);

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

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);

        const auto wood_stairs = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType);
        const Block iBlockState1 = wood_stairs->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block iBlockState2 = wood_stairs->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));
        const Block iBlockState3 = wood_stairs->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::EAST)));

        const Block* iBlockState4 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);


        const Block iBlockState5 = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block* iBlockState6 = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);
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
        const Block* glassPane = &BlocksInit::GLASS_PANE;
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
        piece.fillWithBlocks(worldIn, chunkBB, 1, 3, 4, 7, 3, 4, &BlocksInit::BOOKSHELF, false);
        piece.setBlockState(worldIn, iBlockState4, 7, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState3, 7, 1, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 6, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 5, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 4, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 3, 1, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 6, 1, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::WOODEN_PRESSURE_PLATE, 6, 2, 3, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 4, 1, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::WOODEN_PRESSURE_PLATE, 4, 2, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::CRAFTING_TABLE, 7, 1, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 1, 1, 0, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 1, 2, 0, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 1, 1, 0, enumFacing::NORTH);

        if (ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, 0, -1, chunkBB)->getID()) &&
            !ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, -1, -1, chunkBB)->getID())) {
            piece.setBlockState(worldIn, iBlockState5, 1, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 1, -1, -1, chunkBB) == &BlocksInit::GRASS_PATH) {
                piece.setBlockState(worldIn, &BlocksInit::GRASS, 1, -1, -1, chunkBB);
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

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);
        const Block iBlockState1 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block iBlockState2 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));
        const Block* iBlockState3 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);
        const Block iBlockState4 = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block* iBlockState5 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);
        const Block* iBlockState6 = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);

        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, 9, 4, 6);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 9, 0, 6, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 4, 0, 9, 4, 6, iBlockState, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 5, 0, 9, 5, 6, &BlocksInit::STONE_SLAB, false);
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
        piece.setBlockState(worldIn, &BlocksInit::FLOWING_LAVA, 7, 1, 5, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::FLOWING_LAVA, 8, 1, 5, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, 9, 2, 5, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, 9, 2, 4, chunkBB);
        piece.fillWithAir(worldIn, chunkBB, 7, 2, 4, 8, 2, 5);
        piece.setBlockState(worldIn, iBlockState, 6, 1, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::FURNACE, 6, 2, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::FURNACE, 6, 3, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::DOUBLE_STONE_SLAB, 8, 1, 1, chunkBB);
        const Block* glassPane = &BlocksInit::GLASS_PANE;
        piece.setBlockState(worldIn, glassPane, 0, 2, 2, chunkBB);
        piece.setBlockState(worldIn, glassPane, 0, 2, 4, chunkBB);
        piece.setBlockState(worldIn, glassPane, 2, 2, 6, chunkBB);
        piece.setBlockState(worldIn, glassPane, 4, 2, 6, chunkBB);
        piece.setBlockState(worldIn, iBlockState6, 2, 1, 4, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::WOODEN_PRESSURE_PLATE, 2, 2, 4, chunkBB);
        piece.setBlockState(worldIn, iBlockState3, 1, 1, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState1, 2, 1, 5, chunkBB);
        piece.setBlockState(worldIn, iBlockState2, 1, 1, 4, chunkBB);

        bool hasMadeChest = false;
        Pos3D chestPos = piece.getWorldXYZ(5, 1, 5);
        if (!hasMadeChest && chunkBB.isVecInside(chestPos)) {
            piece.setBlockState(worldIn, BlocksInit::CHEST, 5, 1, 5, chunkBB);
            rng.nextLong();
            // generateChest(worldIn, chunkBB, rng, 5, 1, 5, LootTableList.CHESTS_VILLAGE_BLACKSMITH);
        }


        for (int i=6; i <= 8; ++i) {
            if (ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, i, 0, -1, chunkBB)->getID()) &&
                !ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, i, -1, -1, chunkBB)->getID())) {
                piece.setBlockState(worldIn, iBlockState4, i, 0, -1, chunkBB);

                if (piece.getBlockStateFromPos(worldIn, i, -1, -1, chunkBB) == &BlocksInit::GRASS_PATH) {
                    piece.setBlockState(worldIn, &BlocksInit::GRASS, i, -1, -1, chunkBB);
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

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);
        const Block iBlockState1 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block iBlockState2 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));
        const Block iBlockState3 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::EAST)));
        const Block iBlockState4 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));
        const Block* iBlockState5 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);
        const Block* iBlockState6 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);

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
        const Block& iBlockState7 = iBlockState1;
        const Block& iBlockState8 = iBlockState2;
        const Block& iBlockState9 = iBlockState4;
        const Block& iBlockState10 = iBlockState3;

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

        const Block* glassPane = &BlocksInit::GLASS_PANE;
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
        piece.setBlockState(worldIn, &BlocksInit::AIR, 2, 1, 0, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 2, 2, 0, chunkBB);
        placeTorch(worldIn, enumFacing::NORTH, piece, 2, 3, 1, chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 2, 1, 0, enumFacing::NORTH);
        piece.fillWithAir(worldIn, chunkBB, 1, 0, -1, 3, 2, -1);

        if (ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB)->getID()) &&
            !ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB)->getID())) {
            piece.setBlockState(worldIn, iBlockState7, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB) == &BlocksInit::GRASS_PATH) {
                piece.setBlockState(worldIn, &BlocksInit::GRASS, 2, -1, -1, chunkBB);
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


    bool Path::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                 const StructureComponent& piece) {

        const Block* pathPrimary = getBiomeSpecificBlockState(&BlocksInit::GRASS_PATH, piece.structureType);
        const Block* overWaterBlock = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);
        const Block* pathSecondary = getBiomeSpecificBlockState(&BlocksInit::GRAVEL, piece.structureType);
        const Block* underPathSecondary = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);

        for (int x = piece.minX; x <= piece.maxX; ++x) {
            for (int z = piece.minZ; z <= piece.maxZ; ++z) {
                Pos3D topBlockPos(x, 64, z);

                if (chunkBB.isVecInside(topBlockPos)) {
                    topBlockPos = worldIn.getTopSolidOrLiquidBlock(topBlockPos).down();

                    if (topBlockPos.getY() < World::getSeaLevel()) {
                        topBlockPos = {topBlockPos.getX(), World::getSeaLevel() - 1, topBlockPos.getZ()};
                    }

                    while (topBlockPos.getY() >= World::getSeaLevel() - 1) {
                        const Block* iblockstate4 = worldIn.getBlock(topBlockPos);

                        if (iblockstate4 == &BlocksInit::GRASS && worldIn.isAirBlock(topBlockPos.up())) {
                            worldIn.setBlock(topBlockPos, pathPrimary);
                            break;
                        }

                        if (ids::isLiquidBlock(iblockstate4->getID())) {
                            worldIn.setBlock(topBlockPos, overWaterBlock);
                            break;
                        }

                        if (iblockstate4 == &BlocksInit::SAND ||
                            iblockstate4 == &BlocksInit::SANDSTONE ||
                            iblockstate4 == &BlocksInit::RED_SANDSTONE) {
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


    bool Torch::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 4 - 1, 0);
            piece.data |= 1 << 16;
        }

        const Block* oakFence = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);

        piece.fillWithAir(worldIn, chunkBB, 0, 0, 0, 2, 3, 1);
        piece.setBlockState(worldIn, oakFence, 1, 0, 0, chunkBB);
        piece.setBlockState(worldIn, oakFence, 1, 1, 0, chunkBB);
        piece.setBlockState(worldIn, oakFence, 1, 2, 0, chunkBB);
        piece.setBlockState(worldIn, BlocksInit::BLACK_WOOL, 1, 3, 0, chunkBB);
        placeTorch(worldIn, enumFacing::EAST, piece, 2, 3, 0, chunkBB);
        placeTorch(worldIn, enumFacing::NORTH, piece, 1, 3, 1, chunkBB);
        placeTorch(worldIn, enumFacing::WEST, piece, 0, 3, 0, chunkBB);
        placeTorch(worldIn, enumFacing::SOUTH, piece, 1, 3, -1, chunkBB);
        return true;
    }


    bool WoodHut::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 6 - 1, 0);
            piece.data |= 1 << 16;
        }

        const Block* iblockstate = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);
        const Block* iblockstate1 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);
        const Block iblockstate2 = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block* iblockstate3 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);
        const Block* iblockstate4 = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);

        piece.fillWithAir(worldIn, chunkBB, 1, 1, 1, 3, 5, 4);
        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, 3, 0, 4, iblockstate, iblockstate, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 2, 0, 3, &BlocksInit::DIRT, false);

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
        piece.setBlockState(worldIn, &BlocksInit::GLASS_PANE, 0, 2, 2,  chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::GLASS_PANE, 3, 2, 2,  chunkBB);

        int tablePosition = piece.data & 3;
        if (tablePosition > 0) {
            piece.setBlockState(worldIn, iblockstate4, tablePosition, 1, 3,  chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::WOODEN_PRESSURE_PLATE, tablePosition, 2, 3,  chunkBB);
        }

        piece.setBlockState(worldIn, &BlocksInit::AIR, 1, 1, 0,  chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::AIR, 1, 2, 0,  chunkBB);
        placeDoor(worldIn, chunkBB, rng, piece, 1, 1, 0, enumFacing::NORTH);

        if (ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, 0, -1,  chunkBB)->getID()) &&
            !ids::isReplaceableBlock(piece.getBlockStateFromPos(worldIn, 1, -1, -1,  chunkBB)->getID())) {
            piece.setBlockState(worldIn, iblockstate2, 1, 0, -1,  chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 1, -1, -1,  chunkBB) == &BlocksInit::GRASS_PATH) {
                piece.setBlockState(worldIn, &BlocksInit::GRASS, 1, -1, -1,  chunkBB);
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


    bool Well::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 3, 0);
            piece.data |= 1 << 16;
        }

        const Block* iblockstate = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);
        const Block* iblockstate1 = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);

        piece.fillWithBlocks(worldIn, chunkBB, 1, 0, 1, 4, 12, 4, iblockstate, &BlocksInit::FLOWING_WATER, false);
        piece.setBlockState(worldIn, BlocksInit::AIR, 2, 12, 2, chunkBB);
        piece.setBlockState(worldIn, BlocksInit::AIR, 3, 12, 2, chunkBB);
        piece.setBlockState(worldIn, BlocksInit::AIR, 2, 12, 3, chunkBB);
        piece.setBlockState(worldIn, BlocksInit::AIR, 3, 12, 3, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 1, 13, 1, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 1, 14, 1, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 4, 13, 1, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 4, 14, 1, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 1, 13, 4, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 1, 14, 4, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 4, 13, 4, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 4, 14, 4, chunkBB);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 15, 1, 4, 15, 4, iblockstate, false);

        for (int i=0; i <= 5; ++i) {
            for (int j=0; j <= 5; ++j) {
                if (j == 0 || j == 5 || i == 0 || i == 5) {
                    piece.setBlockState(worldIn, iblockstate, j, 11, i, chunkBB);
                    piece.clearCurrentPositionBlocksUpwards(worldIn, j, 12, i, chunkBB);
                }
            }
        }

        return true;
    }


    bool House4Garden::addComponentParts(World& worldIn, RNG& rng, BoundingBox& chunkBB,
                                         StructureComponent& piece) {
        if ((piece.data >> 16 & 1) == 0) {
            c_int averageGroundLvl = getAverageGroundLevel(worldIn, chunkBB, piece);
            if (averageGroundLvl < 0) { return true; }
            piece.offset(0, averageGroundLvl - piece.maxY + 6 - 1, 0);
            piece.data |= 1 << 16;
        }

        const Block* iBlockState = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE, piece.structureType);
        const Block* iBlockState1 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD_PLANK, piece.structureType);
        const Block iBlockState2 = getBiomeSpecificBlockState(&BlocksInit::COBBLESTONE_STAIRS, piece.structureType)
                                           ->getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
        const Block* iBlockState3 = getBiomeSpecificBlockState(&BlocksInit::OAK_WOOD, piece.structureType);
        const Block* iBlockState4 = getBiomeSpecificBlockState(&BlocksInit::OAK_FENCE, piece.structureType);

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
        const Block* glassPane = &BlocksInit::GLASS_PANE;
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

        if (piece.getBlockStateFromPos(worldIn, 2, 0, -1, chunkBB)->getID() != ids::AIR_ID &&
            piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB)->getID() != ids::AIR_ID) { // material air
            piece.setBlockState(worldIn, iBlockState2, 2, 0, -1, chunkBB);

            if (piece.getBlockStateFromPos(worldIn, 2, -1, -1, chunkBB)->getID() == ids::GRASS_PATH_ID) {
                piece.setBlockState(worldIn, BlocksInit::GRASS, 2, -1, -1, chunkBB);
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
            c_auto ladderSouth = BlocksInit::LADDER.getStateFromMeta(
                    states::Ladder::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));
            piece.setBlockState(worldIn, ladderSouth, 3, 1, 3, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 3, 2, 3, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 3, 3, 3, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 3, 4, 3, chunkBB);
        }

        placeTorch(worldIn, enumFacing::NORTH, piece, 2, 3, 1, chunkBB);

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

#pragma clang diagnostic pop