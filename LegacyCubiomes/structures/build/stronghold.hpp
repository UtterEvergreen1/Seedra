#pragma once

#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"


namespace build::stronghold {

    static Stones STRONGHOLD_STONES = Stones();



    enum Door { OPENING, WOOD_DOOR, GRATES, IRON_DOOR };


    static void placeDoor(World& worldIn, MU RNG& rng, BoundingBox structureBB,
                          StructureComponent& piece, Door door, int x, int y, int z) {
        switch (door) {
            case OPENING:
                piece.fillWithBlocks(worldIn, structureBB, x, y, z, x + 3 - 1, y + 3 - 1, z,
                               &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
                break;

            case WOOD_DOOR:
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x, y, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 1, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 2, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 2, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 2, y, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_DOOR_BLOCK, x + 1, y, z, structureBB);
                // .withProperty(BlockDoor.HALF, BlockDoor.EnumDoorHalf.UPPER)
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_DOOR_BLOCK, x + 1, y + 1, z, structureBB);
                break;

            case GRATES:
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, x + 1, y, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, x + 1, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x, y, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x + 1, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x + 2, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x + 2, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, x + 2, y, z, structureBB);
                break;

            case IRON_DOOR:
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x, y, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 1, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 2, y + 2, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 2, y + 1, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, x + 2, y, z, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_DOOR_BLOCK, x + 1, y, z, structureBB);
                // .withProperty(BlockDoor.HALF, BlockDoor.EnumDoorHalf.UPPER)
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_DOOR_BLOCK, x + 1, y + 1, z, structureBB);
                // .withProperty(BlockButton.FACING, FACING::NORTH)
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BUTTON, x + 2, y + 1, z + 1, structureBB);
                // .withProperty(BlockButton.FACING, FACING::SOUTH)
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BUTTON, x + 2, y + 1, z - 1, structureBB);
        }
    }


    MU static Door getRandomDoor(int data) {
        // int i = rng.nextInt(5);
        switch (data) {
            case 0:
            case 1:
            default:
                return Door::OPENING;
            case 2:
                return Door::WOOD_DOOR;
            case 3:
                return Door::GRATES;
            case 4:
                return Door::IRON_DOOR;
        }
    }



    class MU ChestCorridor final {
    public:
        MU ChestCorridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);
                bool hasMadeChest = false;


                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 4, 4, 6, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 1, 0);
                placeDoor(worldIn, rng, structureBB, piece, Door::OPENING, 1, 1, 6);
                piece.fillWithBlocks(worldIn, structureBB, 3, 1, 2, 3, 1, 4, &lce::blocks::BlocksInit::STONE_BRICKS,
                               &lce::blocks::BlocksInit::STONE_BRICKS, false);
                // .getStateFromMeta(BlockStoneSlab.EnumType.SMOOTHBRICK.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 3, 1, 1, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.SMOOTHBRICK.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 3, 1, 5, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.SMOOTHBRICK.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 3, 2, 2, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.SMOOTHBRICK.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 3, 2, 4, structureBB);

                for (int i = 2; i <= 4; ++i) {
                    // .getStateFromMeta(BlockStoneSlab.EnumType.SMOOTHBRICK.getMetadata())
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 2, 1, i, structureBB);
                }


                if (!hasMadeChest && structureBB.isVecInside({piece.getWorldX(3, 3), piece.getWorldY(2), piece.getWorldZ(3, 3)})) {
                    hasMadeChest = true;
                    // generateChest(worldIn, structureBB, rng, 3, 2, 3, LootTableList.CHESTS_STRONGHOLD_CORRIDOR);
                }


                return true;
            }
        }
    };

    class Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                int steps = piece.facing != FACING::NORTH && piece.facing != FACING::SOUTH ? piece.getXSize() : piece.getZSize();

                for (int i = 0; i < steps; ++i) {
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 0, 0, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 0, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 2, 0, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 0, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 4, 0, i, structureBB);

                    for (int j = 1; j <= 3; ++j) {
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 0, j, i, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 1, j, i, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, j, i, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 3, j, i, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 4, j, i, structureBB);
                    }

                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 0, 4, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 4, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 2, 4, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 4, i, structureBB);
                    piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 4, 4, i, structureBB);
                }

                return true;
            }
        }
    };

    class Crossing final {
    public:
        MU Crossing() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);
                bool leftLow = piece.data & 1;
                bool leftHigh = piece.data & 2;
                bool rightLow = piece.data & 4;
                bool rightHigh = piece.data & 8;

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 9, 8, 10, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 4, 3, 0);

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                if (leftLow) { piece.fillWithBlocks(worldIn, structureBB, 0, 3, 1, 0, 5, 3, air, air, false); }
                if (rightLow) { piece.fillWithBlocks(worldIn, structureBB, 9, 3, 1, 9, 5, 3, air, air, false); }
                if (leftHigh) { piece.fillWithBlocks(worldIn, structureBB, 0, 5, 7, 0, 7, 9, air, air, false); }
                if (rightHigh) { piece.fillWithBlocks(worldIn, structureBB, 9, 5, 7, 9, 7, 9, air, air, false); }

                piece.fillWithBlocks(worldIn, structureBB, 5, 1, 10, 7, 3, 10, air, air, false);
                piece.fillWithRandomizedStones(worldIn, structureBB, 1, 2, 1, 8, 2, 6, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 4, 1, 5, 4, 4, 9, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 8, 1, 5, 8, 4, 9, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 1, 4, 7, 3, 4, 9, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 1, 3, 5, 3, 3, 6, false, rng, STRONGHOLD_STONES);
                piece.fillWithBlocks(worldIn, structureBB, 1, 3, 4, 3, 3, 4, &lce::blocks::BlocksInit::STONE_SLAB,
                               &lce::blocks::BlocksInit::STONE_SLAB, false);
                piece.fillWithBlocks(worldIn, structureBB, 1, 4, 6, 3, 4, 6, &lce::blocks::BlocksInit::STONE_SLAB,
                               &lce::blocks::BlocksInit::STONE_SLAB, false);
                piece.fillWithRandomizedStones(worldIn, structureBB, 5, 1, 7, 7, 1, 8, false, rng, STRONGHOLD_STONES);
                piece.fillWithBlocks(worldIn, structureBB, 5, 1, 9, 7, 1, 9, &lce::blocks::BlocksInit::STONE_SLAB,
                               &lce::blocks::BlocksInit::STONE_SLAB, false);
                piece.fillWithBlocks(worldIn, structureBB, 5, 2, 7, 7, 2, 7, &lce::blocks::BlocksInit::STONE_SLAB,
                               &lce::blocks::BlocksInit::STONE_SLAB, false);
                piece.fillWithBlocks(worldIn, structureBB, 4, 5, 7, 4, 5, 9, &lce::blocks::BlocksInit::STONE_SLAB,
                               &lce::blocks::BlocksInit::STONE_SLAB, false);
                piece.fillWithBlocks(worldIn, structureBB, 8, 5, 7, 8, 5, 9, &lce::blocks::BlocksInit::STONE_SLAB,
                               &lce::blocks::BlocksInit::STONE_SLAB, false);
                piece.fillWithBlocks(worldIn, structureBB, 5, 5, 7, 7, 5, 9, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB,
                               &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, false);
                // .withProperty(BlockTorch.FACING, FACING::SOUTH)
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::TORCH, 6, 5, 6, structureBB);
                return true;
            }
        }
    };

    class MU LeftTurn final {
    public:
        MU LeftTurn() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 4, 4, 4, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 1, 0);

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                if (piece.facing != FACING::NORTH && piece.facing != FACING::EAST) {
                    piece.fillWithBlocks(worldIn, structureBB, 4, 1, 1, 4, 3, 3, air, air, false);
                } else {
                    piece.fillWithBlocks(worldIn, structureBB, 0, 1, 1, 0, 3, 3, air, air, false);
                }


                return true;
            }
        }
    };

    class MU Library final {
    public:
        MU Library() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);
                bool isLargeRoom = piece.data & 1;

                int i = 11;

                if (!isLargeRoom) { i = 6; }

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 13, i - 1, 14, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 4, 1, 0);
                piece.func_189914_a(worldIn, structureBB, rng, 0.07F, 2, 1, 1, 11, 4, 13, &lce::blocks::BlocksInit::COBWEB,
                              &lce::blocks::BlocksInit::COBWEB, false, 0);
                // int j = 1;
                // int k = 12;

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;
                const lce::blocks::Block* plank = &lce::blocks::BlocksInit::OAK_WOOD_PLANK;
                const lce::blocks::Block* bookshelf = &lce::blocks::BlocksInit::BOOKSHELF;
                const lce::blocks::Block* fence = &lce::blocks::BlocksInit::OAK_FENCE;
                const lce::blocks::Block* torch = &lce::blocks::BlocksInit::TORCH;

                for (int l = 1; l <= 13; ++l) {
                    if ((l - 1) % 4 == 0) {
                        piece.fillWithBlocks(worldIn, structureBB, 1, 1, l, 1, 4, l, plank, plank, false);
                        piece.fillWithBlocks(worldIn, structureBB, 12, 1, l, 12, 4, l, plank, plank, false);
                        // .withProperty(BlockTorch.FACING, FACING::EAST)
                        piece.setBlockState(worldIn, torch, 2, 3, l, structureBB);
                        // .withProperty(BlockTorch.FACING, FACING::WEST)
                        piece.setBlockState(worldIn, torch, 11, 3, l, structureBB);

                        if (isLargeRoom) {
                            piece.fillWithBlocks(worldIn, structureBB, 1, 6, l, 1, 9, l, plank, plank, false);
                            piece.fillWithBlocks(worldIn, structureBB, 12, 6, l, 12, 9, l, plank, plank, false);
                        }
                    } else {
                        piece.fillWithBlocks(worldIn, structureBB, 1, 1, l, 1, 4, l, bookshelf, bookshelf, false);
                        piece.fillWithBlocks(worldIn, structureBB, 12, 1, l, 12, 4, l, bookshelf, bookshelf, false);

                        if (isLargeRoom) {
                            piece.fillWithBlocks(worldIn, structureBB, 1, 6, l, 1, 9, l, bookshelf, bookshelf, false);
                            piece.fillWithBlocks(worldIn, structureBB, 12, 6, l, 12, 9, l, bookshelf, bookshelf, false);
                        }
                    }
                }

                for (int k1 = 3; k1 < 12; k1 += 2) {
                    piece.fillWithBlocks(worldIn, structureBB, 3, 1, k1, 4, 3, k1, bookshelf, bookshelf, false);
                    piece.fillWithBlocks(worldIn, structureBB, 6, 1, k1, 7, 3, k1, bookshelf, bookshelf, false);
                    piece.fillWithBlocks(worldIn, structureBB, 9, 1, k1, 10, 3, k1, bookshelf, bookshelf, false);
                }

                if (isLargeRoom) {
                    piece.fillWithBlocks(worldIn, structureBB, 1, 5, 1, 3, 5, 13, plank, plank, false);
                    piece.fillWithBlocks(worldIn, structureBB, 10, 5, 1, 12, 5, 13, plank, plank, false);
                    piece.fillWithBlocks(worldIn, structureBB, 4, 5, 1, 9, 5, 2, plank, plank, false);
                    piece.fillWithBlocks(worldIn, structureBB, 4, 5, 12, 9, 5, 13, plank, plank, false);
                    piece.setBlockState(worldIn, plank, 9, 5, 11, structureBB);
                    piece.setBlockState(worldIn, plank, 8, 5, 11, structureBB);
                    piece.setBlockState(worldIn, plank, 9, 5, 10, structureBB);
                    piece.fillWithBlocks(worldIn, structureBB, 3, 6, 2, 3, 6, 12, fence, fence, false);
                    piece.fillWithBlocks(worldIn, structureBB, 10, 6, 2, 10, 6, 10, fence, fence, false);
                    piece.fillWithBlocks(worldIn, structureBB, 4, 6, 2, 9, 6, 2, fence, fence, false);
                    piece.fillWithBlocks(worldIn, structureBB, 4, 6, 12, 8, 6, 12, fence, fence, false);
                    piece.setBlockState(worldIn, fence, 9, 6, 11, structureBB);
                    piece.setBlockState(worldIn, fence, 8, 6, 11, structureBB);
                    piece.setBlockState(worldIn, fence, 9, 6, 10, structureBB);
                    // .withProperty(BlockLadder.FACING, FACING::SOUTH);
                    const lce::blocks::Block* iblockstate1 = &lce::blocks::BlocksInit::LADDER;
                    piece.setBlockState(worldIn, iblockstate1, 10, 1, 13, structureBB);
                    piece.setBlockState(worldIn, iblockstate1, 10, 2, 13, structureBB);
                    piece.setBlockState(worldIn, iblockstate1, 10, 3, 13, structureBB);
                    piece.setBlockState(worldIn, iblockstate1, 10, 4, 13, structureBB);
                    piece.setBlockState(worldIn, iblockstate1, 10, 5, 13, structureBB);
                    piece.setBlockState(worldIn, iblockstate1, 10, 6, 13, structureBB);
                    piece.setBlockState(worldIn, iblockstate1, 10, 7, 13, structureBB);
                    // int i1 = 7;
                    // int j1 = 7;
                    piece.setBlockState(worldIn, fence, 6, 9, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 7, 9, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 6, 8, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 7, 8, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 6, 7, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 7, 7, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 5, 7, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 8, 7, 7, structureBB);
                    piece.setBlockState(worldIn, fence, 6, 7, 6, structureBB);
                    piece.setBlockState(worldIn, fence, 6, 7, 8, structureBB);
                    piece.setBlockState(worldIn, fence, 7, 7, 6, structureBB);
                    piece.setBlockState(worldIn, fence, 7, 7, 8, structureBB);
                    // .withProperty(BlockTorch.FACING, FACING::UP)
                    const lce::blocks::Block* iblockstate = torch;
                    piece.setBlockState(worldIn, iblockstate, 5, 8, 7, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 8, 8, 7, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 6, 8, 6, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 6, 8, 8, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 7, 8, 6, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 7, 8, 8, structureBB);
                }

                // generateChest(worldIn, structureBB, rng, 3, 3, 5, LootTableList.CHESTS_STRONGHOLD_LIBRARY);

                if (isLargeRoom) {
                    piece.setBlockState(worldIn, air, 12, 9, 1, structureBB);
                    // generateChest(worldIn, structureBB, rng, 12, 8, 1, LootTableList.CHESTS_STRONGHOLD_LIBRARY);
                }

                return true;
            }
        }
    };

    class MU PortalRoom final {
    public:
        MU PortalRoom() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            bool hasSpawner = false;


            piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 10, 7, 15, false, rng, STRONGHOLD_STONES);
            placeDoor(worldIn, rng, structureBB, piece, Door::GRATES, 4, 1, 0);
            int i = 6;
            piece.fillWithRandomizedStones(worldIn, structureBB, 1, i, 1, 1, i, 14, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 9, i, 1, 9, i, 14, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 2, i, 1, 8, i, 2, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 2, i, 14, 8, i, 14, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 1, 1, 1, 2, 1, 4, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 8, 1, 1, 9, 1, 4, false, rng, STRONGHOLD_STONES);
            piece.fillWithBlocks(worldIn, structureBB, 1, 1, 1, 1, 1, 3, &lce::blocks::BlocksInit::FLOWING_LAVA,
                           &lce::blocks::BlocksInit::FLOWING_LAVA, false);
            piece.fillWithBlocks(worldIn, structureBB, 9, 1, 1, 9, 1, 3, &lce::blocks::BlocksInit::FLOWING_LAVA,
                           &lce::blocks::BlocksInit::FLOWING_LAVA, false);
            piece.fillWithRandomizedStones(worldIn, structureBB, 3, 1, 8, 7, 1, 12, false, rng, STRONGHOLD_STONES);
            piece.fillWithBlocks(worldIn, structureBB, 4, 1, 9, 6, 1, 11, &lce::blocks::BlocksInit::FLOWING_LAVA,
                           &lce::blocks::BlocksInit::FLOWING_LAVA, false);

            for (int j = 3; j < 14; j += 2) {
                piece.fillWithBlocks(worldIn, structureBB, 0, 3, j, 0, 4, j, &lce::blocks::BlocksInit::IRON_BARS,
                               &lce::blocks::BlocksInit::IRON_BARS, false);
                piece.fillWithBlocks(worldIn, structureBB, 10, 3, j, 10, 4, j, &lce::blocks::BlocksInit::IRON_BARS,
                               &lce::blocks::BlocksInit::IRON_BARS, false);
            }

            for (int i1 = 2; i1 < 9; i1 += 2) {
                piece.fillWithBlocks(worldIn, structureBB, i1, 3, 15, i1, 4, 15, &lce::blocks::BlocksInit::IRON_BARS,
                               &lce::blocks::BlocksInit::IRON_BARS, false);
            }

            // .withProperty(BlockStairs.FACING, FACING::NORTH)
            const lce::blocks::Block* iblockstate3 = &lce::blocks::BlocksInit::STONE_BRICK_STAIRS;
            piece.fillWithRandomizedStones(worldIn, structureBB, 4, 1, 5, 6, 1, 7, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 4, 2, 6, 6, 2, 7, false, rng, STRONGHOLD_STONES);
            piece.fillWithRandomizedStones(worldIn, structureBB, 4, 3, 7, 6, 3, 7, false, rng, STRONGHOLD_STONES);

            for (int k = 4; k <= 6; ++k) {
                piece.setBlockState(worldIn, iblockstate3, k, 1, 4, structureBB);
                piece.setBlockState(worldIn, iblockstate3, k, 2, 5, structureBB);
                piece.setBlockState(worldIn, iblockstate3, k, 3, 6, structureBB);
            }

            // .withProperty(BlockEndPortalFrame.FACING, FACING::NORTH);
            const lce::blocks::Block* iblockstate4 = &lce::blocks::BlocksInit::END_PORTAL_FRAME;
            // .withProperty(BlockEndPortalFrame.FACING, FACING::SOUTH);
            const lce::blocks::Block* iblockstate = &lce::blocks::BlocksInit::END_PORTAL_FRAME;
            // .withProperty(BlockEndPortalFrame.FACING, FACING::EAST);
            const lce::blocks::Block* iblockstate1 = &lce::blocks::BlocksInit::END_PORTAL_FRAME;
            // .withProperty(BlockEndPortalFrame.FACING, FACING::WEST);
            const lce::blocks::Block* iblockstate2 = &lce::blocks::BlocksInit::END_PORTAL_FRAME;

            /*
            boolean flag = true;
            boolean[] aboolean = new boolean[12];

            for (int l = 0; l < aboolean.length; ++l) {
                aboolean[l] = rng.nextFloat() > 0.9F;
                flag &= aboolean[l];
            }
             */

            piece.setBlockState(worldIn, iblockstate4->getStateFromMeta(0), 4, 3, 8, structureBB);
            piece.setBlockState(worldIn, iblockstate4->getStateFromMeta(0), 5, 3, 8, structureBB);
            piece.setBlockState(worldIn, iblockstate4->getStateFromMeta(0), 6, 3, 8, structureBB);
            piece.setBlockState(worldIn, iblockstate->getStateFromMeta(0), 4, 3, 12, structureBB);
            piece.setBlockState(worldIn, iblockstate->getStateFromMeta(0), 5, 3, 12, structureBB);
            piece.setBlockState(worldIn, iblockstate->getStateFromMeta(0), 6, 3, 12, structureBB);
            piece.setBlockState(worldIn, iblockstate1->getStateFromMeta(0), 3, 3, 9, structureBB);
            piece.setBlockState(worldIn, iblockstate1->getStateFromMeta(0), 3, 3, 10, structureBB);
            piece.setBlockState(worldIn, iblockstate1->getStateFromMeta(0), 3, 3, 11, structureBB);
            piece.setBlockState(worldIn, iblockstate2->getStateFromMeta(0), 7, 3, 9, structureBB);
            piece.setBlockState(worldIn, iblockstate2->getStateFromMeta(0), 7, 3, 10, structureBB);
            piece.setBlockState(worldIn, iblockstate2->getStateFromMeta(0), 7, 3, 11, structureBB);

            /*
            if (flag) {
                const lce::blocks::Block* iblockstate5 = &lce::blocks::BlocksInit::END_PORTAL;
                piece.setBlockState(worldIn, iblockstate5, 4, 3, 9, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 5, 3, 9, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 6, 3, 9, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 4, 3, 10, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 5, 3, 10, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 6, 3, 10, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 4, 3, 11, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 5, 3, 11, structureBB);
                piece.setBlockState(worldIn, iblockstate5, 6, 3, 11, structureBB);
            }
             */



            if (!hasSpawner) {
                i = piece.getWorldY(3);
                Pos3D blockPos(piece.getWorldX(5, 6), i, piece.getWorldZ(5, 6));

                if (structureBB.isVecInside(blockPos)) {
                    hasSpawner = true;
                    worldIn.setBlock(blockPos, &lce::blocks::BlocksInit::MONSTER_SPAWNER); // 2
                    /*
                    TileEntity tileentity = worldIn.getTileEntity(blockPos);

                    if (tileentity instanceof TileEntityMobSpawner) {
                        ((TileEntityMobSpawner) tileentity)
                                .getSpawnerBaseLogic()
                                .func_190894_a(EntityList.func_191306_a(EntitySilverfish.class));
                    }
                     */
                }
            }


            return true;
        }
    };

    class MU Prison final {
    public:
        MU Prison() = delete;


        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 8, 4, 10, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 1, 0);
                piece.fillWithBlocks(worldIn, structureBB, 1, 1, 10, 3, 3, 10, &lce::blocks::BlocksInit::AIR,
                               &lce::blocks::BlocksInit::AIR, false);
                piece.fillWithRandomizedStones(worldIn, structureBB, 4, 1, 1, 4, 3, 1, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 4, 1, 3, 4, 3, 3, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 4, 1, 7, 4, 3, 7, false, rng, STRONGHOLD_STONES);
                piece.fillWithRandomizedStones(worldIn, structureBB, 4, 1, 9, 4, 3, 9, false, rng, STRONGHOLD_STONES);
                piece.fillWithBlocks(worldIn, structureBB, 4, 1, 4, 4, 3, 6, &lce::blocks::BlocksInit::IRON_BARS,
                               &lce::blocks::BlocksInit::IRON_BARS, false);
                piece.fillWithBlocks(worldIn, structureBB, 5, 1, 5, 7, 3, 5, &lce::blocks::BlocksInit::IRON_BARS,
                               &lce::blocks::BlocksInit::IRON_BARS, false);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, 4, 3, 2, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, 4, 3, 8, structureBB);
                // .withProperty(BlockDoor.FACING, FACING::WEST)
                const lce::blocks::Block* iblockstate = &lce::blocks::BlocksInit::IRON_DOOR_BLOCK;
                // .withProperty(BlockDoor.FACING, FACING::WEST).withProperty(BlockDoor.HALF, BlockDoor.EnumDoorHalf.UPPER)
                const lce::blocks::Block* iblockstate1 = &lce::blocks::BlocksInit::IRON_DOOR_BLOCK;
                piece.setBlockState(worldIn, iblockstate, 4, 1, 2, structureBB);
                piece.setBlockState(worldIn, iblockstate1, 4, 2, 2, structureBB);
                piece.setBlockState(worldIn, iblockstate, 4, 1, 8, structureBB);
                piece.setBlockState(worldIn, iblockstate1, 4, 2, 8, structureBB);
                return true;
            }
        }
    };

    class MU RightTurn final {
    public:
        MU RightTurn() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 4, 4, 4, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 1, 0);

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;
                if (piece.facing != FACING::NORTH && piece.facing != FACING::EAST) {
                    piece.fillWithBlocks(worldIn, structureBB, 0, 1, 1, 0, 3, 3, air, air, false);
                } else {
                    piece.fillWithBlocks(worldIn, structureBB, 4, 1, 1, 4, 3, 3, air, air, false);
                }

                return true;
            }
        }
    };

    class MU RoomCrossing final {
    public:
        MU RoomCrossing() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                // TODO: these might need swapped
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);
                int roomType = piece.data & 7;

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 10, 6, 10, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 4, 1, 0);
                piece.fillWithBlocks(worldIn, structureBB, 4, 1, 10, 6, 3, 10, air, air, false);
                piece.fillWithBlocks(worldIn, structureBB, 0, 1, 4, 0, 3, 6, air, air, false);
                piece.fillWithBlocks(worldIn, structureBB, 10, 1, 4, 10, 3, 6, air, air, false);

                switch (roomType) {
                    default:
                        break;
                    case 0:
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 5, 1, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 5, 2, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 5, 3, 5, structureBB);
                        // .withProperty(BlockTorch.FACING, FACING::WEST)
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::TORCH, 4, 3, 5, structureBB);
                        // .withProperty(BlockTorch.FACING, FACING::EAST)
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::TORCH, 6, 3, 5, structureBB);
                        // .withProperty(BlockTorch.FACING, FACING::SOUTH)
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::TORCH, 5, 3, 4, structureBB);
                        // .withProperty(BlockTorch.FACING, FACING::NORTH)
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::TORCH, 5, 3, 6, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 4, 1, 4, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 4, 1, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 4, 1, 6, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 6, 1, 4, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 6, 1, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 6, 1, 6, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 5, 1, 4, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 5, 1, 6, structureBB);
                        break;

                    case 1:
                        for (int i1 = 0; i1 < 5; ++i1) {
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 1, 3 + i1, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 7, 1, 3 + i1, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3 + i1, 1, 3, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3 + i1, 1, 7, structureBB);
                        }

                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 5, 1, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 5, 2, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 5, 3, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::FLOWING_WATER, 5, 4, 5, structureBB);
                        break;

                    case 2:
                        for (int i = 1; i <= 9; ++i) {
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 1, 3, i, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 9, 3, i, structureBB);
                        }

                        for (int j = 1; j <= 9; ++j) {
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, j, 3, 1, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, j, 3, 9, structureBB);
                        }

                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 5, 1, 4, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 5, 1, 6, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 5, 3, 4, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 5, 3, 6, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 4, 1, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 6, 1, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 4, 3, 5, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 6, 3, 5, structureBB);

                        for (int k = 1; k <= 3; ++k) {
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 4, k, 4, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 6, k, 4, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 4, k, 6, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::COBBLESTONE, 6, k, 6, structureBB);
                        }

                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::TORCH, 5, 3, 5, structureBB);

                        for (int l = 2; l <= 8; ++l) {
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 2, 3, l, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 3, 3, l, structureBB);

                            if (l <= 3 || l >= 7) {
                                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 4, 3, l, structureBB);
                                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 5, 3, l, structureBB);
                                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 6, 3, l, structureBB);
                            }

                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 7, 3, l, structureBB);
                            piece.setBlockState(worldIn, &lce::blocks::BlocksInit::OAK_WOOD_PLANK, 8, 3, l, structureBB);
                        }

                        // .withProperty(BlockLadder.FACING, FACING::WEST)
                        const lce::blocks::Block* iblockstate = &lce::blocks::BlocksInit::LADDER;
                        piece.setBlockState(worldIn, iblockstate, 9, 1, 3, structureBB);
                        piece.setBlockState(worldIn, iblockstate, 9, 2, 3, structureBB);
                        piece.setBlockState(worldIn, iblockstate, 9, 3, 3, structureBB);
                        // generateChest(worldIn, structureBB, rng, 3, 4, 8, LootTableList.CHESTS_STRONGHOLD_CROSSING);
                }

                return true;
            }
        }
    };

    class MU Stairs final {
    public:
        MU Stairs() = delete;


        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 4, 10, 4, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 7, 0);
                placeDoor(worldIn, rng, structureBB, piece, Door::OPENING, 1, 1, 4);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 2, 6, 1, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 5, 1, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.STONE.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 1, 6, 1, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 5, 2, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 4, 3, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.STONE.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 1, 5, 3, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 2, 4, 3, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 3, 3, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.STONE.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 3, 4, 3, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 3, 2, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 2, 1, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.STONE.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 3, 3, 1, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 2, 2, 1, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 1, 1, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.STONE.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 1, 2, 1, structureBB);
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 1, 2, structureBB);
                // .getStateFromMeta(BlockStoneSlab.EnumType.STONE.getMetadata())
                piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_SLAB, 1, 1, 3, structureBB);
                return true;
            }
        }
    };


    class MU StairsStraight final {
    public:
        MU StairsStraight() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 4, 10, 7, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 7, 0);
                placeDoor(worldIn, rng, structureBB, piece, Door::OPENING, 1, 1, 7);
                // .withProperty(BlockStairs.FACING, FACING::SOUTH)
                const lce::blocks::Block* iblockstate = &lce::blocks::BlocksInit::STONE_BRICK_STAIRS;

                for (int i = 0; i < 6; ++i) {
                    piece.setBlockState(worldIn, iblockstate, 1, 6 - i, 1 + i, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 2, 6 - i, 1 + i, structureBB);
                    piece.setBlockState(worldIn, iblockstate, 3, 6 - i, 1 + i, structureBB);

                    if (i < 5) {
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 1, 5 - i, 1 + i, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 2, 5 - i, 1 + i, structureBB);
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::STONE_BRICKS, 3, 5 - i, 1 + i, structureBB);
                    }
                }

                return true;
            }
        }
    };

    class MU Straight final {
    public:
        MU Straight() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                Door entryDoor = getRandomDoor((piece.data >> 16) & 7);

                piece.fillWithRandomizedStones(worldIn, structureBB, 0, 0, 0, 4, 4, 6, true, rng, STRONGHOLD_STONES);
                placeDoor(worldIn, rng, structureBB, piece, entryDoor, 1, 1, 0);
                placeDoor(worldIn, rng, structureBB, piece, Door::OPENING, 1, 1, 6);
                // .withProperty(BlockTorch.FACING, FACING::EAST)
                const lce::blocks::Block* iblockstate = &lce::blocks::BlocksInit::TORCH;
                // .withProperty(BlockTorch.FACING, FACING::WEST)
                const lce::blocks::Block* iblockstate1 = &lce::blocks::BlocksInit::TORCH;
                piece.randomlyPlaceBlock(worldIn, structureBB, rng, 0.1F, 1, 2, 1, iblockstate);
                piece.randomlyPlaceBlock(worldIn, structureBB, rng, 0.1F, 3, 2, 1, iblockstate1);
                piece.randomlyPlaceBlock(worldIn, structureBB, rng, 0.1F, 1, 2, 5, iblockstate);
                piece.randomlyPlaceBlock(worldIn, structureBB, rng, 0.1F, 3, 2, 5, iblockstate1);

                bool expandsX = piece.data & 1;
                bool expandsZ = piece.data & 2;

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                if (expandsX) {
                    piece.fillWithBlocks(worldIn, structureBB, 0, 1, 2, 0, 3, 4, air, air, false);
                }

                if (expandsZ) {
                    piece.fillWithBlocks(worldIn, structureBB, 4, 1, 2, 4, 3, 4, air, air, false);
                }

                return true;
            }
        }
    };



    MU static bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {

        bool result = false;

        switch (piece.type) {
            case PieceType::PT_Stronghold_Straight:
                result = Straight::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_PrisonHall:
                result = Prison::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_LeftTurn:
                result = LeftTurn::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_RightTurn:
                result = RightTurn::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_RoomCrossing:
                result = RoomCrossing::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_StraightStairsDown:
                result = StairsStraight::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_StairsDown:
                result = Stairs::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_FiveCrossing:
                result = Crossing::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_ChestCorridor:
                result = ChestCorridor::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_Library:
                result = Library::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_PortalRoom:
                result = PortalRoom::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Stronghold_FillerCorridor:
                result = Corridor::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            default:;
        }
        return result;
    }












} // namespace build::stronghold