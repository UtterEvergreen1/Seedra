#pragma


#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
namespace rolls {


    class MineshaftPiece : public StructureComponent {
    public:
        int mineshaftType = 0;

        MineshaftPiece() = delete;

        explicit MineshaftPiece(int mineshaftTypeIn) : mineshaftType(mineshaftTypeIn) {}


        MU ND const lce::blocks::Block* getBiomeSpecificPlank() const {
            switch (this->mineshaftType) {
                case 0: // normal
                default:
                    return &lce::blocks::BlocksInit::OAK_WOOD_PLANK;
                case 1: // mesa
                    return &lce::blocks::BlocksInit::DARK_OAK_WOOD_PLANK;
            }
        }


        MU ND const lce::blocks::Block* getBiomeSpecificFence() const {
            switch (this->mineshaftType) {
                case 0: // normal
                default:
                    return &lce::blocks::BlocksInit::OAK_FENCE;

                case 1: // mesa
                    return &lce::blocks::BlocksInit::DARK_OAK_FENCE;
            }
        }


        bool func_189918_a(World& p_189918_1_, const BoundingBox& p_189918_2_, int p_189918_3_, int p_189918_4_,
                           int p_189918_5_, int p_189918_6_) {
            for (int i = p_189918_3_; i <= p_189918_4_; ++i) {
                if (lce::blocks::ids::isReplaceableBlock(
                            getBlockStateFromPos(p_189918_1_, i, p_189918_5_ + 1, p_189918_6_, p_189918_2_)->getID())) {
                    return false;
                }
            }

            return true;
        }
    };


    class MU Room final : public MineshaftPiece {
    public:
        Room() = delete;

        MU explicit Room(int mineshaftTypeIn) : MineshaftPiece(mineshaftTypeIn) {}

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {


            if (isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                fillWithBlocks(worldIn, structureBB, minX, minY, minZ, maxX, minY, maxZ, &lce::blocks::BlocksInit::DIRT,
                               air, false);
                fillWithBlocks(worldIn, structureBB, minX, minY + 1, minZ, maxX, std::min((int) minY + 3, (int) maxY),
                               maxZ, air, air, false);

                // for (const BoundingBox& structureboundingbox: roomsLinkedToTheRoom) {
                //     fillWithBlocks(worldIn, structureBB, structureboundingbox.minX, structureboundingbox.maxY - 2,
                //                    structureboundingbox.minZ, structureboundingbox.maxX, structureboundingbox.maxY,
                //                    structureboundingbox.maxZ, air, air, false);
                // }

                randomlyRareFillWithBlocks(worldIn, structureBB, minX, minY + 4, minZ, maxX, maxY, maxZ, air, false);
                return true;
            }
        }
    };


    class MU Corridor final : public MineshaftPiece {
    public:
        Corridor() = delete;

        MU explicit Corridor(int mineshaftTypeIn) : MineshaftPiece(mineshaftTypeIn) {}


        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            bool hasRails = additionalData | 1;
            bool hasSpiders = (additionalData >> 1) | 1;
            bool spawnerPlaced = false;
            int sectionCount;
            if (getAxis(orientation) == EnumAxis::Z) {
                sectionCount = getZSize() / 5;
            } else {
                sectionCount = getZSize() / 5;
            }


            if (isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                int i = 0;
                int j = 2;
                int k = 0;
                int l = 2;
                int i1 = sectionCount * 5 - 1;

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                const lce::blocks::Block* iblockstate = getBiomeSpecificPlank();
                fillWithBlocks(worldIn, structureBB, 0, 0, 0, 2, 1, i1, air, air, false);
                func_189914_a(worldIn, structureBB, rng, 0.8F, 0, 2, 0, 2, 2, i1, air, air, false, 0);

                if (hasSpiders) {
                    func_189914_a(worldIn, structureBB, rng, 0.6F, 0, 0, 0, 2, 1, i1, air, air, false, 8);
                }

                for (int j1 = 0; j1 < sectionCount; ++j1) {
                    int k1 = 2 + j1 * 5;
                    func_189921_a(worldIn, structureBB, 0, 0, k1, 2, 2, rng);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 0, 2, k1 - 1);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 2, 2, k1 - 1);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 0, 2, k1 + 1);
                    func_189922_a(worldIn, structureBB, rng, 0.1F, 2, 2, k1 + 1);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 0, 2, k1 - 2);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 2, 2, k1 - 2);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 0, 2, k1 + 2);
                    func_189922_a(worldIn, structureBB, rng, 0.05F, 2, 2, k1 + 2);

                    // if (rng.nextInt(100) == 0) {
                    //     generateChest(worldIn, structureBB, rng, 2, 0, k1 - 1,
                    //                   LootTableList.CHESTS_ABANDONED_MINESHAFT);
                    // }

                    // if (rng.nextInt(100) == 0) {
                    //     generateChest(worldIn, structureBB, rng, 0, 0, k1 + 1,
                    //                   LootTableList.CHESTS_ABANDONED_MINESHAFT);
                    // }

                    if (hasSpiders && !spawnerPlaced) {
                        int l1 = getWorldY(0);
                        int i2 = k1 - 1 + rng.nextInt(3);
                        int j2 = getWorldX(1, i2);
                        int k2 = getWorldZ(1, i2);

                        Pos3D blockPos(j2, l1, k2);

                        if (structureBB.isVecInside(blockPos) && func_189916_b(worldIn, 1, 0, i2, structureBB) < 8) {
                            spawnerPlaced = true;
                            worldIn.setBlock(blockPos, &lce::blocks::BlocksInit::MONSTER_SPAWNER); // 2
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
                        int k3 = -1;
                        const lce::blocks::Block* iblockstate3 = getBlockStateFromPos(worldIn, l2, -1, i3, structureBB);

                        if (lce::blocks::ids::isReplaceableBlock(iblockstate3->getID()) &&
                            func_189916_b(worldIn, l2, -1, i3, structureBB) < 8) {
                            int l3 = -1;
                            setBlockState(worldIn, iblockstate, l2, -1, i3, structureBB);
                        }
                    }
                }

                if (hasRails) {
                    const lce::blocks::Block* iblockstate1 =
                            &lce::blocks::BlocksInit::
                                    RAIL; // .withProperty(BlockRail.SHAPE, BlockRailBase.EnumRailDirection.NORTH_SOUTH);

                    for (int j3 = 0; j3 <= i1; ++j3) {
                        const lce::blocks::Block* iblockstate2 = getBlockStateFromPos(worldIn, 1, -1, j3, structureBB);
                        if (lce::blocks::ids::isReplaceableBlock(iblockstate2->getID()) &&
                            lce::blocks::ids::isFullBlock(iblockstate1->getID())) {
                            float f = func_189916_b(worldIn, 1, 0, j3, structureBB) > 8 ? 0.9F : 0.7F;
                            randomlyPlaceBlock(worldIn, structureBB, rng, f, 1, 0, j3, iblockstate1);
                        }
                    }
                }

                return true;
            }
        }


        void func_189921_a(World& world, const BoundingBox& p_189921_2_, int p_189921_3_, int p_189921_4_,
                           int p_189921_5_, int p_189921_6_, int p_189921_7_, RNG& rng) {
            if (func_189918_a(world, p_189921_2_, p_189921_3_, p_189921_7_, p_189921_6_, p_189921_5_)) {
                const lce::blocks::Block* iblockstate = getBiomeSpecificPlank();
                const lce::blocks::Block* iblockstate1 = getBiomeSpecificFence();
                const lce::blocks::Block* iblockstate2 = &lce::blocks::BlocksInit::AIR;
                fillWithBlocks(world, p_189921_2_, p_189921_3_, p_189921_4_, p_189921_5_, p_189921_3_,
                                    p_189921_6_ - 1, p_189921_5_, iblockstate1, iblockstate2, false);
                fillWithBlocks(world, p_189921_2_, p_189921_7_, p_189921_4_, p_189921_5_, p_189921_7_,
                                    p_189921_6_ - 1, p_189921_5_, iblockstate1, iblockstate2, false);

                if (rng.nextInt(4) == 0) {
                    fillWithBlocks(world, p_189921_2_, p_189921_3_, p_189921_6_, p_189921_5_, p_189921_3_,
                                        p_189921_6_, p_189921_5_, iblockstate, iblockstate2, false);
                    fillWithBlocks(world, p_189921_2_, p_189921_7_, p_189921_6_, p_189921_5_, p_189921_7_,
                                        p_189921_6_, p_189921_5_, iblockstate, iblockstate2, false);
                } else {
                    fillWithBlocks(world, p_189921_2_, p_189921_3_, p_189921_6_, p_189921_5_, p_189921_7_,
                                        p_189921_6_, p_189921_5_, iblockstate, iblockstate2, false);
                    randomlyPlaceBlock(
                            world, p_189921_2_, rng, 0.05F, p_189921_3_ + 1, p_189921_6_, p_189921_5_ - 1,
                            &lce::blocks::BlocksInit::TORCH); // .withProperty(BlockTorch.FACING, EnumFacing.NORTH));
                    randomlyPlaceBlock(
                            world, p_189921_2_, rng, 0.05F, p_189921_3_ + 1, p_189921_6_, p_189921_5_ + 1,
                            &lce::blocks::BlocksInit::TORCH); // .withProperty(BlockTorch.FACING, EnumFacing.SOUTH));
                }
            }
        }

        void func_189922_a(World& world, const BoundingBox& p_189922_2_, RNG& rng, float p_189922_4_,
                           int p_189922_5_, int p_189922_6_, int p_189922_7_) {
            if (func_189916_b(world, p_189922_5_, p_189922_6_, p_189922_7_, p_189922_2_) < 8) {
                randomlyPlaceBlock(world, p_189922_2_, rng, p_189922_4_, p_189922_5_, p_189922_6_,
                                        p_189922_7_, &lce::blocks::BlocksInit::COBWEB);
            }
        }
    };


    class MU Crossing final : public MineshaftPiece {
    public:
        Crossing() = delete;

        MU explicit Crossing(int mineshaftTypeIn) : MineshaftPiece(mineshaftTypeIn) {}

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            bool isMultipleFloors;


            if (isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                const lce::blocks::Block* iblockstate = getBiomeSpecificPlank();

                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                if (isMultipleFloors) {
                    fillWithBlocks(worldIn, structureBB, minX + 1, minY, minZ, maxX - 1, minY + 3 - 1, maxZ, air, air,
                                   false);
                    fillWithBlocks(worldIn, structureBB, minX, minY, minZ + 1, maxX, minY + 3 - 1, maxZ - 1, air, air,
                                   false);
                    fillWithBlocks(worldIn, structureBB, minX + 1, maxY - 2, minZ, maxX - 1, maxY, maxZ, air, air,
                                   false);
                    fillWithBlocks(worldIn, structureBB, minX, maxY - 2, minZ + 1, maxX, maxY, maxZ - 1, air, air,
                                   false);
                    fillWithBlocks(worldIn, structureBB, minX + 1, minY + 3, minZ + 1, maxX - 1, minY + 3, maxZ - 1,
                                   air, air, false);
                } else {
                    fillWithBlocks(worldIn, structureBB, minX + 1, minY, minZ, maxX - 1, maxY, maxZ, air, air, false);
                    fillWithBlocks(worldIn, structureBB, minX, minY, minZ + 1, maxX, maxY, maxZ - 1, air, air, false);
                }

                func_189923_b(worldIn, structureBB, minX + 1, minY, minZ + 1, maxY);
                func_189923_b(worldIn, structureBB, minX + 1, minY, maxZ - 1, maxY);
                func_189923_b(worldIn, structureBB, maxX - 1, minY, minZ + 1, maxY);
                func_189923_b(worldIn, structureBB, maxX - 1, minY, maxZ - 1, maxY);

                for (int i = minX; i <= maxX; ++i) {
                    for (int j = minZ; j <= maxZ; ++j) {
                        if (lce::blocks::ids::isReplaceableBlock(
                                    getBlockStateFromPos(worldIn, i, minY - 1, j, structureBB)->getID()) &&
                            func_189916_b(worldIn, i, minY - 1, j, structureBB) < 8) {
                            setBlockState(worldIn, iblockstate, i, minY - 1, j, structureBB);
                        }
                    }
                }

                return true;
            }
        }


        void func_189923_b(World& world, const BoundingBox& p_189923_2_, int p_189923_3_, int p_189923_4_,
                           int p_189923_5_, int p_189923_6_) {

            if (!lce::blocks::ids::isReplaceableBlock(
                        getBlockStateFromPos(world, p_189923_3_, p_189923_6_ + 1, p_189923_5_, p_189923_2_)
                                ->getID())) {
                fillWithBlocks(world, p_189923_2_, p_189923_3_, p_189923_4_, p_189923_5_, p_189923_3_,
                               p_189923_6_, p_189923_5_, getBiomeSpecificPlank(), &lce::blocks::BlocksInit::AIR, false);
            }
        }
    };


    class MU Stairs final : public MineshaftPiece {
    public:
        Stairs() = delete;

        MU explicit Stairs(int mineshaftTypeIn) : MineshaftPiece(mineshaftTypeIn) {}

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (isLiquidInStructureBoundingBox(worldIn, structureBB)) {
                return false;
            } else {
                const lce::blocks::Block* air = &lce::blocks::BlocksInit::AIR;

                fillWithBlocks(worldIn, structureBB, 0, 5, 0, 2, 7, 1, air, air, false);
                fillWithBlocks(worldIn, structureBB, 0, 0, 7, 2, 2, 8, air, air, false);

                for (int i = 0; i < 5; ++i) {
                    fillWithBlocks(worldIn, structureBB, 0, 5 - i - (i < 4 ? 1 : 0), 2 + i, 2, 7 - i, 2 + i, air, air,
                                   false);
                }

                return true;
            }
        }
    };
} // namespace rolls