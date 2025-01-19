#include <cmath>

#include "LegacyCubiomes/utils/rng.hpp"
#include "stronghold.hpp"

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/include.hpp"
#include "lce/blocks/block_states.hpp"

using namespace lce::blocks;
using namespace lce::blocks::states;


namespace build::stronghold {

    using namespace lce::blocks;
    using namespace states;


    void placeDoor(World& worldIn, const BoundingBox& chunkBB, const StructureComponent& piece,
                   const Door door, const int x, const int y, const int z) {
        switch (door) {
            case Door::OPENING:
                piece.fillWithAir(worldIn, chunkBB, x, y, z, x + 3 - 1, y + 3 - 1, z);
                break;

            case Door::WOOD_DOOR: {
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x, y, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 1, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 2, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 2, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 2, y, z, chunkBB);

                const auto door_base = &BlocksInit::OAK_DOOR_BLOCK;

                c_auto door_bottom = door_base->getStateFromMeta(states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
                piece.setBlockState(worldIn, door_bottom, x + 1, y, z, chunkBB);

                c_auto door_top = door_base->getStateFromMeta(states::DoorUpper::withProperty());
                piece.setBlockState(worldIn, door_top, x + 1, y + 1, z, chunkBB);
                break;
            }

            case Door::GRATES:
                piece.setBlockState(worldIn, &BlocksInit::AIR, x + 1, y, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::AIR, x + 1, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x, y, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x + 1, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x + 2, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x + 2, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, x + 2, y, z, chunkBB);
                break;

            case Door::IRON_DOOR: {
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x, y, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 1, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 2, y + 2, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 2, y + 1, z, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, x + 2, y, z, chunkBB);

                c_auto doorBlockLower = BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                        states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
                piece.setBlockState(worldIn, doorBlockLower, x + 1, y, z, chunkBB);

                c_auto doorBlockUpper = BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                        states::DoorUpper::withProperty());
                piece.setBlockState(worldIn, doorBlockUpper, x + 1, y + 1, z, chunkBB);

                c_auto buttonBlock1 = BlocksInit::STONE_BUTTON.getStateFromMeta(
                        states::Button::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));
                piece.setBlockState(worldIn, &buttonBlock1, x + 2, y + 1, z + 1, chunkBB);

                c_auto buttonBlock2 = BlocksInit::STONE_BUTTON.getStateFromMeta(
                        states::Button::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));
                piece.setBlockState(worldIn, &buttonBlock2, x + 2, y + 1, z - 1, chunkBB);
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
        piece.fillWithBlocks(worldIn, chunkBB, 3, 1, 2, 3, 1, 4, &BlocksInit::STONE_BRICKS, false);

        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICK_SLAB, 3, 1, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICK_SLAB, 3, 1, 5, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICK_SLAB, 3, 2, 2, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICK_SLAB, 3, 2, 4, chunkBB);

        for (int i = 2; i <= 4; ++i) {
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICK_SLAB, 2, 1, i, chunkBB);
        }

        Pos3D chestPos = piece.getWorldXYZ(3, 2, 3);
        if (!hasMadeChest && chunkBB.isVecInside(chestPos)) {
            hasMadeChest = true;
            // generateChest(worldIn, chunkBB, rng, 3, 2, 3, LootTableList.CHESTS_STRONGHOLD_CORRIDOR);
            piece.setBlockState(worldIn, &BlocksInit::CHEST,  3, 2, 3, chunkBB);
            rng.nextLong();
        }

        return true;
    }


    bool Corridor::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        c_int steps = piece.facing != enumFacing::NORTH && piece.facing != enumFacing::SOUTH ? piece.getXSize() : piece.getZSize();

        for (int i = 0; i < steps; ++i) {
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 0, 0, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 0, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 2, 0, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 0, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 4, 0, i, chunkBB);

            for (int j = 1; j <= 3; ++j) {
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 0, j, i, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::AIR, 1, j, i, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::AIR, 2, j, i, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::AIR, 3, j, i, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 4, j, i, chunkBB);
            }

            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 0, 4, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 4, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 2, 4, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 4, i, chunkBB);
            piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 4, 4, i, chunkBB);

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
        piece.fillWithBlocks(worldIn, chunkBB, 1, 3, 4, 3, 3, 4, &BlocksInit::STONE_SLAB, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1, 4, 6, 3, 4, 6, &BlocksInit::STONE_SLAB, false);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 5, 1, 7, 7, 1, 8, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 1, 9, 7, 1, 9, &BlocksInit::STONE_SLAB, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 2, 7, 7, 2, 7, &BlocksInit::STONE_SLAB, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 5, 7, 4, 5, 9, &BlocksInit::STONE_SLAB, false);
        piece.fillWithBlocks(worldIn, chunkBB, 8, 5, 7, 8, 5, 9, &BlocksInit::STONE_SLAB, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 5, 7, 7, 5, 9, &BlocksInit::DOUBLE_STONE_SLAB, false);

        const Block torchSouth = BlocksInit::TORCH.getStateFromMeta(
                states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));
        piece.setBlockState(worldIn, torchSouth, 6, 5, 6, chunkBB);
        return true;
    }


    bool LeftTurn::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 4, 4, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 1, 0);

        if (piece.facing != enumFacing::NORTH && piece.facing != enumFacing::EAST) {
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
                                             &BlocksInit::COBWEB, &BlocksInit::COBWEB, false, 0);
        // int j = 1;
        // int k = 12;

        const Block* air = &BlocksInit::AIR;
        const Block* plank = &BlocksInit::OAK_WOOD_PLANK;
        const Block* bookshelf = &BlocksInit::BOOKSHELF;
        const Block* fence = &BlocksInit::OAK_FENCE;
        const Block* torch = &BlocksInit::TORCH;

        for (int l = 1; l <= 13; ++l) {
            if ((l - 1) % 4 == 0) {
                piece.fillWithBlocks(worldIn, chunkBB, 1, 1, l, 1, 4, l, plank, false);
                piece.fillWithBlocks(worldIn, chunkBB, 12, 1, l, 12, 4, l, plank, false);

                auto torchEast = torch->getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::EAST)));
                piece.setBlockState(worldIn, torchEast, 2, 3, l, chunkBB);

                auto torchWest = torch->getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));
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

            c_auto ladderSouth = BlocksInit::LADDER.getStateFromMeta(
                    states::Ladder::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));

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

            c_auto torchUp = torch->getStateFromMeta(states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::UP)));
            piece.setBlockState(worldIn, torchUp, 5, 8, 7, chunkBB);
            piece.setBlockState(worldIn, torchUp, 8, 8, 7, chunkBB);
            piece.setBlockState(worldIn, torchUp, 6, 8, 6, chunkBB);
            piece.setBlockState(worldIn, torchUp, 6, 8, 8, chunkBB);
            piece.setBlockState(worldIn, torchUp, 7, 8, 6, chunkBB);
            piece.setBlockState(worldIn, torchUp, 7, 8, 8, chunkBB);
        }

        // generateChest(worldIn, chunkBB, rng, 3, 3, 5, LootTableList.CHESTS_STRONGHOLD_LIBRARY);
        piece.setBlockState(worldIn, BlocksInit::CHEST, 3, 3, 5, chunkBB);
        rng.nextLong();

        if (isLargeRoom) {
            piece.setBlockState(worldIn, air, 12, 9, 1, chunkBB);
            // generateChest(worldIn, chunkBB, rng, 12, 8, 1, LootTableList.CHESTS_STRONGHOLD_LIBRARY);
            piece.setBlockState(worldIn, BlocksInit::CHEST, 12, 8, 1, chunkBB);
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
        piece.fillWithBlocks(worldIn, chunkBB, 1, 1, 1, 1, 1, 3, &BlocksInit::FLOWING_LAVA, false);
        piece.fillWithBlocks(worldIn, chunkBB, 9, 1, 1, 9, 1, 3, &BlocksInit::FLOWING_LAVA, false);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 3, 1, 8, 7, 1, 12, false, rng);
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 9, 6, 1, 11, &BlocksInit::FLOWING_LAVA, false);

        for (int j = 3; j < 14; j += 2) {
            piece.fillWithBlocks(worldIn, chunkBB, 0, 3, j, 0, 4, j, &BlocksInit::IRON_BARS, false);
            piece.fillWithBlocks(worldIn, chunkBB, 10, 3, j, 10, 4, j, &BlocksInit::IRON_BARS, false);
        }

        for (int i1 = 2; i1 < 9; i1 += 2) {
            piece.fillWithBlocks(worldIn, chunkBB, i1, 3, 15, i1, 4, 15, &BlocksInit::IRON_BARS, false);
        }

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 1, 5, 6, 1, 7, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 2, 6, 6, 2, 7, false, rng);
        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 4, 3, 7, 6, 3, 7, false, rng);

        const Block cobbleStairNorth = BlocksInit::STONE_BRICK_STAIRS
                                               .getStateFromMeta(states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH)));

        for (int k = 4; k <= 6; ++k) {
            piece.setBlockState(worldIn, cobbleStairNorth, k, 1, 4, chunkBB);
            piece.setBlockState(worldIn, cobbleStairNorth, k, 2, 5, chunkBB);
            piece.setBlockState(worldIn, cobbleStairNorth, k, 3, 6, chunkBB);
        }

        // .withProperty(BlockEndPortalFrame.FACING, FACING::NORTH);
        const Block* iblockstate4 = &BlocksInit::END_PORTAL_FRAME;
        // .withProperty(BlockEndPortalFrame.FACING, FACING::SOUTH);
        const Block* iblockstate = &BlocksInit::END_PORTAL_FRAME;
        // .withProperty(BlockEndPortalFrame.FACING, FACING::EAST);
        const Block* iblockstate1 = &BlocksInit::END_PORTAL_FRAME;
        // .withProperty(BlockEndPortalFrame.FACING, FACING::WEST);
        const Block* iblockstate2 = &BlocksInit::END_PORTAL_FRAME;

        /*
        boolean flag = true;
        boolean[] aboolean = new boolean[12];

        for (int l = 0; l < aboolean.length; ++l) {
            aboolean[l] = rng.nextFloat() > 0.9F;
            flag &= aboolean[l];
        }
         */

        piece.setBlockState(worldIn, iblockstate4->getStateFromMeta(0), 4, 3, 8, chunkBB);
        piece.setBlockState(worldIn, iblockstate4->getStateFromMeta(0), 5, 3, 8, chunkBB);
        piece.setBlockState(worldIn, iblockstate4->getStateFromMeta(0), 6, 3, 8, chunkBB);
        piece.setBlockState(worldIn, iblockstate->getStateFromMeta(0), 4, 3, 12, chunkBB);
        piece.setBlockState(worldIn, iblockstate->getStateFromMeta(0), 5, 3, 12, chunkBB);
        piece.setBlockState(worldIn, iblockstate->getStateFromMeta(0), 6, 3, 12, chunkBB);
        piece.setBlockState(worldIn, iblockstate1->getStateFromMeta(0), 3, 3, 9, chunkBB);
        piece.setBlockState(worldIn, iblockstate1->getStateFromMeta(0), 3, 3, 10, chunkBB);
        piece.setBlockState(worldIn, iblockstate1->getStateFromMeta(0), 3, 3, 11, chunkBB);
        piece.setBlockState(worldIn, iblockstate2->getStateFromMeta(0), 7, 3, 9, chunkBB);
        piece.setBlockState(worldIn, iblockstate2->getStateFromMeta(0), 7, 3, 10, chunkBB);
        piece.setBlockState(worldIn, iblockstate2->getStateFromMeta(0), 7, 3, 11, chunkBB);

        /*
        if (flag) {
            const Block* iblockstate5 = &BlocksInit::END_PORTAL;
            piece.setBlockState(worldIn, iblockstate5, 4, 3, 9, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 5, 3, 9, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 6, 3, 9, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 4, 3, 10, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 5, 3, 10, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 6, 3, 10, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 4, 3, 11, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 5, 3, 11, chunkBB);
            piece.setBlockState(worldIn, iblockstate5, 6, 3, 11, chunkBB);
        }
         */


        bool hasSpawner = false;
        if (!hasSpawner) {
            i = piece.getWorldY(3);
            Pos3D blockPos(piece.getWorldX(5, 6), i, piece.getWorldZ(5, 6));

            if (chunkBB.isVecInside(blockPos)) {
                hasSpawner = true;
                worldIn.setBlock(blockPos, &BlocksInit::MONSTER_SPAWNER); // 2
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
        piece.fillWithBlocks(worldIn, chunkBB, 4, 1, 4, 4, 3, 6, &BlocksInit::IRON_BARS, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5, 1, 5, 7, 3, 5, &BlocksInit::IRON_BARS, false);
        piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, 4, 3, 2, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::IRON_BARS, 4, 3, 8, chunkBB);


        const Block iblockstate = BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
                states::DoorLower::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));

        const Block iblockstate1 = BlocksInit::IRON_DOOR_BLOCK.getStateFromMeta(
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

        if (piece.facing != enumFacing::NORTH && piece.facing != enumFacing::EAST) {
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
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 5, 1, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 5, 2, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 5, 3, 5, chunkBB);

                piece.setBlockState(worldIn, BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST))), 4, 3, 5, chunkBB);
                piece.setBlockState(worldIn, BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::EAST))), 6, 3, 5, chunkBB);
                piece.setBlockState(worldIn, BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH))), 5, 3, 4, chunkBB);
                piece.setBlockState(worldIn, BlocksInit::TORCH.getStateFromMeta(
                                                     states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::NORTH))), 5, 3, 6, chunkBB);


                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 4, 1, 4, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 4, 1, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 4, 1, 6, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 6, 1, 4, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 6, 1, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 6, 1, 6, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 5, 1, 4, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 5, 1, 6, chunkBB);
                break;

            case 1:
                for (int i1 = 0; i1 < 5; ++i1) {
                    piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 1, 3 + i1, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 7, 1, 3 + i1, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3 + i1, 1, 3, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3 + i1, 1, 7, chunkBB);
                }

                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 5, 1, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 5, 2, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 5, 3, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::FLOWING_WATER, 5, 4, 5, chunkBB);
                break;

            case 2:
                for (int i = 1; i <= 9; ++i) {
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 1, 3, i, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 9, 3, i, chunkBB);
                }

                for (int j = 1; j <= 9; ++j) {
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, j, 3, 1, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, j, 3, 9, chunkBB);
                }

                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 5, 1, 4, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 5, 1, 6, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 5, 3, 4, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 5, 3, 6, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 4, 1, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 6, 1, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 4, 3, 5, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 6, 3, 5, chunkBB);

                for (int k = 1; k <= 3; ++k) {
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 4, k, 4, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 6, k, 4, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 4, k, 6, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::COBBLESTONE, 6, k, 6, chunkBB);
                }

                piece.setBlockState(worldIn, &BlocksInit::TORCH, 5, 3, 5, chunkBB);

                for (int l = 2; l <= 8; ++l) {
                    piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 2, 3, l, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 3, 3, l, chunkBB);

                    if (l <= 3 || l >= 7) {
                        piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 4, 3, l, chunkBB);
                        piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 5, 3, l, chunkBB);
                        piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 6, 3, l, chunkBB);
                    }

                    piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 7, 3, l, chunkBB);
                    piece.setBlockState(worldIn, &BlocksInit::OAK_WOOD_PLANK, 8, 3, l, chunkBB);
                }

                const Block ladderWest = BlocksInit::LADDER.getStateFromMeta(
                        states::Ladder::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));
                piece.setBlockState(worldIn, ladderWest, 9, 1, 3, chunkBB);
                piece.setBlockState(worldIn, ladderWest, 9, 2, 3, chunkBB);
                piece.setBlockState(worldIn, ladderWest, 9, 3, 3, chunkBB);

                // LootTableList.CHESTS_STRONGHOLD_CROSSING
                // generateChest(worldIn, chunkBB, rng, 3, 4, 8);
                piece.setBlockState(worldIn, BlocksInit::CHEST, 3, 4, 8, chunkBB);
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
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 2, 6, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 5, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 1, 6, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 5, 2, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 4, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 1, 5, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 2, 4, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 3, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 3, 4, 3, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 3, 2, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 2, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 3, 3, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 2, 2, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 1, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 1, 2, 1, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 1, 2, chunkBB);
        piece.setBlockState(worldIn, &BlocksInit::STONE_SLAB, 1, 1, 3, chunkBB);
        return true;
    }


    bool StairsStraight::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                           const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const Door entryDoor = getRandomDoor(piece.data >> 16 & 7);

        piece.fillWithRandomizedStrongholdStones(worldIn, chunkBB, 0, 0, 0, 4, 10, 7, true, rng);
        placeDoor(worldIn, chunkBB, piece, entryDoor, 1, 7, 0);
        placeDoor(worldIn, chunkBB, piece, Door::OPENING, 1, 1, 7);

        const Block cobblestoneStairs = BlocksInit::COBBLESTONE_STAIRS.getStateFromMeta(
                states::Stairs::withProperty(piece.rotation.apply(piece.mirror, enumFacing::SOUTH)));

        for (int i = 0; i < 6; ++i) {
            piece.setBlockState(worldIn, cobblestoneStairs, 1, 6 - i, 1 + i, chunkBB);
            piece.setBlockState(worldIn, cobblestoneStairs, 2, 6 - i, 1 + i, chunkBB);
            piece.setBlockState(worldIn, cobblestoneStairs, 3, 6 - i, 1 + i, chunkBB);

            if (i < 5) {
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 1, 5 - i, 1 + i, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 2, 5 - i, 1 + i, chunkBB);
                piece.setBlockState(worldIn, &BlocksInit::STONE_BRICKS, 3, 5 - i, 1 + i, chunkBB);
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

        const Block iblockstate = BlocksInit::TORCH.getStateFromMeta(
                states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::EAST)));
        const Block iblockstate1 = BlocksInit::TORCH.getStateFromMeta(
                states::Torch::withProperty(piece.rotation.apply(piece.mirror, enumFacing::WEST)));

        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 1, 2, 1, &iblockstate);
        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 3, 2, 1, &iblockstate1);
        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 1, 2, 5, &iblockstate);
        piece.randomlyPlaceBlock(worldIn, chunkBB, rng, 0.1F, 3, 2, 5, &iblockstate1);

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



