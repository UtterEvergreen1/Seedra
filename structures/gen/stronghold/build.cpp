#include <cmath>

#include "common/rng.hpp"
#include "stronghold.hpp"

#include "enchants/__include.hpp"
#include "lce/blocks/blockStates.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;
using namespace lce::blocks::states;


namespace build::stronghold {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    void placeDoor(World& worldIn, const BoundingBox& chunkBB, const StructureComponent& piece,
                   const Door door, const int x, const int y, const int z) {
        switch (door) {
            case Door::OPENING:
                piece.fillWithAir(worldIn, chunkBB, x, y, z, x + 3 - 1, y + 3 - 1, z);
                break;

            case Door::WOOD_DOOR: {
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x, y, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 1, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 2, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 2, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 2, y, z, chunkBB);

                constexpr auto door_base = lce::BlocksInit::OAK_DOOR_BLOCK.getState();

                c_auto door_bottom = door_base.getStateFromMeta(states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
                piece.setBlockState(worldIn, door_bottom, x + 1, y, z, chunkBB);

                c_auto door_top = door_base.getStateFromMeta(states::DoorUpper::withProperty());
                piece.setBlockState(worldIn, door_top, x + 1, y + 1, z, chunkBB);
                break;
            }

            case Door::GRATES:
                piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), x + 1, y, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), x + 1, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x, y, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x + 1, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x + 2, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x + 2, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), x + 2, y, z, chunkBB);
                break;

            case Door::IRON_DOOR: {
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x, y, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 1, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 2, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 2, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), x + 2, y, z, chunkBB);

                c_auto doorBlockLower = lce::BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                        DoorLower::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
                piece.setBlockState(worldIn, doorBlockLower, x + 1, y, z, chunkBB);

                c_auto doorBlockUpper = lce::BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                        DoorUpper::withProperty());
                piece.setBlockState(worldIn, doorBlockUpper, x + 1, y + 1, z, chunkBB);

                c_auto buttonBlock1 = lce::BlocksInit::STONE_BUTTON.getStateFromMeta(
                        Button::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));
                piece.setBlockState(worldIn, buttonBlock1, x + 2, y + 1, z + 1, chunkBB);

                c_auto buttonBlock2 = lce::BlocksInit::STONE_BUTTON.getStateFromMeta(
                        Button::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));
                piece.setBlockState(worldIn, buttonBlock2, x + 2, y + 1, z - 1, chunkBB);
            }
        }
    }


    bool ChestCorridor::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                          const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);
        bool hasMadeChest = false;


        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 4, 6, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 1, 0);
        placeDoor(worldIn, chunkBB, piece, Door::OPENING, 1, 1, 6);
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 2, 3, 1, 4, lce::BlocksInit::STONE_BRICKS.getState(), false);

        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICK_SLAB.getState(), 3, 1, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICK_SLAB.getState(), 3, 1, 5, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICK_SLAB.getState(), 3, 2, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICK_SLAB.getState(), 3, 2, 4, chunkBB);

        for (int i = 2; i <= 4; ++i) {
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICK_SLAB.getState(), 2, 1, i, chunkBB);
        }

        Pos3D chestPos = piece.getWorldXYZ(3, 2, 3);
        if (!hasMadeChest && chunkBB.isVecInside(chestPos)) {
            hasMadeChest = true;
            // generateChest(worldIn, chunkBB, rng, 3, 2, 3, LootTableList.CHESTS_STRONGHOLD_CORRIDOR);
            piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(),  3, 2, 3, chunkBB);
            rng.nextLong();
        }

        return true;
    }


    bool Corridor::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        c_int steps = piece.facing != EnumFacing::NORTH && piece.facing != EnumFacing::SOUTH ? piece.getXSize() : piece.getZSize();

        for (int i = 0; i < steps; ++i) {
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 0, 0, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 0, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 2, 0, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 0, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 4, 0, i, chunkBB);

            for (int j = 1; j <= 3; ++j) {
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 0, j, i, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 1, j, i, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 2, j, i, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::AIR.getState(), 3, j, i, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 4, j, i, chunkBB);
            }

            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 0, 4, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 4, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 2, 4, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 4, i, chunkBB);
            piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 4, 4, i, chunkBB);

        }
        return true;
    }


    bool Crossing::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);
        c_bool leftLow = piece.data & 1;
        c_bool leftHigh = piece.data & 2;
        c_bool rightLow = piece.data & 4;
        c_bool rightHigh = piece.data & 8;

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 9, 8, 10, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 4, 3, 0);

        if (leftLow) { piece.fillWithAir(worldIn, chunkBB, 0, 3, 1, 0, 5, 3); }
        if (rightLow) { piece.fillWithAir(worldIn, chunkBB, 9, 3, 1, 9, 5, 3); }
        if (leftHigh) { piece.fillWithAir(worldIn, chunkBB, 0, 5, 7, 0, 7, 9); }
        if (rightHigh) { piece.fillWithAir(worldIn, chunkBB, 9, 5, 7, 9, 7, 9); }

        piece.fillWithAir(worldIn, chunkBB, 5, 1, 10, 7, 3, 10);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 1, 2, 1, 8, 2, 6, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 5, 4, 4, 9, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 8, 1, 5, 8, 4, 9, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 1, 4, 7, 3, 4, 9, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 1, 3, 5, 3, 3, 6, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 3, 4, 3, 3, 4, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 4, 6, 3, 4, 6, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 5, 1, 7, 7, 1, 8, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 1, 9, 7, 1, 9, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 2, 7, 7, 2, 7, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 5, 7, 4, 5, 9, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 5, 7, 8, 5, 9, lce::BlocksInit::STONE_SLAB.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 5, 7, 7, 5, 9, lce::BlocksInit::DOUBLE_STONE_SLAB.getState(), false);

        const lce::BlockState torchSouth = lce::BlocksInit::TORCH.getStateFromMeta(
                states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));
        piece.setBlockState(worldIn, torchSouth, 6, 5, 6, chunkBB);
        return true;
    }


    bool LeftTurn::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 4, 4, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 1, 0);

        if (piece.facing != EnumFacing::NORTH && piece.facing != EnumFacing::EAST) {
            piece.fillWithAir(worldIn, chunkBB, 4, 1, 1, 4, 3, 3);
        } else {
            piece.fillWithAir(worldIn, chunkBB, 0, 1, 1, 0, 3, 3);
        }

        return true;
    }


    bool Library::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                    const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);
        c_bool isLargeRoom = piece.data & 1;

        int i = 11;

        if (!isLargeRoom) { i = 6; }

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 13, i - 1, 14, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 4, 1, 0);
        piece.fillWithBlocksRandomLightCheck(worldIn, chunkBB, rng, 0.07F, 2, 1, 1, 11, 4, 13,
                                             lce::BlocksInit::COBWEB.getState(), lce::BlocksInit::COBWEB.getState(), false, 0);
        // int j = 1;
        // int k = 12;

        constexpr lce::BlockState air = lce::BlocksInit::AIR.getState();
        constexpr lce::BlockState plank = lce::BlocksInit::OAK_WOOD_PLANK.getState();
        constexpr lce::BlockState bookshelf = lce::BlocksInit::BOOKSHELF.getState();
        constexpr lce::BlockState fence = lce::BlocksInit::OAK_FENCE.getState();
        const lce::Block& torch = lce::BlocksInit::TORCH;

        for (int l = 1; l <= 13; ++l) {
            if ((l - 1) % 4 == 0) {
                piece.fillWithBlocks(worldIn, chunkBB, 1, 1, l, 1, 4, l, plank, false);
                piece.fillWithBlocks(worldIn, chunkBB, 12, 1, l, 12, 4, l, plank, false);

                c_auto torchEast = torch.getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST)));
                piece.setBlockState(worldIn, torchEast, 2, 3, l, chunkBB);

                c_auto torchWest = torch.getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));
                piece.setBlockState(worldIn, torchWest, 11, 3, l, chunkBB);

                if (isLargeRoom) {
                    piece.fillWithBlocks(worldIn, chunkBB, 1, 6, l, 1, 9, l, plank, false);
                    piece.fillWithBlocks(worldIn, chunkBB, 12, 6, l, 12, 9, l, plank, false);
                }
            } else {
                piece.fillWithBlocks(worldIn, chunkBB, 1, 1, l, 1, 4, l, bookshelf, false);
                piece.fillWithBlocks(worldIn, chunkBB, 12, 1, l, 12, 4, l, bookshelf, false);

                if (isLargeRoom) {
                    piece.fillWithBlocks(worldIn, chunkBB, 1, 6, l, 1, 9, l, bookshelf, false);
                    piece.fillWithBlocks(worldIn, chunkBB, 12, 6, l, 12, 9, l, bookshelf, false);
                }
            }
        }

        for (int k1 = 3; k1 < 12; k1 += 2) {
            piece.fillWithBlocks(worldIn, chunkBB, 3, 1, k1, 4, 3, k1, bookshelf, false);
            piece.fillWithBlocks(worldIn, chunkBB, 6, 1, k1, 7, 3, k1, bookshelf, false);
            piece.fillWithBlocks(worldIn, chunkBB, 9, 1, k1, 10, 3, k1, bookshelf, false);
        }

        if (isLargeRoom) {
            piece.fillWithBlocks(worldIn, chunkBB, 1, 5, 1, 3, 5, 13, plank, false);
            piece.fillWithBlocks(worldIn, chunkBB, 10, 5, 1, 12, 5, 13, plank, false);
            piece.fillWithBlocks(worldIn, chunkBB, 4, 5, 1, 9, 5, 2, plank, false);
            piece.fillWithBlocks(worldIn, chunkBB, 4, 5, 12, 9, 5, 13, plank, false);
            piece.setBlockState(worldIn, plank, 9, 5, 11, chunkBB);
            piece.setBlockState(worldIn, plank, 8, 5, 11, chunkBB);
            piece.setBlockState(worldIn, plank, 9, 5, 10, chunkBB);
            piece.fillWithBlocks(worldIn, chunkBB, 3, 6, 2, 3, 6, 12, fence, false);
            piece.fillWithBlocks(worldIn, chunkBB, 10, 6, 2, 10, 6, 10, fence, false);
            piece.fillWithBlocks(worldIn, chunkBB, 4, 6, 2, 9, 6, 2, fence, false);
            piece.fillWithBlocks(worldIn, chunkBB, 4, 6, 12, 8, 6, 12, fence, false);
            piece.setBlockState(worldIn, fence, 9, 6, 11, chunkBB);
            piece.setBlockState(worldIn, fence, 8, 6, 11, chunkBB);
            piece.setBlockState(worldIn, fence, 9, 6, 10, chunkBB);

            // .withProperty(BlockLadder.FACING, FACING::SOUTH);

            c_auto ladderSouth = lce::BlocksInit::LADDER.getStateFromMeta(
                    states::Ladder::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));

            piece.setBlockState(worldIn, ladderSouth, 10, 1, 13, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 10, 2, 13, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 10, 3, 13, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 10, 4, 13, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 10, 5, 13, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 10, 6, 13, chunkBB);
            piece.setBlockState(worldIn, ladderSouth, 10, 7, 13, chunkBB);
            // int i1 = 7;
            // int j1 = 7;
            piece.setBlockState(worldIn, fence, 6, 9, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 7, 9, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 6, 8, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 7, 8, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 6, 7, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 7, 7, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 5, 7, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 8, 7, 7, chunkBB);
            piece.setBlockState(worldIn, fence, 6, 7, 6, chunkBB);
            piece.setBlockState(worldIn, fence, 6, 7, 8, chunkBB);
            piece.setBlockState(worldIn, fence, 7, 7, 6, chunkBB);
            piece.setBlockState(worldIn, fence, 7, 7, 8, chunkBB);

            c_auto torchUp = torch.getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::UP)));
            piece.setBlockState(worldIn, torchUp, 5, 8, 7, chunkBB);
            piece.setBlockState(worldIn, torchUp, 8, 8, 7, chunkBB);
            piece.setBlockState(worldIn, torchUp, 6, 8, 6, chunkBB);
            piece.setBlockState(worldIn, torchUp, 6, 8, 8, chunkBB);
            piece.setBlockState(worldIn, torchUp, 7, 8, 6, chunkBB);
            piece.setBlockState(worldIn, torchUp, 7, 8, 8, chunkBB);
        }

        // generateChest(worldIn, chunkBB, rng, 3, 3, 5, LootTableList.CHESTS_STRONGHOLD_LIBRARY);
        piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(), 3, 3, 5, chunkBB);
        rng.nextLong();

        if (isLargeRoom) {
            piece.setBlockState(worldIn, air, 12, 9, 1, chunkBB);
            // generateChest(worldIn, chunkBB, rng, 12, 8, 1, LootTableList.CHESTS_STRONGHOLD_LIBRARY);
            piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(), 12, 8, 1, chunkBB);
            rng.nextLong();
        }

        return true;
    }


    bool PortalRoom::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                       const StructureComponent& piece) {

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 10, 7, 15, false, rng);
        placeDoor(worldIn, chunkBB, piece, Door::GRATES, 4, 1, 0);
        int i = 6;
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 1, i, 1, 1, i, 14, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 9, i, 1, 9, i, 14, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 2, i, 1, 8, i, 2, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 2, i, 14, 8, i, 14, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 1, 1, 1, 2, 1, 4, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 8, 1, 1, 9, 1, 4, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 1, 1, 1, 3, lce::BlocksInit::FLOWING_LAVA.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 9, 1, 1, 9, 1, 3, lce::BlocksInit::FLOWING_LAVA.getState(), false);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 3, 1, 8, 7, 1, 12, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 9, 6, 1, 11, lce::BlocksInit::FLOWING_LAVA.getState(), false);

        for (int j = 3; j < 14; j += 2) {
            piece.fillWithBlocks(worldIn, chunkBB, 0, 3, j, 0, 4, j, lce::BlocksInit::IRON_BARS.getState(), false);
            piece.fillWithBlocks(worldIn, chunkBB, 10, 3, j, 10, 4, j, lce::BlocksInit::IRON_BARS.getState(), false);
        }

        for (int i1 = 2; i1 < 9; i1 += 2) {
            piece.fillWithBlocks(worldIn, chunkBB, i1, 3, 15, i1, 4, 15, lce::BlocksInit::IRON_BARS.getState(), false);
        }

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 5, 6, 1, 7, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 2, 6, 6, 2, 7, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 3, 7, 6, 3, 7, false, rng);

        const lce::BlockState cobbleStairNorth = lce::BlocksInit::STONE_BRICK_STAIRS
            .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));

        for (int k = 4; k <= 6; ++k) {
            piece.setBlockState(worldIn, cobbleStairNorth, k, 1, 4, chunkBB);
            piece.setBlockState(worldIn, cobbleStairNorth, k, 2, 5, chunkBB);
            piece.setBlockState(worldIn, cobbleStairNorth, k, 3, 6, chunkBB);
        }

        const lce::BlockState northFrame = lce::BlocksInit::END_PORTAL_FRAME.getStateFromMeta(
            states::EndPortalFrame::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH)));

        const lce::BlockState southFrame = lce::BlocksInit::END_PORTAL_FRAME.getStateFromMeta(
            states::EndPortalFrame::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));

        const lce::BlockState eastFrame = lce::BlocksInit::END_PORTAL_FRAME.getStateFromMeta(
            states::EndPortalFrame::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST)));

        const lce::BlockState westFrame = lce::BlocksInit::END_PORTAL_FRAME.getStateFromMeta(
            states::EndPortalFrame::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));

        bool flag = true;
        constexpr int NUM_FRAMES = 12;
        bool eyes[NUM_FRAMES];

        for (bool & eye : eyes) {
            eye = rng.nextFloat() > 0.9F;
            flag &= eye;
        }

        for (int x = 4; x <= 6; ++x) {
            piece.setBlockState(worldIn, northFrame.getStateFromMeta(
                states::EndPortalFrame::withProperty(northFrame.getDataTag(), eyes[x-4])), x, 3, 8, chunkBB);
            piece.setBlockState(worldIn, southFrame.getStateFromMeta(
                states::EndPortalFrame::withProperty(southFrame.getDataTag(), eyes[x-1])), x, 3, 12, chunkBB);
        }
        for (int z = 9; z <= 11; ++z) {
            piece.setBlockState(worldIn, eastFrame.getStateFromMeta(
                states::EndPortalFrame::withProperty(eastFrame.getDataTag(), eyes[z-3])), 3, 3, z, chunkBB);
            piece.setBlockState(worldIn, westFrame.getStateFromMeta(
                states::EndPortalFrame::withProperty(westFrame.getDataTag(), eyes[z])), 7, 3, z, chunkBB);
        }

        if (flag) {
            constexpr lce::BlockState endPortal = lce::BlocksInit::END_PORTAL.getState();
            for (int x = 4; x <= 6; ++x) {
                for (int z = 9; z <= 11; ++z) {
                    piece.setBlockState(worldIn, endPortal, x, 3, z, chunkBB);
                }
            }
        }


        bool hasSpawner = false;
        if (!hasSpawner) {
            i = piece.getWorldY(3);
            Pos3D blockPos(piece.getWorldX(5, 6), i, piece.getWorldZ(5, 6));

            if (chunkBB.isVecInside(blockPos)) {
                hasSpawner = true;
                worldIn.setBlock(blockPos, lce::BlocksInit::MONSTER_SPAWNER.getState()); // 2
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


    bool Prison::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                   const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 8, 4, 10, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 1, 0);
        piece.fillWithAir(worldIn, chunkBB, 1, 1, 10, 3, 3, 10);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 1, 4, 3, 1, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 3, 4, 3, 3, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 7, 4, 3, 7, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 9, 4, 3, 9, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 4, 4, 3, 6, lce::BlocksInit::IRON_BARS.getState(), false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 1, 5, 7, 3, 5, lce::BlocksInit::IRON_BARS.getState(), false);
        piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), 4, 3, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::IRON_BARS.getState(), 4, 3, 8, chunkBB);


        const lce::BlockState iblockstate = lce::BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));

        const lce::BlockState iblockstate1 = lce::BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                states::DoorUpper::withProperty());


        piece.setBlockState(worldIn, iblockstate, 4, 1, 2, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 4, 2, 2, chunkBB);
        piece.setBlockState(worldIn, iblockstate, 4, 1, 8, chunkBB);
        piece.setBlockState(worldIn, iblockstate1, 4, 2, 8, chunkBB);
        return true;
    }


    bool RightTurn::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                      const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 4, 4, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 1, 0);

        if (piece.facing != EnumFacing::NORTH && piece.facing != EnumFacing::EAST) {
            piece.fillWithAir(worldIn, chunkBB, 0, 1, 1, 0, 3, 3);
        } else {
            piece.fillWithAir(worldIn, chunkBB, 4, 1, 1, 4, 3, 3);
        }

        return true;
    }


    bool RoomCrossing::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                         const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        // TODO: these might need swapped
        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);
        c_int roomType = piece.data & 7;

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 10, 6, 10, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 4, 1, 0);
        piece.fillWithAir(worldIn, chunkBB, 4, 1, 10, 6, 3, 10);
        piece.fillWithAir(worldIn, chunkBB, 0, 1, 4, 0, 3, 6);
        piece.fillWithAir(worldIn, chunkBB, 10, 1, 4, 10, 3, 6);

        switch (roomType) {
            default:
                break;
            case 0:
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 5, 1, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 5, 2, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 5, 3, 5, chunkBB);

                piece.setBlockState(worldIn, lce::BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST))), 4, 3, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST))), 6, 3, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH))), 5, 3, 4, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::NORTH))), 5, 3, 6, chunkBB);


                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 4, 1, 4, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 4, 1, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 4, 1, 6, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 6, 1, 4, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 6, 1, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 6, 1, 6, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 5, 1, 4, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 5, 1, 6, chunkBB);
                break;

            case 1:
                for (int i1 = 0; i1 < 5; ++i1) {
                    piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 1, 3 + i1, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 7, 1, 3 + i1, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3 + i1, 1, 3, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3 + i1, 1, 7, chunkBB);
                }

                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 5, 1, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 5, 2, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 5, 3, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::FLOWING_WATER.getState(), 5, 4, 5, chunkBB);
                break;

            case 2:
                for (int i = 1; i <= 9; ++i) {
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 1, 3, i, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 9, 3, i, chunkBB);
                }

                for (int j = 1; j <= 9; ++j) {
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), j, 3, 1, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), j, 3, 9, chunkBB);
                }

                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 5, 1, 4, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 5, 1, 6, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 5, 3, 4, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 5, 3, 6, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 4, 1, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 6, 1, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 4, 3, 5, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 6, 3, 5, chunkBB);

                for (int k = 1; k <= 3; ++k) {
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 4, k, 4, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 6, k, 4, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 4, k, 6, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::COBBLESTONE.getState(), 6, k, 6, chunkBB);
                }

                piece.setBlockState(worldIn, lce::BlocksInit::TORCH.getState(), 5, 3, 5, chunkBB);

                for (int l = 2; l <= 8; ++l) {
                    piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 2, 3, l, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 3, 3, l, chunkBB);

                    if (l <= 3 || l >= 7) {
                        piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 4, 3, l, chunkBB);
                        piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 5, 3, l, chunkBB);
                        piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 6, 3, l, chunkBB);
                    }

                    piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 7, 3, l, chunkBB);
                    piece.setBlockState(worldIn, lce::BlocksInit::OAK_WOOD_PLANK.getState(), 8, 3, l, chunkBB);
                }

                const lce::BlockState ladderWest = lce::BlocksInit::LADDER.getStateFromMeta(
                        states::Ladder::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));
                piece.setBlockState(worldIn, ladderWest, 9, 1, 3, chunkBB);
                piece.setBlockState(worldIn, ladderWest, 9, 2, 3, chunkBB);
                piece.setBlockState(worldIn, ladderWest, 9, 3, 3, chunkBB);

                // LootTableList.CHESTS_STRONGHOLD_CROSSING
                // generateChest(worldIn, chunkBB, rng, 3, 4, 8);
                piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(), 3, 4, 8, chunkBB);
                rng.nextLong();
        }

        return true;
    }


    bool Stairs::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                   const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 10, 4, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 7, 0);
        placeDoor(worldIn, chunkBB, piece, Door::OPENING, 1, 1, 4);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 2, 6, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 5, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 1, 6, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 5, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 4, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 1, 5, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 2, 4, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 3, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 3, 4, 3, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 3, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 2, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 3, 3, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 2, 2, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 1, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 1, 2, 1, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 1, 2, chunkBB);
        piece.setBlockState(worldIn, lce::BlocksInit::STONE_SLAB.getState(), 1, 1, 3, chunkBB);
        return true;
    }


    bool StairsStraight::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                           const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 10, 7, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 7, 0);
        placeDoor(worldIn, chunkBB, piece, Door::OPENING, 1, 1, 7);

        const lce::BlockState cobblestoneStairs = lce::BlocksInit::COBBLESTONE_STAIRS.getStateFromMeta(
                states::Stairs::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::SOUTH)));

        for (int i = 0; i < 6; ++i) {
            piece.setBlockState(worldIn, cobblestoneStairs, 1, 6 - i, 1 + i, chunkBB);
            piece.setBlockState(worldIn, cobblestoneStairs, 2, 6 - i, 1 + i, chunkBB);
            piece.setBlockState(worldIn, cobblestoneStairs, 3, 6 - i, 1 + i, chunkBB);

            if (i < 5) {
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 1, 5 - i, 1 + i, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 2, 5 - i, 1 + i, chunkBB);
                piece.setBlockState(worldIn, lce::BlocksInit::STONE_BRICKS.getState(), 3, 5 - i, 1 + i, chunkBB);
            }
        }

        return true;
    }


    bool Straight::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 4, 6, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 1, 0);
        placeDoor(worldIn, chunkBB, piece, Door::OPENING, 1, 1, 6);

        const lce::BlockState iblockstate = lce::BlocksInit::TORCH.getStateFromMeta(
                states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::EAST)));
        const lce::BlockState iblockstate1 = lce::BlocksInit::TORCH.getStateFromMeta(
                states::Torch::withProperty(piece.rotation.apply(piece.mirror, EnumFacing::WEST)));

        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 1, 2, 1, iblockstate);
        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 3, 2, 1, iblockstate1);
        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 1, 2, 5, iblockstate);
        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 3, 2, 5, iblockstate1);

        c_bool expandsX = piece.data & 1;
        if (expandsX) {
            piece.fillWithAir(worldIn, chunkBB, 0, 1, 2, 0, 3, 4);
        }

        c_bool expandsZ = piece.data & 2;
        if (expandsZ) {
            piece.fillWithAir(worldIn, chunkBB, 4, 1, 2, 4, 3, 4);
        }

        return true;
    }


    bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece) {
        bool result = false;

        switch (piece.type) {
            case PT_Stronghold_Straight:
                result = Straight::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_PrisonHall:
                result = Prison::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_LeftTurn:
                result = LeftTurn::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_RightTurn:
                result = RightTurn::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_RoomCrossing:
                result = RoomCrossing::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_StraightStairsDown:
                result = StairsStraight::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_StairsDown:
                result = Stairs::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_FiveCrossing:
                result = Crossing::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_ChestCorridor:
                result = ChestCorridor::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_Library:
                result = Library::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_PortalRoom:
                result = PortalRoom::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Stronghold_FillerCorridor:
                result = Corridor::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            default:;
        }
        return result;
    }
}



