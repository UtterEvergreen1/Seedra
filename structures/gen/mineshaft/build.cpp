#include "mineshaft.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace build::mineshaft {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    lce::BlockState getBiomeSpecificPlank(const StructureVariant variant) {
        switch (variant) {
            case StructureVariant::SV_Mineshaft_Normal:
            default:
                return lce::BlocksInit::OAK_WOOD_PLANK.getState();
            case StructureVariant::SV_Mineshaft_Mesa:
                return lce::BlocksInit::DARK_OAK_WOOD_PLANK.getState();
        }
    }


    lce::BlockState getBiomeSpecificFence(const StructureVariant variant) {
        switch (variant) {
            case StructureVariant::SV_Mineshaft_Normal:
            default:
                return lce::BlocksInit::OAK_FENCE.getState();
            case StructureVariant::SV_Mineshaft_Mesa:
                return lce::BlocksInit::DARK_OAK_FENCE.getState();
        }
    }


    bool func_189918_a(World& world, const BoundingBox& chunkBB, c_int minX, c_int maxX, c_int y,
                       c_int z, const StructureComponent& piece) {
        for (int x = minX; x <= maxX; ++x) {
            if (isReplaceableBlock(
                        piece.getBlockStateFromPos(world, x, y + 1, z, chunkBB).getID())) {
                return false;
            }
        }

        return true;
    }


    bool Corridor::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

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

        constexpr lce::BlockState air = lce::BlocksInit::AIR.getState();

        const lce::BlockState iblockstate = getBiomeSpecificPlank(piece.variant);
        piece.fillWithAir(worldIn, chunkBB, 0, 0, 0, 2, 1, i1);
        piece.fillWithBlocksRandomLightCheck(worldIn, chunkBB, rng, 0.8F, 0, 2, 0, 2, 2, i1, air, air, false, 0);

        if (hasSpiders) {
            piece.fillWithBlocksRandomLightCheck(worldIn, chunkBB, rng, 0.6F, 0, 0, 0, 2, 1, i1, air, air, false, 8);
        }

        for (int j1 = 0; j1 < sectionCount; ++j1) {
            c_int k1 = 2 + j1 * 5;
            func_189921_a(worldIn, chunkBB, 0, 0, k1, 2, 2, rng, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.1F, 0, 2, k1 - 1, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.1F, 2, 2, k1 - 1, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.1F, 0, 2, k1 + 1, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.1F, 2, 2, k1 + 1, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.05F, 0, 2, k1 - 2, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.05F, 2, 2, k1 - 2, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.05F, 0, 2, k1 + 2, piece);
            placeCobwebsRandomly(worldIn, chunkBB, rng, 0.05F, 2, 2, k1 + 2, piece);

            if (rng.nextInt(100) == 0) {
                // generateChest(worldIn, chunkBB, rng, 2, 0, k1 - 1, LootTableList.CHESTS_ABANDONED_MINESHAFT);
                piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(), 2, 0, k1 - 1, chunkBB);
                rng.nextLong();
            }


            if (rng.nextInt(100) == 0) {
                // generateChest(worldIn, chunkBB, rng, 0, 0, k1 + 1, LootTableList.CHESTS_ABANDONED_MINESHAFT);
                piece.setBlockState(worldIn, lce::BlocksInit::CHEST.getState(), 0, 0, k1 + 1, chunkBB);
                rng.nextLong();
            }

            if (hasSpiders && !spawnerPlaced) {
                c_int i2 = k1 - 1 + rng.nextInt(3);
                const Pos3D blockPos = piece.getWorldXYZ(1, 0, i2); // (j2, l1, k2);

                if (chunkBB.isVecInside(blockPos) && piece.getLightLevelAtBlock(worldIn, 1, 0, i2, chunkBB) < 8) {
                    spawnerPlaced = true;
                    // caveSpider
                    piece.setBlockState(worldIn, lce::BlocksInit::MONSTER_SPAWNER.getState(), 1, 0, 1, chunkBB);
                }
            }
        }

        for (int l2 = 0; l2 <= 2; ++l2) {
            for (int i3 = 0; i3 <= i1; ++i3) {
                MU int k3 = -1;
                const lce::BlockState iBlockState3 =
                        piece.getBlockStateFromPos(worldIn, l2, -1, i3, chunkBB);

                if (isReplaceableBlock(iBlockState3.getID()) &&
                    piece.getLightLevelAtBlock(worldIn, l2, -1, i3, chunkBB) < 8) {
                    MU int l3 = -1;
                    piece.setBlockState(worldIn, iblockstate, l2, -1, i3, chunkBB);
                }
            }
        }

        if (hasRails) {
            c_auto iblockstate1 = lce::BlocksInit::RAIL.getStateFromMeta(states::Rail::withProperty(states::RailShape::NS));

            for (int j3 = 0; j3 <= i1; ++j3) {
                const lce::BlockState iBlockState2 = piece.getBlockStateFromPos(worldIn, 1, -1, j3, chunkBB);
                if (!isReplaceableBlock(iBlockState2.getID()) && isFullBlock(iBlockState2.getID())) {
                    c_float f = piece.getLightLevelAtBlock(worldIn, 1, 0, j3, chunkBB) > 8 ? 0.9F : 0.7F;
                    piece.randomlyPlaceBlock(worldIn, chunkBB, rng, f, 1, 0, j3, iblockstate1);
                }
            }
        }

        return true;
    }




    void Corridor::func_189921_a(World& world, const BoundingBox& chunkBB,
                                 c_int minX, c_int minY, c_int minZ, c_int maxY, c_int maxX, RNG& rng,
                                 const StructureComponent& piece) {

        if (func_189918_a(world, chunkBB, minX, maxX, maxY, minZ, piece)) {
            const lce::BlockState plank = getBiomeSpecificPlank(piece.variant);
            const lce::BlockState fence = getBiomeSpecificFence(piece.variant);
            constexpr lce::BlockState air = lce::BlocksInit::AIR.getState();

            piece.fillWithBlocks(world, chunkBB, minX, minY, minZ, minX, maxY - 1, minZ, fence, air, false);
            piece.fillWithBlocks(world, chunkBB, maxX, minY, minZ, maxX, maxY - 1, minZ, fence, air, false);

            if (rng.nextInt(4) == 0) {
                piece.fillWithBlocks(world, chunkBB, minX, maxY, minZ, minX, maxY, minZ, plank, air, false);
                piece.fillWithBlocks(world, chunkBB, maxX, maxY, minZ, maxX, maxY, minZ, plank, air, false);
            } else {
                piece.fillWithBlocks(world, chunkBB, minX, maxY, minZ, maxX, maxY, minZ, plank, air, false);
                const lce::BlockState torchNorth = lce::BlocksInit::TORCH.getStateFromMeta(
                        states::Torch::withProperty(EnumFacing::NORTH));
                piece.randomlyPlaceBlock(world, chunkBB, rng, 0.05F, minX + 1, maxY, minZ - 1, torchNorth);
                const lce::BlockState torchSouth = lce::BlocksInit::TORCH.getStateFromMeta(
                        states::Torch::withProperty(EnumFacing::SOUTH));
                piece.randomlyPlaceBlock(world, chunkBB, rng, 0.05F, minX + 1, maxY, minZ + 1, torchSouth);
            }
        }
    }


    void Corridor::placeCobwebsRandomly(World& world, const BoundingBox& chunkBB, RNG& rng, c_float chance,
                                        c_int x, c_int y, c_int z, const StructureComponent& piece) {
        if (piece.getLightLevelAtBlock(world, x, y, z, chunkBB) < 8) {
            piece.randomlyPlaceBlock(world, chunkBB, rng, chance, x, y, z, lce::BlocksInit::COBWEB.getState());
        }
    }

    bool Room::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                                 const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }


        piece.fillWithBlocks(worldIn, chunkBB, 0, 0, 0, piece.maxX - piece.minX, 0, piece.maxZ - piece.minZ,
                             lce::BlocksInit::DIRT.getState(), lce::BlocksInit::AIR.getState(), false);
        piece.fillWithAir(worldIn, chunkBB, 0, 1, 0, piece.maxX - piece.minX,
                          std::min(3, piece.maxY - piece.minY), piece.maxZ - piece.minZ);

        // for (const BoundingBox& sbb: roomsLinkedToTheRoom) {
        //     piece.fillWithBlocks(worldIn, chunkBB, sbb.piece.minX, sbb.piece.maxY - 2,
        //                    sbb.piece.minZ, sbb.piece.maxX, sbb.piece.maxY,
        //                    sbb.piece.maxZ, air, air, false);
        // }

        piece.randomlyRareFillWithBlocks(worldIn, chunkBB, 0, 4, 0, piece.maxX - piece.minX,
                                         piece.maxY - piece.minY, piece.maxZ - piece.minZ, lce::BlocksInit::AIR.getState(), false);
        return true;
    }


    bool Crossing::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                                     const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        const bool isMultipleFloors = piece.data & 1;

        const lce::BlockState iblockstate = getBiomeSpecificPlank(piece.variant);

        if (isMultipleFloors) {
            piece.fillWithAir(worldIn, chunkBB, 1, 0, 0, piece.maxX - piece.minX - 1, 3 - 1,
                              piece.maxZ - piece.minZ);
            piece.fillWithAir(worldIn, chunkBB, 0, 0, 1, piece.maxX - piece.minX, 3 - 1,
                              piece.maxZ - piece.minZ - 1);
            piece.fillWithAir(worldIn, chunkBB, 1, -2, 0, piece.maxX - piece.minX - 1, piece.maxY - piece.minY,
                              piece.maxZ - piece.minZ);
            piece.fillWithAir(worldIn, chunkBB, 0, -2, 1, piece.maxX - piece.minX, piece.maxY - piece.minY,
                              piece.maxZ - piece.minZ - 1);
            piece.fillWithAir(worldIn, chunkBB, 1, 3, 1, piece.maxX - piece.minX - 1, 3,
                              piece.maxZ - piece.minZ - 1);
        } else {
            piece.fillWithAir(worldIn, chunkBB, 1, 0, 0, piece.maxX - piece.minX - 1, piece.maxY - piece.minY,
                              piece.maxZ - piece.minZ);
            piece.fillWithAir(worldIn, chunkBB, 0, 0, 1, piece.maxX - piece.minX, piece.maxY - piece.minY,
                              piece.maxZ - piece.minZ - 1);
        }

        placePlankPillar(worldIn, chunkBB, 1, 0, 1, piece.maxY - piece.minY, piece);
        placePlankPillar(worldIn, chunkBB, 1, 0, piece.maxZ - piece.minZ - 1, piece.maxY - piece.minY, piece);
        placePlankPillar(worldIn, chunkBB, piece.maxX - piece.minX - 1, 0, 1, piece.maxY - piece.minY, piece);
        placePlankPillar(worldIn, chunkBB, piece.maxX - piece.minX - 1, 0, piece.maxZ - piece.minZ - 1,
                         piece.maxY - piece.minY, piece);

        for (int i = 0; i <= piece.maxX - piece.minX; ++i) {
            for (int j = 0; j <= piece.maxZ - piece.minZ; ++j) {
                if (isReplaceableBlock(
                            piece.getBlockStateFromPos(worldIn, i, -1, j, chunkBB).getID()) &&
                    piece.getLightLevelAtBlock(worldIn, i, -1, j, chunkBB) < 8) {
                    piece.setBlockState(worldIn, iblockstate, i, -1, j, chunkBB);
                }
            }
        }

        return true;
    }


    void Crossing::placePlankPillar(World& world, const BoundingBox& chunkBB, c_int x, c_int minY,
                                    c_int z, c_int maxY, const StructureComponent& piece) {

        c_auto block = piece.getBlockStateFromPos(world, x, maxY + 1, z, chunkBB);

        if (!isReplaceableBlock(block.getID())) {
            const lce::BlockState plank = getBiomeSpecificPlank(piece.variant);
            piece.fillWithBlocks(world, chunkBB, x, minY, z, x, maxY, z, plank, lce::BlocksInit::AIR.getState(), false);
        }
    }


    bool Stairs::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                                   const StructureComponent& piece) {
        if (piece.isLiquidInStructureBoundingBox(worldIn, chunkBB)) { return false; }

        piece.fillWithAir(worldIn, chunkBB, 0, 5, 0, 2, 7, 1);
        piece.fillWithAir(worldIn, chunkBB, 0, 0, 7, 2, 2, 8);
        for (int i = 0; i < 5; ++i) {
            piece.fillWithAir(worldIn, chunkBB, 0, 5 - i - (i < 4 ? 1 : 0), 2 + i, 2, 7 - i, 2 + i);
        }

        return true;
    }


    bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece) {
        bool result = false;

        switch (piece.type) {
            case PT_Mineshaft_Crossing:
                result = Crossing::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Mineshaft_Room:
                result = Room::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Mineshaft_Corridor:
                result = Corridor::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            case PT_Mineshaft_Stairs:
                result = Stairs::addComponentParts(worldIn, rng, chunkBB, piece);
                break;
            default:;
        }
        return result;
    }
}



