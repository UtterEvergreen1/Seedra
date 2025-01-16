#pragma once

#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/block_states.hpp"
#include "lce/blocks/blocks.hpp"


namespace build::mineshaft {


    MU ND static const lce::blocks::Block* getBiomeSpecificPlank(c_int structureType) {
        switch (structureType) {
            case 0: // normal
            default:
                return &lce::blocks::BlocksInit::OAK_WOOD_PLANK;
            case 1: // mesa
                return &lce::blocks::BlocksInit::DARK_OAK_WOOD_PLANK;
        }
    }


    MU ND static const lce::blocks::Block* getBiomeSpecificFence(c_int structureType) {
        switch (structureType) {
            case 0: // normal
            default:
                return &lce::blocks::BlocksInit::OAK_FENCE;
            case 1: // mesa
                return &lce::blocks::BlocksInit::DARK_OAK_FENCE;
        }
    }


    static bool func_189918_a(World& world, const BoundingBox& structureBB, c_int minX, c_int maxX,
                              c_int y, c_int z, const StructureComponent& piece) {
        for (int x = minX; x <= maxX; ++x) {
            if (lce::blocks::ids::isReplaceableBlock(
                        piece.getBlockStateFromPos(world, x, y + 1, z, structureBB)->getID())) {
                return false;
            }
        }

        return true;
    }



    class MU Room final {
    public:
        MU Room() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& structureBB, const StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) { return false; }

            const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

            piece.fillWithBlocks(worldIn, structureBB, 0, 0, 0, piece.maxX - piece.minX, 0, piece.maxZ - piece.minZ,
                                 &lce::blocks::BlocksInit::DIRT, air, false);
            piece.fillWithAir(worldIn, structureBB, 0, 1, 0, piece.maxX - piece.minX,
                              std::min(3, static_cast<int>(piece.maxY - piece.minY)), piece.maxZ - piece.minZ);

            // for (const BoundingBox& sbb: roomsLinkedToTheRoom) {
            //     piece.fillWithBlocks(worldIn, structureBB, sbb.piece.minX, sbb.piece.maxY - 2,
            //                    sbb.piece.minZ, sbb.piece.maxX, sbb.piece.maxY,
            //                    sbb.piece.maxZ, air, air, false);
            // }

            piece.randomlyRareFillWithBlocks(worldIn, structureBB, 0, 4, 0, piece.maxX - piece.minX,
                                             piece.maxY - piece.minY, piece.maxZ - piece.minZ, air, false);
            return true;
        }
    };


    class MU Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, const StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) { return false; }

            c_bool hasRails = piece.data & 1;
            c_bool hasSpiders = piece.data >> 1 & 1;
            bool spawnerPlaced = false;
            int sectionCount;
            if (getAxis(piece.facing) == EnumAxis::Z) {
                sectionCount = piece.getZSize() / 5;
            } else {
                sectionCount = piece.getXSize() / 5;
            }

            c_int i1 = sectionCount * 5 - 1;

            const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

            const lce::blocks::Block* iblockstate = getBiomeSpecificPlank(piece.structureType);
            piece.fillWithAir(worldIn, structureBB, 0, 0, 0, 2, 1, i1);
            piece.fillWithBlocksRandomLightCheck(worldIn, structureBB, rng, 0.8F, 0, 2, 0, 2, 2, i1, air, air, false, 0);

            if (hasSpiders) {
                piece.fillWithBlocksRandomLightCheck(worldIn, structureBB, rng, 0.6F, 0, 0, 0, 2, 1, i1, air, air, false, 8);
            }

            for (int j1 = 0; j1 < sectionCount; ++j1) {
                c_int k1 = 2 + j1 * 5;
                func_189921_a(worldIn, structureBB, 0, 0, k1, 2, 2, rng, piece);
                func_189922_a(worldIn, structureBB, rng, 0.1F, 0, 2, k1 - 1, piece);
                func_189922_a(worldIn, structureBB, rng, 0.1F, 2, 2, k1 - 1, piece);
                func_189922_a(worldIn, structureBB, rng, 0.1F, 0, 2, k1 + 1, piece);
                func_189922_a(worldIn, structureBB, rng, 0.1F, 2, 2, k1 + 1, piece);
                func_189922_a(worldIn, structureBB, rng, 0.05F, 0, 2, k1 - 2, piece);
                func_189922_a(worldIn, structureBB, rng, 0.05F, 2, 2, k1 - 2, piece);
                func_189922_a(worldIn, structureBB, rng, 0.05F, 0, 2, k1 + 2, piece);
                func_189922_a(worldIn, structureBB, rng, 0.05F, 2, 2, k1 + 2, piece);

                if (rng.nextInt(100) == 0) {
                    // generateChest(worldIn, structureBB, rng, 2, 0, k1 - 1, LootTableList.CHESTS_ABANDONED_MINESHAFT);
                    piece.setBlockState(worldIn, lce::blocks::BlocksInit::CHEST, 2, 0, k1 - 1, structureBB);
                    rng.nextLong();
                }


                if (rng.nextInt(100) == 0) {
                    // generateChest(worldIn, structureBB, rng, 0, 0, k1 + 1, LootTableList.CHESTS_ABANDONED_MINESHAFT);
                    piece.setBlockState(worldIn, lce::blocks::BlocksInit::CHEST, 0, 0, k1 + 1, structureBB);
                    rng.nextLong();
                }

                if (hasSpiders && !spawnerPlaced) {
                    c_int i2 = k1 - 1 + rng.nextInt(3);
                    const Pos3D blockPos = piece.getWorldXYZ(1, 0, i2); // (j2, l1, k2);

                    if (structureBB.isVecInside(blockPos) && piece.getLightLevelAtBlock(worldIn, 1, 0, i2, structureBB) < 8) {
                        spawnerPlaced = true;
                        // caveSpider
                        piece.setBlockState(worldIn, &lce::blocks::BlocksInit::MONSTER_SPAWNER, 1, 0, 1, structureBB);
                    }
                }
            }

            for (int l2 = 0; l2 <= 2; ++l2) {
                for (int i3 = 0; i3 <= i1; ++i3) {
                    MU int k3 = -1;
                    const lce::blocks::Block* iBlockState3 =
                            piece.getBlockStateFromPos(worldIn, l2, -1, i3, structureBB);

                    if (lce::blocks::ids::isReplaceableBlock(iBlockState3->getID()) &&
                        piece.getLightLevelAtBlock(worldIn, l2, -1, i3, structureBB) < 8) {
                        MU int l3 = -1;
                        piece.setBlockState(worldIn, iblockstate, l2, -1, i3, structureBB);
                    }
                }
            }

            if (hasRails) {
                // .withProperty(BlockRail.SHAPE, BlockRailBase.EnumRailDirection.NORTH_SOUTH);
                const lce::blocks::Block iblockstate1 = lce::blocks::BlocksInit::RAIL.getStateFromMeta(
                    lce::blocks::states::Rail::withProperty(lce::blocks::states::RailShape::NS));

                for (int j3 = 0; j3 <= i1; ++j3) {
                    c_auto* iBlockState2 = piece.getBlockStateFromPos(worldIn, 1, -1, j3, structureBB);
                    if (lce::blocks::ids::isReplaceableBlock(iBlockState2->getID()) &&
                        lce::blocks::ids::isFullBlock(iblockstate1.getID())) {
                        c_float f = piece.getLightLevelAtBlock(worldIn, 1, 0, j3, structureBB) > 8 ? 0.9F : 0.7F;
                        piece.randomlyPlaceBlock(worldIn, structureBB, rng, f, 1, 0, j3, &iblockstate1);
                    }
                }
            }

            return true;
        }


        /// variable names may be incorrect
        static void func_189921_a(World& world, const BoundingBox& structureBB,
            c_int minX, c_int minY, c_int minZ, c_int maxY, c_int maxX,
            RNG& rng, const StructureComponent& piece) {

            if (func_189918_a(world, structureBB, minX, maxX, maxY, minZ, piece)) {
                const lce::blocks::Block* plank = getBiomeSpecificPlank(piece.structureType);
                const lce::blocks::Block* fence = getBiomeSpecificFence(piece.structureType);
                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                piece.fillWithBlocks(world, structureBB, minX, minY, minZ, minX, maxY - 1, minZ, fence, air, false);
                piece.fillWithBlocks(world, structureBB, maxX, minY, minZ, maxX, maxY - 1, minZ, fence, air, false);

                if (rng.nextInt(4) == 0) {
                    piece.fillWithBlocks(world, structureBB, minX, maxY, minZ, minX, maxY, minZ, plank, air, false);
                    piece.fillWithBlocks(world, structureBB, maxX, maxY, minZ, maxX, maxY, minZ, plank, air, false);
                } else {
                    piece.fillWithBlocks(world, structureBB, minX, maxY, minZ, maxX, maxY, minZ, plank, air, false);
                    c_auto torchNorth = lce::blocks::BlocksInit::TORCH.getStateFromMeta(
                        lce::blocks::states::Torch::withProperty(enumFacing::NORTH));
                    piece.randomlyPlaceBlock(world, structureBB, rng, 0.05F, minX + 1, maxY, minZ - 1, &torchNorth);
                    c_auto torchSouth = lce::blocks::BlocksInit::TORCH.getStateFromMeta(
                        lce::blocks::states::Torch::withProperty(enumFacing::SOUTH));
                    piece.randomlyPlaceBlock(world, structureBB, rng, 0.05F, minX + 1, maxY, minZ + 1, &torchSouth);
                }
            }
        }

        static void func_189922_a(World& world, const BoundingBox& structureBB, RNG& rng,
            c_float chance, c_int x, c_int y, c_int z, const StructureComponent& piece) {
            if (piece.getLightLevelAtBlock(world, x, y, z, structureBB) < 8) {
                piece.randomlyPlaceBlock(world, structureBB, rng, chance, x, y, z, &lce::blocks::BlocksInit::COBWEB);
            }
        }
    };


    class MU Crossing final {
    public:
        MU Crossing() = delete;
        
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& structureBB, const StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) { return false; }

            const bool isMultipleFloors = piece.data & 1;

            const lce::blocks::Block* iblockstate = getBiomeSpecificPlank(piece.structureType);

            if (isMultipleFloors) {
                piece.fillWithAir(worldIn, structureBB, 1, 0, 0, piece.maxX - piece.minX - 1, 3 - 1,
                                  piece.maxZ - piece.minZ);
                piece.fillWithAir(worldIn, structureBB, 0, 0, 1, piece.maxX - piece.minX, 3 - 1,
                                  piece.maxZ - piece.minZ - 1);
                piece.fillWithAir(worldIn, structureBB, 1, -2, 0, piece.maxX - piece.minX - 1, piece.maxY - piece.minY,
                                  piece.maxZ - piece.minZ);
                piece.fillWithAir(worldIn, structureBB, 0, -2, 1, piece.maxX - piece.minX, piece.maxY - piece.minY,
                                  piece.maxZ - piece.minZ - 1);
                piece.fillWithAir(worldIn, structureBB, 1, 3, 1, piece.maxX - piece.minX - 1, 3,
                                  piece.maxZ - piece.minZ - 1);
            } else {
                piece.fillWithAir(worldIn, structureBB, 1, 0, 0, piece.maxX - piece.minX - 1, piece.maxY - piece.minY,
                                  piece.maxZ - piece.minZ);
                piece.fillWithAir(worldIn, structureBB, 0, 0, 1, piece.maxX - piece.minX, piece.maxY - piece.minY,
                                  piece.maxZ - piece.minZ - 1);
            }

            placePlankPillar(worldIn, structureBB, 1, 0, 1, piece.maxY - piece.minY, piece);
            placePlankPillar(worldIn, structureBB, 1, 0, piece.maxZ - piece.minZ - 1, piece.maxY - piece.minY, piece);
            placePlankPillar(worldIn, structureBB, piece.maxX - piece.minX - 1, 0, 1, piece.maxY - piece.minY, piece);
            placePlankPillar(worldIn, structureBB, piece.maxX - piece.minX - 1, 0, piece.maxZ - piece.minZ - 1,
                             piece.maxY - piece.minY, piece);

            for (int i = 0; i <= piece.maxX - piece.minX; ++i) {
                for (int j = 0; j <= piece.maxZ - piece.minZ; ++j) {
                    if (lce::blocks::ids::isReplaceableBlock(
                                piece.getBlockStateFromPos(worldIn, i, -1, j, structureBB)->getID()) &&
                        piece.getLightLevelAtBlock(worldIn, i, -1, j, structureBB) < 8) {
                        piece.setBlockState(worldIn, iblockstate, i, -1, j, structureBB);
                    }
                }
            }

            return true;
        }


        static void placePlankPillar(World& world, const BoundingBox& structureBB,
            c_int x, c_int minY, c_int z, c_int maxY, const StructureComponent& piece) {

            c_auto block = piece.getBlockStateFromPos(world, x, maxY + 1, z, structureBB);

            if (!lce::blocks::ids::isReplaceableBlock(block->getID())) {
                c_auto* plank = getBiomeSpecificPlank(piece.structureType);
                piece.fillWithBlocks(world, structureBB, x, minY, z, x, maxY, z, plank, &lce::blocks::BlocksInit::AIR, false);
            }
        }
    };


    class MU Stairs final {
    public:
        MU Stairs() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& structureBB, const StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) { return false; }

            piece.fillWithAir(worldIn, structureBB, 0, 5, 0, 2, 7, 1);
            piece.fillWithAir(worldIn, structureBB, 0, 0, 7, 2, 2, 8);
            for (int i = 0; i < 5; ++i) {
                piece.fillWithAir(worldIn, structureBB, 0, 5 - i - (i < 4 ? 1 : 0), 2 + i, 2, 7 - i, 2 + i);
            }

            return true;
        }
    };


    MU static bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& structureBB, const StructureComponent& piece) {
        bool result = false;

        switch (piece.type) {
            case PT_Mineshaft_Crossing:
                result = Crossing::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Mineshaft_Room:
                result = Room::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Mineshaft_Corridor:
                result = Corridor::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Mineshaft_Stairs:
                result = Stairs::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            default:;
        }
        return result;
    }


} // namespace build::mineshaft