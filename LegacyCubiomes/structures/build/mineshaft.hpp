#pragma


#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
namespace build::mineshaft {




    MU ND static const lce::blocks::Block* getBiomeSpecificPlank(int structureType) {
        switch (structureType) {
            case 0: // normal
            default:
                return &lce::blocks::BlocksInit::OAK_WOOD_PLANK;
            case 1: // mesa
                return &lce::blocks::BlocksInit::DARK_OAK_WOOD_PLANK;
        }
    }


    MU ND static const lce::blocks::Block* getBiomeSpecificFence(int structureType) {
        switch (structureType) {
            case 0: // normal
            default:
                return &lce::blocks::BlocksInit::OAK_FENCE;
            case 1: // mesa
                return &lce::blocks::BlocksInit::DARK_OAK_FENCE;
        }
    }


    static bool func_189918_a(World& world, const BoundingBox& structureBB, int p_189918_3_, int p_189918_4_,
                              int p_189918_5_, int z, StructureComponent& piece) {
        for (int i = p_189918_3_; i <= p_189918_4_; ++i) {
            if (lce::blocks::ids::isReplaceableBlock(
                        piece.getBlockStateFromPos(world, i, p_189918_5_ + 1, z, structureBB)->getID())) {
                return false;
            }
        }

        return true;
    }



    class MU Room final {
    public:
        MU Room() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {


            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                piece.fillWithBlocks(worldIn, structureBB, 0, 0, 0, piece.maxX - piece.minX, 0, piece.maxZ - piece.minZ,
                                     &lce::blocks::BlocksInit::DIRT, air, false);
                piece.fillWithBlocks(worldIn, structureBB, 0, 1, 0, piece.maxX - piece.minX, std::min((int) 3, (int) piece.maxY - piece.minY),
                                     piece.maxZ - piece.minZ, air, air, false);

                // for (const BoundingBox& sbb: roomsLinkedToTheRoom) {
                //     piece.fillWithBlocks(worldIn, structureBB, sbb.piece.minX, sbb.piece.maxY - 2,
                //                    sbb.piece.minZ, sbb.piece.maxX, sbb.piece.maxY,
                //                    sbb.piece.maxZ, air, air, false);
                // }

                piece.randomlyRareFillWithBlocks(worldIn, structureBB, 0, 4, 0, piece.maxX - piece.minX, piece.maxY - piece.minY, piece.maxZ - piece.minZ, air,
                                           false);
                return true;
            }
        }
    };


    class MU Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            bool hasRails = piece.data & 1;
            bool hasSpiders = (piece.data >> 1) & 1;
            bool spawnerPlaced = false;
            int sectionCount;
            if (getAxis(piece.facing) == EnumAxis::Z) {
                sectionCount = piece.getZSize() / 5;
            } else {
                sectionCount = piece.getXSize() / 5;
            }


            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                // int i = 0;
                // int j = 2;
                // int k = 0;
                // int l = 2;
                int i1 = sectionCount * 5 - 1;

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                const lce::blocks::Block* iblockstate = getBiomeSpecificPlank(piece.structureType);
                piece.fillWithBlocks(worldIn, structureBB, 0, 0, 0, 2, 1, i1, air, air, false);
                piece.func_189914_a(worldIn, structureBB, rng, 0.8F, 0, 2, 0, 2, 2, i1, air, air, false, 0);

                if (hasSpiders) {
                    piece.func_189914_a(worldIn, structureBB, rng, 0.6F, 0, 0, 0, 2, 1, i1, air, air, false, 8);
                }

                for (int j1 = 0; j1 < sectionCount; ++j1) {
                    int k1 = 2 + j1 * 5;
                    func_189921_a(worldIn, structureBB, 0, 0, k1, 2, 2, rng, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 0, 2, k1 - 1, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 2, 2, k1 - 1, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 0, 2, k1 + 1, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 2, 2, k1 + 1, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 0, 2, k1 - 2, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 2, 2, k1 - 2, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 0, 2, k1 + 2, piece);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 2, 2, k1 + 2, piece);

                    // if (rng.nextInt(100) == 0) {
                    //     generateChest(worldIn, structureBB, rng, 2, 0, k1 - 1,
                    //                   LootTableList.CHESTS_ABANDONED_MINESHAFT);
                    // }

                    // if (rng.nextInt(100) == 0) {
                    //     generateChest(worldIn, structureBB, rng, 0, 0, k1 + 1,
                    //                   LootTableList.CHESTS_ABANDONED_MINESHAFT);
                    // }

                    if (hasSpiders && !spawnerPlaced) {
                        int l1 = piece.getWorldY(0);
                        int i2 = k1 - 1 + rng.nextInt(3);
                        int j2 = piece.getWorldX(1, i2);
                        int k2 = piece.getWorldZ(1, i2);

                        Pos3D blockPos(j2, l1, k2);

                        if (structureBB.isVecInside(blockPos) &&
                            piece.getLightLevelAtBlock(worldIn, 1, 0, i2, structureBB) < 8) {
                            spawnerPlaced = true;
                            piece.setBlockStateWithoutOffset(worldIn, &lce::blocks::BlocksInit::MONSTER_SPAWNER,
                                                             j2, l1, k2, structureBB);
                            // TileEntity tileentity = worldIn.getTileEntity(blockpos);
                            // if (tileentity instanceof TileEntityMobSpawner) {
                            //     ((TileEntityMobSpawner) tileentity)
                            //             .getSpawnerBaseLogic()
                            //             .func_190894_a(EntityList.func_191306_a(EntityCaveSpider.class));
                            // }
                        }
                    }
                }

                for (int l2 = 0; l2 <= 2; ++l2) {
                    for (int i3 = 0; i3 <= i1; ++i3) {
                        MU int k3 = -1;
                        const lce::blocks::Block* iblockstate3 = piece.getBlockStateFromPos(worldIn, l2, -1, i3, structureBB);

                        if (lce::blocks::ids::isReplaceableBlock(iblockstate3->getID()) &&
                            piece.getLightLevelAtBlock(worldIn, l2, -1, i3, structureBB) < 8) {
                            MU int l3 = -1;
                            piece.setBlockState(worldIn, iblockstate, l2, -1, i3, structureBB);
                        }
                    }
                }

                if (hasRails) {
                    // .withProperty(BlockRail.SHAPE, BlockRailBase.EnumRailDirection.NORTH_SOUTH);
                    const lce::blocks::Block* iblockstate1 = &lce::blocks::BlocksInit::RAIL;

                    for (int j3 = 0; j3 <= i1; ++j3) {
                        const lce::blocks::Block* iblockstate2 = piece.getBlockStateFromPos(worldIn, 1, -1, j3, structureBB);
                        if (lce::blocks::ids::isReplaceableBlock(iblockstate2->getID()) &&
                            lce::blocks::ids::isFullBlock(iblockstate1->getID())) {
                            float f = piece.getLightLevelAtBlock(worldIn, 1, 0, j3, structureBB) > 8 ? 0.9F : 0.7F;
                            piece.randomlyPlaceBlock(worldIn, structureBB, rng, f, 1, 0, j3, iblockstate1);
                        }
                    }
                }

                return true;
            }
        }


        static void func_189921_a(World& world, const BoundingBox& structureBB, int p_189921_3_, int p_189921_4_,
                                  int p_189921_5_, int p_189921_6_, int p_189921_7_, RNG& rng, StructureComponent& piece) {
            if (func_189918_a(world, structureBB, p_189921_3_, p_189921_7_, p_189921_6_, p_189921_5_, piece)) {
                const lce::blocks::Block* iblockstate = getBiomeSpecificPlank(piece.structureType);
                const lce::blocks::Block* iblockstate1 = getBiomeSpecificFence(piece.structureType);
                const lce::blocks::Block* iblockstate2 = &lce::blocks::BlocksInit::AIR;
                piece.fillWithBlocks(world, structureBB, p_189921_3_, p_189921_4_, p_189921_5_, p_189921_3_, p_189921_6_ - 1,
                                     p_189921_5_, iblockstate1, iblockstate2, false);
                piece.fillWithBlocks(world, structureBB, p_189921_7_, p_189921_4_, p_189921_5_, p_189921_7_, p_189921_6_ - 1,
                                     p_189921_5_, iblockstate1, iblockstate2, false);

                if (rng.nextInt(4) == 0) {
                    piece.fillWithBlocks(world, structureBB, p_189921_3_, p_189921_6_, p_189921_5_, p_189921_3_, p_189921_6_,
                                         p_189921_5_, iblockstate, iblockstate2, false);
                    piece.fillWithBlocks(world, structureBB, p_189921_7_, p_189921_6_, p_189921_5_, p_189921_7_, p_189921_6_,
                                         p_189921_5_, iblockstate, iblockstate2, false);
                } else {
                    piece.fillWithBlocks(world, structureBB, p_189921_3_, p_189921_6_, p_189921_5_, p_189921_7_, p_189921_6_,
                                         p_189921_5_, iblockstate, iblockstate2, false);
                    piece.randomlyPlaceBlock(
                            world, structureBB, rng, 0.05F, p_189921_3_ + 1, p_189921_6_, p_189921_5_ - 1,
                            &lce::blocks::BlocksInit::TORCH); // .withProperty(BlockTorch.FACING, EnumFacing.NORTH));
                    piece.randomlyPlaceBlock(
                            world, structureBB, rng, 0.05F, p_189921_3_ + 1, p_189921_6_, p_189921_5_ + 1,
                            &lce::blocks::BlocksInit::TORCH); // .withProperty(BlockTorch.FACING, EnumFacing.SOUTH));
                }
            }
        }

        static void func_189922_a(World& world, const BoundingBox& structureBB, RNG& rng, float chance, int x, int y, int z, StructureComponent& piece) {
            if (piece.getLightLevelAtBlock(world, x, y, z, structureBB) < 8) {
                piece.randomlyPlaceBlock(world, structureBB, rng, chance, x, y, z, &lce::blocks::BlocksInit::COBWEB);
            }
        }
    };


    class MU Crossing final {
    public:
        MU Crossing() = delete;
        
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            bool isMultipleFloors = piece.data & 1;


            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {

                const lce::blocks::Block* iblockstate = getBiomeSpecificPlank(piece.structureType);

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                if (isMultipleFloors) {
                    piece.fillWithBlocks(worldIn, structureBB, 1, 0, 0, piece.maxX - piece.minX - 1, 3 - 1, piece.maxZ - piece.minZ, air, air, false);
                    piece.fillWithBlocks(worldIn, structureBB, 0, 0, 1, piece.maxX - piece.minX, 3 - 1, piece.maxZ - piece.minZ - 1, air, air, false);
                    piece.fillWithBlocks(worldIn, structureBB, 1, -2, 0, piece.maxX - piece.minX - 1, piece.maxY - piece.minY, piece.maxZ - piece.minZ, air, air, false);
                    piece.fillWithBlocks(worldIn, structureBB, 0, -2, 1, piece.maxX - piece.minX, piece.maxY - piece.minY, piece.maxZ - piece.minZ - 1, air, air, false);
                    piece.fillWithBlocks(worldIn, structureBB, 1, 3, 1, piece.maxX - piece.minX - 1, 3, piece.maxZ - piece.minZ - 1, air, air, false);
                } else {
                    piece.fillWithBlocks(worldIn, structureBB, 1, 0, 0, piece.maxX - piece.minX - 1, piece.maxY - piece.minY, piece.maxZ - piece.minZ, air, air, false);
                    piece.fillWithBlocks(worldIn, structureBB, 0, 0, 1, piece.maxX - piece.minX, piece.maxY - piece.minY, piece.maxZ - piece.minZ - 1, air, air, false);
                }

                placePlankPillar(worldIn, structureBB, 1, 0, 1, piece.maxY - piece.minY, piece);
                placePlankPillar(worldIn, structureBB, 1, 0, piece.maxZ - piece.minZ - 1, piece.maxY - piece.minY, piece);
                placePlankPillar(worldIn, structureBB, piece.maxX - piece.minX - 1, 0, 1, piece.maxY - piece.minY, piece);
                placePlankPillar(worldIn, structureBB, piece.maxX - piece.minX - 1, 0, piece.maxZ - piece.minZ - 1, piece.maxY - piece.minY, piece);

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
        }


        static void placePlankPillar(World& world, const BoundingBox& structureBB, int x, int minY, int z, int maxY,
                                     StructureComponent& piece) {
            auto block = piece.getBlockStateFromPos(world, x, maxY + 1, z, structureBB);
            if (!lce::blocks::ids::isReplaceableBlock(block->getID())) {
                piece.fillWithBlocks(world, structureBB, x, minY, z, x, maxY, z, getBiomeSpecificPlank(piece.structureType),
                                     &lce::blocks::BlocksInit::AIR, false);
            }
        }
    };


    class MU Stairs final {
    public:
        MU Stairs() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
            if (piece.isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                piece.fillWithBlocks(worldIn, structureBB, 0, 5, 0, 2, 7, 1, air, air, false);
                piece.fillWithBlocks(worldIn, structureBB, 0, 0, 7, 2, 2, 8, air, air, false);

                for (int i = 0; i < 5; ++i) {
                    piece.fillWithBlocks(worldIn, structureBB,
                                         0, 5 - i - (i < 4 ? 1 : 0), 2 + i, 2, 7 - i, 2 + i, air, air,
                                         false);
                }

                return true;
            }
        }
    };


    /*
    /// user must manually free
    MineshaftPiece* getMineshaftPiece(const PieceType pieceType) {
        build::MineshaftPiece* piecePtr = nullptr;
        switch (pieceType) {
            case PieceType::PT_Mineshaft_Crossing:
                piecePtr = new build::Crossing();
                break;
            case PieceType::PT_Mineshaft_Room:
                piecePtr = new build::Room();
                break;
            case PieceType::PT_Mineshaft_Corridor:
                piecePtr = new build::Corridor();
                break;
            case PieceType::PT_Mineshaft_Stairs:
                piecePtr = new build::Stairs(0);
                break;
            default:;
        }
        return piecePtr;
    }
     */



    MU static bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {

        bool result = false;

        switch (piece.type) {
            case PieceType::PT_Mineshaft_Crossing:
                result = Crossing::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Mineshaft_Room:
                result = Room::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Mineshaft_Corridor:
                result = Corridor::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PieceType::PT_Mineshaft_Stairs:
                result = Stairs::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            default:;
        }
        return result;
    }





} // namespace build::mineshaft