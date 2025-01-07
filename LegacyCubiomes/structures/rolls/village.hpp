#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"

#include "LegacyCubiomes/structures/generation/village/village.hpp"
#include "RollsBase.hpp"
#include "StructureComponent.hpp"
#include "lce/blocks/blocks.hpp"


namespace structure_rolls {

    class Village : public RollsBase, public StructureComponent {
    protected:
        MU int villagersSpawned{};
        generation::Village* villageData{};
        int averageGroundLvl = -1;
        int structureType = -1;
        
    public:

        Village() = delete;

        explicit Village(generation::Village* villageIn, int structureTypeIn) {
            villageData = villageIn;
            structureType = structureTypeIn;
        }

        const lce::blocks::Block* getBiomeSpecificBlockState(const lce::blocks::Block* blockPtr) const {


            if (this->structureType == 1) {
                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD/* || blockPtr == Blocks.LOG2*/) {
                    return &lce::blocks::BlocksInit::SANDSTONE;
                }

                if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE) {
                    return &lce::blocks::BlocksInit::SANDSTONE;
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                    return &lce::blocks::BlocksInit::SMOOTH_SANDSTONE;
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                    return &lce::blocks::BlocksInit::SANDSTONE_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                }

                if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE_STAIRS) {
                    return &lce::blocks::BlocksInit::SANDSTONE_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                }

                if (blockPtr == &lce::blocks::BlocksInit::GRAVEL) {
                    return &lce::blocks::BlocksInit::SANDSTONE;
                }
            } else if (this->structureType == 3) {
                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD/* || blockPtr == Blocks.LOG2*/) {
                    return &lce::blocks::BlocksInit::SPRUCE_WOOD; // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                    return &lce::blocks::BlocksInit::SPRUCE_WOOD_PLANK;
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                    return &lce::blocks::BlocksInit::SPRUCE_WOOD_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_FENCE) {
                    return &lce::blocks::BlocksInit::SPRUCE_FENCE;
                }
            } else if (this->structureType == 2) {
                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD/* || blockPtr == Blocks.LOG2*/) {
                    return &lce::blocks::BlocksInit::ACACIA_WOOD; // .withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_PLANK) {
                    return &lce::blocks::BlocksInit::ACACIA_WOOD_PLANK;
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_WOOD_STAIRS) {
                    return &lce::blocks::BlocksInit::ACACIA_WOOD_STAIRS; // .withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                }

                if (blockPtr == &lce::blocks::BlocksInit::COBBLESTONE) {
                    return &lce::blocks::BlocksInit::ACACIA_WOOD; // .withProperty(BlockLog.LOG_AXIS, BlockLog.EnumAxis.Y);
                }

                if (blockPtr == &lce::blocks::BlocksInit::OAK_FENCE) {
                    return &lce::blocks::BlocksInit::ACACIA_FENCE;
                }
            }

            return blockPtr;
        }

        MU ND int getAverageGroundLevel(World& worldIn, const BoundingBox& structureBB) const {
            int i = 0;
            int j = 0;
            Pos3D blockPos;

            for (int k = piece.minZ; k <= piece.maxZ; ++k) {
                for (int l = piece.minX; l <= piece.maxX; ++l) {
                    blockPos.setPos(l, 64, k);

                    if (structureBB.isVecInside(blockPos)) {
                        i += std::max(worldIn.getTopSolidOrLiquidBlock(blockPos).getY(), 64);
                        ++j;
                    }
                }
            }

            if (j == 0) {
                return -1;
            } else {
                return i / j;
            }
        }

    void func_189927_a(MU const World& p_189927_1_, MU BoundingBox p_189927_2_, MU RNG& p_189927_3_, MU int p_189927_4_, MU int p_189927_5_, MU int p_189927_6_, MU FACING p_189927_7_) const {
            if (!this->villageData->isZombieInfested) {
                // this->func_189915_a(p_189927_1_, p_189927_2_, p_189927_3_, p_189927_4_, p_189927_5_, p_189927_6_, FACING::NORTH, this->func_189925_i());
            }
        }

    void func_189926_a(MU const World& p_189926_1_, MU FACING p_189926_2_, MU int p_189926_3_, MU int p_189926_4_, MU int p_189926_5_, MU BoundingBox p_189926_6_) const {
            if (!this->villageData->isZombieInfested) {
                // this->setBlockState(p_189926_1_, Blocks.TORCH.getDefaultState().withProperty(BlockTorch.FACING, p_189926_2_), p_189926_3_, p_189926_4_, p_189926_5_, p_189926_6_);
            }
        }

    };
    

    class MU Church final : public Village {
    public:
        Church() = delete;

        MU explicit Church(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 12 - 1, 0);
            }

            const lce::blocks::Block* cobblestone = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::EAST));
            fillWithBlocks(worldIn, piece, 1, 1, 1, 3, 3, 7, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 1, 5, 1, 3, 9, 3, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 1, 0, 0, 3, 0, 8, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 1, 1, 0, 3, 10, 0, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 0, 1, 1, 0, 10, 3, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 4, 1, 1, 4, 10, 3, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 0, 0, 4, 0, 4, 7, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 4, 0, 4, 4, 4, 7, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 1, 1, 8, 3, 4, 8, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 1, 5, 4, 3, 10, 4, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 1, 5, 5, 3, 5, 7, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 0, 9, 0, 4, 9, 4, cobblestone, cobblestone, false);
            fillWithBlocks(worldIn, piece, 0, 4, 0, 4, 4, 4, cobblestone, cobblestone, false);
            this->setBlockState(worldIn, cobblestone, 0, 11, 2, structureBB);
            this->setBlockState(worldIn, cobblestone, 4, 11, 2, structureBB);
            this->setBlockState(worldIn, cobblestone, 2, 11, 0, structureBB);
            this->setBlockState(worldIn, cobblestone, 2, 11, 4, structureBB);
            this->setBlockState(worldIn, cobblestone, 1, 1, 6, structureBB);
            this->setBlockState(worldIn, cobblestone, 1, 1, 7, structureBB);
            this->setBlockState(worldIn, cobblestone, 2, 1, 7, structureBB);
            this->setBlockState(worldIn, cobblestone, 3, 1, 6, structureBB);
            this->setBlockState(worldIn, cobblestone, 3, 1, 7, structureBB);
            this->setBlockState(worldIn, iBlockState1, 1, 1, 5, structureBB);
            this->setBlockState(worldIn, iBlockState1, 2, 1, 6, structureBB);
            this->setBlockState(worldIn, iBlockState1, 3, 1, 5, structureBB);
            this->setBlockState(worldIn, iBlockState2, 1, 2, 7, structureBB);
            this->setBlockState(worldIn, iBlockState3, 3, 2, 7, structureBB);
            const lce::blocks::Block* glassPane = &lce::blocks::BlocksInit::GLASS_PANE;
            this->setBlockState(worldIn, glassPane, 0, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 3, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 3, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 6, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 7, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 6, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 7, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 6, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 7, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 6, 4, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 7, 4, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 3, 6, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 3, 6, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 3, 8, structureBB);
            this->func_189926_a(worldIn, FACING::SOUTH, 2, 4, 7, structureBB);
            this->func_189926_a(worldIn, FACING::EAST, 1, 4, 6, structureBB);
            this->func_189926_a(worldIn, FACING::WEST, 3, 4, 6, structureBB);
            this->func_189926_a(worldIn, FACING::NORTH, 2, 4, 5, structureBB);
            const lce::blocks::Block* iBlockState4 = &lce::blocks::BlocksInit::LADDER; // .withProperty(BlockLadder.FACING, FACING::WEST);

            for (int i=1; i <= 9; ++i) {
                this->setBlockState(worldIn, iBlockState4, 3, i, 3, structureBB);
            }

            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 1, 0, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 2, 0, structureBB);
            this->func_189927_a(worldIn, structureBB, rng, 2, 1, 0, FACING::NORTH);

            if (lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID()) &&
                !lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID())) {
                this->setBlockState(worldIn, iBlockState1, 2, 0, -1, structureBB);

                if (this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                    this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, 2, -1, -1, structureBB);
                }
            }

            for (int k=0; k < 9; ++k) {
                for (int j=0; j < 5; ++j) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, j, 12, k, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, cobblestone, j, -1, k, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 2, 1, 2, 1);
            return true;
        }

    };

    class MU Field1 final : public Village {
    public:
        Field1() = delete;

        MU explicit Field1(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        MU static const lce::blocks::Block* getRandomCropType(const int data) {
            switch (data) {
                case 0:
                case 1:
                    return &lce::blocks::BlocksInit::CARROTS;
                case 2:
                case 3:
                    return &lce::blocks::BlocksInit::POTATOES;
                case 4:
                    return &lce::blocks::BlocksInit::BEETROOT_BLOCK;
                default:
                    return &lce::blocks::BlocksInit::WHEAT_CROPS;
            }
        }


        MU static int getMaxAgeFromCrop(int id) {
            switch (id) {
                case lce::blocks::ids::CARROTS_ID:
                case lce::blocks::ids::POTATOES_ID:
                case lce::blocks::ids::BEETROOT_BLOCK_ID:
                default:
                    return 4;
                case lce::blocks::ids::WHEAT_CROPS_ID:
                    return 8;
            }
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 4 - 1, 0);
            }

            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            fillWithBlocks(worldIn, piece, 0, 1, 0, 12, 4, 8, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 1, 0, 1, 2, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND, false);
            fillWithBlocks(worldIn, piece, 4, 0, 1, 5, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND, false);
            fillWithBlocks(worldIn, piece, 7, 0, 1, 8, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND, false);
            fillWithBlocks(worldIn, piece, 10, 0, 1, 11, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 0, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 6, 0, 0, 6, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 12, 0, 0, 12, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 0, 11, 0, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 8, 11, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 3, 0, 1, 3, 0, 7, &lce::blocks::BlocksInit::FLOWING_WATER, &lce::blocks::BlocksInit::FLOWING_WATER, false);
            fillWithBlocks(worldIn, piece, 9, 0, 1, 9, 0, 7, &lce::blocks::BlocksInit::STILL_WATER, &lce::blocks::BlocksInit::FLOWING_WATER, false);

            const lce::blocks::Block* cropTypeA = getRandomCropType(piece.additionalData >> 15 & 0xF);
            const lce::blocks::Block* cropTypeB = getRandomCropType(piece.additionalData >> 10 & 0xF);
            const lce::blocks::Block* cropTypeC = getRandomCropType(piece.additionalData >> 5 & 0xF);
            const lce::blocks::Block* cropTypeD = getRandomCropType(piece.additionalData & 0xF);
            for (int i = 1; i <= 7; ++i) {
                int j = getMaxAgeFromCrop(cropTypeA->getID());
                int k = j / 3;
                this->setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, structureBB);
                this->setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, structureBB);
                const int l = getMaxAgeFromCrop(cropTypeB->getID());
                const int i1 = l / 3;
                this->setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, structureBB);
                this->setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, structureBB);
                const int j1 = 0; getMaxAgeFromCrop(cropTypeC->getID());
                const int k1 = j1 / 3;
                this->setBlockState(worldIn, cropTypeC->getStateFromMeta(rng.nextInt(k1, j1)), 7, 1, i, structureBB);
                this->setBlockState(worldIn, cropTypeC->getStateFromMeta(rng.nextInt(k1, j1)), 8, 1, i, structureBB);
                const int l1 = 0; getMaxAgeFromCrop(cropTypeD->getID());
                const int i2 = l1 / 3;
                this->setBlockState(worldIn, cropTypeD->getStateFromMeta(rng.nextInt(i2, l1)), 10, 1, i, structureBB);
                this->setBlockState(worldIn, cropTypeD->getStateFromMeta(rng.nextInt(i2, l1)), 11, 1, i, structureBB);
            }

            for (int j2 = 0; j2 < 9; ++j2) {
                for (int k2 = 0; k2 < 13; ++k2) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, k2, 4, j2, piece);
                    this->replaceAirAndLiquidDownwards(worldIn, &lce::blocks::BlocksInit::DIRT, k2, -1, j2, piece);
                }
            }

            return true;
        }
    };

    class MU Field2 final : public Village {
    public:

        Field2() = delete;

        MU explicit Field2(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        MU static const lce::blocks::Block* getRandomCropType(const int data) {
            switch (data) {
                case 0:
                case 1:
                    return &lce::blocks::BlocksInit::CARROTS;
                case 2:
                case 3:
                    return &lce::blocks::BlocksInit::POTATOES;
                case 4:
                    return &lce::blocks::BlocksInit::BEETROOT_BLOCK;
                default:
                    return &lce::blocks::BlocksInit::WHEAT_CROPS;
            }
        }

        MU static int getMaxAgeFromCrop(int id) {
            switch (id) {
                case lce::blocks::ids::CARROTS_ID:
                case lce::blocks::ids::POTATOES_ID:
                case lce::blocks::ids::BEETROOT_BLOCK_ID:
                default:
                    return 4;
                case lce::blocks::ids::WHEAT_CROPS_ID:
                    return 8;
            }
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 4 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* farmLand = &lce::blocks::BlocksInit::FARMLAND;
            const lce::blocks::Block* stillWater = &lce::blocks::BlocksInit::STILL_WATER;
            fillWithBlocks(worldIn, piece, 0, 1, 0, 6, 4, 8, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 1, 0, 1, 2, 0, 7, farmLand, farmLand, false);
            fillWithBlocks(worldIn, piece, 4, 0, 1, 5, 0, 7, farmLand, farmLand, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 0, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 6, 0, 0, 6, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 0, 5, 0, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 8, 5, 0, 8, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 3, 0, 1, 3, 0, 7, stillWater, stillWater, false);

            const lce::blocks::Block* cropTypeA = getRandomCropType(piece.additionalData >> 5 & 0xF);
            const lce::blocks::Block* cropTypeB = getRandomCropType(piece.additionalData & 0xF);
            for (int i = 1; i <= 7; ++i) {
                int j = getMaxAgeFromCrop(cropTypeA->getID());
                int k = j / 3;
                this->setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, structureBB);
                this->setBlockState(worldIn, cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, structureBB);
                int l = getMaxAgeFromCrop(cropTypeB->getID());
                int i1=l / 3;
                this->setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, structureBB);
                this->setBlockState(worldIn, cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, structureBB);
            }

            for (int j1=0; j1 < 9; ++j1) {
                for (int k1=0; k1 < 7; ++k1) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, k1, 4, j1, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, &lce::blocks::BlocksInit::DIRT, k1, -1, j1, structureBB);
                }
            }

            return true;
        }
    };

    class MU Hall final : public Village {
    public:

        Hall() = delete;

        MU explicit Hall(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 7 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::SOUTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 1, 1, 1, 7, 4, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 2, 1, 6, 8, 4, 10, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 2, 0, 6, 8, 0, 10, &lce::blocks::BlocksInit::DIRT, &lce::blocks::BlocksInit::DIRT, false);
            this->setBlockState(worldIn, iBlockState, 6, 0, 6, structureBB);
            fillWithBlocks(worldIn, piece, 2, 1, 6, 2, 1, 10, iBlockState6, iBlockState6, false);
            fillWithBlocks(worldIn, piece, 8, 1, 6, 8, 1, 10, iBlockState6, iBlockState6, false);
            fillWithBlocks(worldIn, piece, 3, 1, 10, 7, 1, 10, iBlockState6, iBlockState6, false);
            fillWithBlocks(worldIn, piece, 1, 0, 1, 7, 0, 4, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 0, 3, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 8, 0, 0, 8, 3, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 0, 7, 1, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 5, 7, 1, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 2, 0, 7, 3, 0, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 1, 2, 5, 7, 3, 5, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 0, 4, 1, 8, 4, 1, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 0, 4, 4, 8, 4, 4, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 0, 5, 2, 8, 5, 3, iBlockState4, iBlockState4, false);
            this->setBlockState(worldIn, iBlockState4, 0, 4, 2, structureBB);
            this->setBlockState(worldIn, iBlockState4, 0, 4, 3, structureBB);
            this->setBlockState(worldIn, iBlockState4, 8, 4, 2, structureBB);
            this->setBlockState(worldIn, iBlockState4, 8, 4, 3, structureBB);
            const lce::blocks::Block* iBlockState7 = iBlockState1;
            const lce::blocks::Block* iBlockState8 = iBlockState2;

            for (int i=-1; i <= 2; ++i) {
                for (int j=0; j <= 8; ++j) {
                    this->setBlockState(worldIn, iBlockState7, j, 4 + i, i, structureBB);
                    this->setBlockState(worldIn, iBlockState8, j, 4 + i, 5 - i, structureBB);
                }
            }

            this->setBlockState(worldIn, iBlockState5, 0, 2, 1, structureBB);
            this->setBlockState(worldIn, iBlockState5, 0, 2, 4, structureBB);
            this->setBlockState(worldIn, iBlockState5, 8, 2, 1, structureBB);
            this->setBlockState(worldIn, iBlockState5, 8, 2, 4, structureBB);
            const lce::blocks::Block* glassPane = &lce::blocks::BlocksInit::GLASS_PANE;
            this->setBlockState(worldIn, glassPane, 0, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 2, 3, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 3, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 2, 5, structureBB);
            this->setBlockState(worldIn, glassPane, 3, 2, 5, structureBB);
            this->setBlockState(worldIn, glassPane, 5, 2, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 6, 2, 5, structureBB);
            this->setBlockState(worldIn, iBlockState6, 2, 1, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::WOODEN_PRESSURE_PLATE, 2, 2, 3, structureBB);
            this->setBlockState(worldIn, iBlockState4, 1, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState7, 2, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState3, 1, 1, 3, structureBB);
            fillWithBlocks(worldIn, piece, 5, 0, 1, 7, 0, 3, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, false);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, 6, 1, 1, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, 6, 1, 2, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 1, 0, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 2, 0, structureBB);
            this->func_189926_a(worldIn, FACING::NORTH, 2, 3, 1, structureBB);
            this->func_189927_a(worldIn, structureBB, rng, 2, 1, 0, FACING::NORTH);

            if (lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID()) &&
                !lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID())) {
                this->setBlockState(worldIn, iBlockState7, 2, 0, -1, structureBB);

                if (this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                    this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, 2, -1, -1, structureBB);
                }
            }

            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 6, 1, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 6, 2, 5, structureBB);
            this->func_189926_a(worldIn, FACING::SOUTH, 6, 3, 4, structureBB);
            this->func_189927_a(worldIn, structureBB, rng, 6, 1, 5, FACING::SOUTH);

            for (int k=0; k < 5; ++k) {
                for (int l=0; l < 9; ++l) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, l, 7, k, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, l, -1, k, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 4, 1, 2, 2);
            return true;
        }
    };

    class MU House1 final : public Village {
    public:

        House1() = delete;

        MU explicit House1(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 9 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::SOUTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::EAST));
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 1, 1, 1, 7, 5, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 8, 0, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 5, 0, 8, 5, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 6, 1, 8, 6, 4, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 7, 2, 8, 7, 3, iBlockState, iBlockState, false);

            for (int i=-1; i <= 2; ++i) {
                for (int j=0; j <= 8; ++j) {
                    this->setBlockState(worldIn, iBlockState1, j, 6 + i, i, structureBB);
                    this->setBlockState(worldIn, iBlockState2, j, 6 + i, 5 - i, structureBB);
                }
            }

            fillWithBlocks(worldIn, piece, 0, 1, 0, 0, 1, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 1, 5, 8, 1, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 8, 1, 0, 8, 1, 4, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 2, 1, 0, 7, 1, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 2, 0, 0, 4, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 2, 5, 0, 4, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 8, 2, 5, 8, 4, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 8, 2, 0, 8, 4, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 2, 1, 0, 4, 4, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 1, 2, 5, 7, 4, 5, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 8, 2, 1, 8, 4, 4, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 1, 2, 0, 7, 4, 0, iBlockState4, iBlockState4, false);
            const lce::blocks::Block* glassPane = &lce::blocks::BlocksInit::GLASS_PANE;
            this->setBlockState(worldIn, glassPane, 4, 2, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 5, 2, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 6, 2, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 3, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 5, 3, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 6, 3, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 2, 3, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 3, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 3, 3, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 3, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 3, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 3, 3, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 2, 5, structureBB);
            this->setBlockState(worldIn, glassPane, 3, 2, 5, structureBB);
            this->setBlockState(worldIn, glassPane, 5, 2, 5, structureBB);
            this->setBlockState(worldIn, glassPane, 6, 2, 5, structureBB);
            fillWithBlocks(worldIn, piece, 1, 4, 1, 7, 4, 1, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 1, 4, 4, 7, 4, 4, iBlockState4, iBlockState4, false);
            fillWithBlocks(worldIn, piece, 1, 3, 4, 7, 3, 4, &lce::blocks::BlocksInit::BOOKSHELF, &lce::blocks::BlocksInit::BOOKSHELF, false);
            this->setBlockState(worldIn, iBlockState4, 7, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState3, 7, 1, 3, structureBB);
            this->setBlockState(worldIn, iBlockState1, 6, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState1, 5, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState1, 4, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState1, 3, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState6, 6, 1, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::WOODEN_PRESSURE_PLATE, 6, 2, 3, structureBB);
            this->setBlockState(worldIn, iBlockState6, 4, 1, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::WOODEN_PRESSURE_PLATE, 4, 2, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::CRAFTING_TABLE, 7, 1, 1, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 1, 1, 0, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 1, 2, 0, structureBB);
            this->func_189927_a(worldIn, structureBB, rng, 1, 1, 0, FACING::NORTH);

            if (lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 1, 0, -1, structureBB)->getID()) &&
                !lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 1, -1, -1, structureBB)->getID())) {
                this->setBlockState(worldIn, iBlockState5, 1, 0, -1, structureBB);

                if (this->getBlockStateFromPos(worldIn, 1, -1, -1, structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                    this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, 1, -1, -1, structureBB);
                }
            }

            for (int l=0; l < 6; ++l) {
                for (int k=0; k < 9; ++k) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, k, 9, l, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, k, -1, l, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 2, 1, 2, 1);
            return true;
        }
    };

    class MU House2 final : public Village {
    public:

        House2() = delete;

        MU explicit House2(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 6 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 0, 1, 0, 9, 4, 6, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 9, 0, 6, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 4, 0, 9, 4, 6, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 5, 0, 9, 5, 6, &lce::blocks::BlocksInit::STONE_SLAB, &lce::blocks::BlocksInit::STONE_SLAB, false);
            fillWithBlocks(worldIn, piece, 1, 5, 1, 8, 5, 5, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 1, 1, 0, 2, 3, 0, iBlockState3, iBlockState3, false);
            fillWithBlocks(worldIn, piece, 0, 1, 0, 0, 4, 0, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 3, 1, 0, 3, 4, 0, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 0, 1, 6, 0, 4, 6, iBlockState5, iBlockState5, false);
            this->setBlockState(worldIn, iBlockState3, 3, 3, 1, structureBB);
            fillWithBlocks(worldIn, piece, 3, 1, 2, 3, 3, 2, iBlockState3, iBlockState3, false);
            fillWithBlocks(worldIn, piece, 4, 1, 3, 5, 3, 3, iBlockState3, iBlockState3, false);
            fillWithBlocks(worldIn, piece, 0, 1, 1, 0, 3, 5, iBlockState3, iBlockState3, false);
            fillWithBlocks(worldIn, piece, 1, 1, 6, 5, 3, 6, iBlockState3, iBlockState3, false);
            fillWithBlocks(worldIn, piece, 5, 1, 0, 5, 3, 0, iBlockState6, iBlockState6, false);
            fillWithBlocks(worldIn, piece, 9, 1, 0, 9, 3, 0, iBlockState6, iBlockState6, false);
            fillWithBlocks(worldIn, piece, 6, 1, 4, 9, 4, 6, iBlockState, iBlockState, false);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::FLOWING_LAVA, 7, 1, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::FLOWING_LAVA, 8, 1, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, 9, 2, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, 9, 2, 4, structureBB);
            fillWithBlocks(worldIn, piece, 7, 2, 4, 8, 2, 5, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            this->setBlockState(worldIn, iBlockState, 6, 1, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::FURNACE, 6, 2, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::FURNACE, 6, 3, 3, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, 8, 1, 1, structureBB);
            const lce::blocks::Block* glassPane = &lce::blocks::BlocksInit::GLASS_PANE;
            this->setBlockState(worldIn, glassPane, 0, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 2, 4, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 2, 6, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 2, 6, structureBB);
            this->setBlockState(worldIn, iBlockState6, 2, 1, 4, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::WOODEN_PRESSURE_PLATE, 2, 2, 4, structureBB);
            this->setBlockState(worldIn, iBlockState3, 1, 1, 5, structureBB);
            this->setBlockState(worldIn, iBlockState1, 2, 1, 5, structureBB);
            this->setBlockState(worldIn, iBlockState2, 1, 1, 4, structureBB);

            /*
            if (!this->hasMadeChest && structureBB.isVecInside(new BlockPos(this->getXWithOffset(5, 5), this->getYWithOffset(1), this->getZWithOffset(5, 5)))) {
                this->hasMadeChest = true;
                this->generateChest(worldIn, structureBB, rng, 5, 1, 5, LootTableList.CHESTS_VILLAGE_BLACKSMITH);
            }
             */

            for (int i=6; i <= 8; ++i) {
                if (lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, i, 0, -1, structureBB)->getID()) &&
                    !lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, i, -1, -1, structureBB)->getID())) {
                    this->setBlockState(worldIn, iBlockState4, i, 0, -1, structureBB);

                    if (this->getBlockStateFromPos(worldIn, i, -1, -1, structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                        this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, i, -1, -1, structureBB);
                    }
                }
            }

            for (int k=0; k < 7; ++k) {
                for (int j=0; j < 10; ++j) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, j, 6, k, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, j, -1, k, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 7, 1, 1, 1);
            return true;
        }
    };

    class MU House3 final : public Village {
    public:

        House3() = delete;

        MU explicit House3(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 7 - 1, 0);
            }

            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::SOUTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::EAST));
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            fillWithBlocks(worldIn, piece, 1, 1, 1, 7, 4, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 2, 1, 6, 8, 4, 10, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 2, 0, 5, 8, 0, 10, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 1, 0, 1, 7, 0, 4, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 0, 3, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 8, 0, 0, 8, 3, 10, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 0, 7, 2, 0, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 0, 5, 2, 1, 5, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 2, 0, 6, 2, 3, 10, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 3, 0, 10, 7, 3, 10, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 1, 2, 0, 7, 3, 0, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 1, 2, 5, 2, 3, 5, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 0, 4, 1, 8, 4, 1, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 0, 4, 4, 3, 4, 4, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 0, 5, 2, 8, 5, 3, iBlockState5, iBlockState5, false);
            this->setBlockState(worldIn, iBlockState5, 0, 4, 2, structureBB);
            this->setBlockState(worldIn, iBlockState5, 0, 4, 3, structureBB);
            this->setBlockState(worldIn, iBlockState5, 8, 4, 2, structureBB);
            this->setBlockState(worldIn, iBlockState5, 8, 4, 3, structureBB);
            this->setBlockState(worldIn, iBlockState5, 8, 4, 4, structureBB);
            const lce::blocks::Block* iBlockState7 = iBlockState1;
            const lce::blocks::Block* iBlockState8 = iBlockState2;
            const lce::blocks::Block* iBlockState9 = iBlockState4;
            const lce::blocks::Block* iBlockState10 = iBlockState3;

            for (int i=-1; i <= 2; ++i) {
                for (int j=0; j <= 8; ++j) {
                    this->setBlockState(worldIn, iBlockState7, j, 4 + i, i, structureBB);

                    if ((i > -1 || j <= 1) && (i > 0 || j <= 3) && (i > 1 || j <= 4 || j >= 6)) {
                        this->setBlockState(worldIn, iBlockState8, j, 4 + i, 5 - i, structureBB);
                    }
                }
            }

            fillWithBlocks(worldIn, piece, 3, 4, 5, 3, 4, 10, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 7, 4, 2, 7, 4, 10, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 4, 5, 4, 4, 5, 10, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 6, 5, 4, 6, 5, 10, iBlockState5, iBlockState5, false);
            fillWithBlocks(worldIn, piece, 5, 6, 3, 5, 6, 10, iBlockState5, iBlockState5, false);

            for (int k=4; k >= 1; --k) {
                this->setBlockState(worldIn, iBlockState5, k, 2 + k, 7 - k, structureBB);

                for (int k1=8 - k; k1 <= 10; ++k1) {
                    this->setBlockState(worldIn, iBlockState10, k, 2 + k, k1, structureBB);
                }
            }

            this->setBlockState(worldIn, iBlockState5, 6, 6, 3, structureBB);
            this->setBlockState(worldIn, iBlockState5, 7, 5, 4, structureBB);
            this->setBlockState(worldIn, iBlockState4, 6, 6, 4, structureBB);

            for (int l=6; l <= 8; ++l) {
                for (int l1=5; l1 <= 10; ++l1) {
                    this->setBlockState(worldIn, iBlockState9, l, 12 - l, l1, structureBB);
                }
            }

            const lce::blocks::Block* glassPane = &lce::blocks::BlocksInit::GLASS_PANE;
            this->setBlockState(worldIn, iBlockState6, 0, 2, 1, structureBB);
            this->setBlockState(worldIn, iBlockState6, 0, 2, 4, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 0, 2, 3, structureBB);
            this->setBlockState(worldIn, iBlockState6, 4, 2, 0, structureBB);
            this->setBlockState(worldIn, glassPane, 5, 2, 0, structureBB);
            this->setBlockState(worldIn, iBlockState6, 6, 2, 0, structureBB);
            this->setBlockState(worldIn, iBlockState6, 8, 2, 1, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 3, structureBB);
            this->setBlockState(worldIn, iBlockState6, 8, 2, 4, structureBB);
            this->setBlockState(worldIn, iBlockState5, 8, 2, 5, structureBB);
            this->setBlockState(worldIn, iBlockState6, 8, 2, 6, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 7, structureBB);
            this->setBlockState(worldIn, glassPane, 8, 2, 8, structureBB);
            this->setBlockState(worldIn, iBlockState6, 8, 2, 9, structureBB);
            this->setBlockState(worldIn, iBlockState6, 2, 2, 6, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 2, 7, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 2, 8, structureBB);
            this->setBlockState(worldIn, iBlockState6, 2, 2, 9, structureBB);
            this->setBlockState(worldIn, iBlockState6, 4, 4, 10, structureBB);
            this->setBlockState(worldIn, glassPane, 5, 4, 10, structureBB);
            this->setBlockState(worldIn, iBlockState6, 6, 4, 10, structureBB);
            this->setBlockState(worldIn, iBlockState5, 5, 5, 10, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 1, 0, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 2, 0, structureBB);
            this->func_189926_a(worldIn, FACING::NORTH, 2, 3, 1, structureBB);
            this->func_189927_a(worldIn, structureBB, rng, 2, 1, 0, FACING::NORTH);
            fillWithBlocks(worldIn, piece, 1, 0, -1, 3, 2, -1, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);

            if (lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID()) &&
                !lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID())) {
                this->setBlockState(worldIn, iBlockState7, 2, 0, -1, structureBB);

                if (this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                    this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, 2, -1, -1, structureBB);
                }
            }

            for (int i1=0; i1 < 5; ++i1) {
                for (int i2=0; i2 < 9; ++i2) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, i2, 7, i1, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, i2, -1, i1, structureBB);
                }
            }

            for (int j1=5; j1 < 11; ++j1) {
                for (int j2=2; j2 < 9; ++j2) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, j2, 7, j1, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, j2, -1, j1, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 4, 1, 2, 2);
            return true;
        }
    };

    class MU House4Garden final : public Village {
    public:
        House4Garden() = delete;

        MU explicit House4Garden(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 6 - 1, 0);
            }

            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 4, 0, 4, iBlockState, iBlockState, false);
            fillWithBlocks(worldIn, piece, 0, 4, 0, 4, 4, 4, iBlockState3, iBlockState3, false);
            fillWithBlocks(worldIn, piece, 1, 4, 1, 3, 4, 3, iBlockState1, iBlockState1, false);
            this->setBlockState(worldIn, iBlockState, 0, 1, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 0, 2, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 0, 3, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 1, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 2, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 3, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 0, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState, 0, 2, 4, structureBB);
            this->setBlockState(worldIn, iBlockState, 0, 3, 4, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 1, 4, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 2, 4, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 3, 4, structureBB);
            fillWithBlocks(worldIn, piece, 0, 1, 1, 0, 3, 3, iBlockState1, iBlockState1, false);
            fillWithBlocks(worldIn, piece, 4, 1, 1, 4, 3, 3, iBlockState1, iBlockState1, false);
            fillWithBlocks(worldIn, piece, 1, 1, 4, 3, 3, 4, iBlockState1, iBlockState1, false);
            const lce::blocks::Block* glassPane = &lce::blocks::BlocksInit::GLASS_PANE;
            this->setBlockState(worldIn, glassPane, 0, 2, 2, structureBB);
            this->setBlockState(worldIn, glassPane, 2, 2, 4, structureBB);
            this->setBlockState(worldIn, glassPane, 4, 2, 2, structureBB);
            this->setBlockState(worldIn, iBlockState1, 1, 1, 0, structureBB);
            this->setBlockState(worldIn, iBlockState1, 1, 2, 0, structureBB);
            this->setBlockState(worldIn, iBlockState1, 1, 3, 0, structureBB);
            this->setBlockState(worldIn, iBlockState1, 2, 3, 0, structureBB);
            this->setBlockState(worldIn, iBlockState1, 3, 3, 0, structureBB);
            this->setBlockState(worldIn, iBlockState1, 3, 2, 0, structureBB);
            this->setBlockState(worldIn, iBlockState1, 3, 1, 0, structureBB);
            
            if (this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID &&
                this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID) { // material air
                this->setBlockState(worldIn, iBlockState2, 2, 0, -1, structureBB);

                if (this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID() == lce::blocks::ids::GRASS_PATH_ID) {
                    this->setBlockState(worldIn, lce::blocks::BlocksInit::GRASS, 2, -1, -1, structureBB);
                }
            }

            fillWithBlocks(worldIn, piece, 1, 1, 1, 3, 3, 3, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);

            if (this->piece.additionalData & 1) /* isRoofAccessible */ {
                this->setBlockState(worldIn, iBlockState4, 0, 5, 0, structureBB);
                this->setBlockState(worldIn, iBlockState4, 1, 5, 0, structureBB);
                this->setBlockState(worldIn, iBlockState4, 2, 5, 0, structureBB);
                this->setBlockState(worldIn, iBlockState4, 3, 5, 0, structureBB);
                this->setBlockState(worldIn, iBlockState4, 4, 5, 0, structureBB);
                this->setBlockState(worldIn, iBlockState4, 0, 5, 4, structureBB);
                this->setBlockState(worldIn, iBlockState4, 1, 5, 4, structureBB);
                this->setBlockState(worldIn, iBlockState4, 2, 5, 4, structureBB);
                this->setBlockState(worldIn, iBlockState4, 3, 5, 4, structureBB);
                this->setBlockState(worldIn, iBlockState4, 4, 5, 4, structureBB);
                this->setBlockState(worldIn, iBlockState4, 4, 5, 1, structureBB);
                this->setBlockState(worldIn, iBlockState4, 4, 5, 2, structureBB);
                this->setBlockState(worldIn, iBlockState4, 4, 5, 3, structureBB);
                this->setBlockState(worldIn, iBlockState4, 0, 5, 1, structureBB);
                this->setBlockState(worldIn, iBlockState4, 0, 5, 2, structureBB);
                this->setBlockState(worldIn, iBlockState4, 0, 5, 3, structureBB);
            }

            if (this->piece.additionalData & 1) /* isRoofAccessible */ {
                const lce::blocks::Block* iBlockState5 = &lce::blocks::BlocksInit::LADDER; // .withProperty(BlockLadder.FACING, FACING::SOUTH);
                this->setBlockState(worldIn, iBlockState5, 3, 1, 3, structureBB);
                this->setBlockState(worldIn, iBlockState5, 3, 2, 3, structureBB);
                this->setBlockState(worldIn, iBlockState5, 3, 3, 3, structureBB);
                this->setBlockState(worldIn, iBlockState5, 3, 4, 3, structureBB);
            }

            this->func_189926_a(worldIn, FACING::NORTH, 2, 3, 1, structureBB);

            for (int j=0; j < 5; ++j) {
                for (int i=0; i < 5; ++i) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, i, 6, j, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, i, -1, j, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 1, 1, 2, 1);
            return true;
        }
    };
    
    
    class MU Path final : public Village {
    public:
        Path() = delete;
        
        MU explicit Path(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }
        
        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            const lce::blocks::Block* iblockstate = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::GRASS_PATH);
            const lce::blocks::Block* iblockstate1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iblockstate2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::GRAVEL);
            const lce::blocks::Block* iblockstate3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);

            for (int i = this->piece.minX; i <= this->piece.maxX; ++i) {
                for (int j = this->piece.minZ; j <= this->piece.maxZ; ++j) {
                    Pos3D blockPos(i, 64, j);

                    if (structureBB.isVecInside(blockPos)) {
                        blockPos =worldIn.getTopSolidOrLiquidBlock(blockPos).down();

                        if (blockPos.getY() < World::getSeaLevel()) {
                            blockPos = {blockPos.getX(), World::getSeaLevel() - 1, blockPos.getZ()};
                        }

                        while (blockPos.getY() >= World::getSeaLevel() - 1) {
                            const lce::blocks::Block* iblockstate4 = worldIn.getBlock(blockPos);

                            if (iblockstate4 == &lce::blocks::BlocksInit::GRASS && worldIn.isAirBlock(blockPos.up())) {
                                worldIn.setBlock(blockPos, iblockstate); // 2
                                break;
                            }

                            if (lce::blocks::ids::isLiquidBlock(iblockstate4->getID())) {
                                worldIn.setBlock(blockPos, iblockstate1); // 2
                                break;
                            }

                            if (iblockstate4 == &lce::blocks::BlocksInit::SAND ||
                                iblockstate4 == &lce::blocks::BlocksInit::SANDSTONE ||
                                iblockstate4 == &lce::blocks::BlocksInit::RED_SANDSTONE) {
                                worldIn.setBlock(blockPos, iblockstate2); // 2
                                worldIn.setBlock(blockPos.down(), iblockstate3); // 2
                                break;
                            }

                            blockPos = blockPos.down();
                        }
                    }
                }
            }

            return true;
        }
    };

    
    class MU Torch final : public Village {
    public:
        Torch() = delete;
        
        MU explicit Torch(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }
        
        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 4 - 1, 0);
            }

            const lce::blocks::Block* oakFence = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 2, 3, 1, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            this->setBlockState(worldIn, oakFence, 1, 0, 0, structureBB);
            this->setBlockState(worldIn, oakFence, 1, 1, 0, structureBB);
            this->setBlockState(worldIn, oakFence, 1, 2, 0, structureBB);
            this->setBlockState(worldIn, lce::blocks::BlocksInit::BLACK_WOOL, 1, 3, 0, structureBB); // Blocks.WOOL.getStateFromMeta(EnumDyeColor.WHITE.getDyeDamage())
            this->func_189926_a(worldIn, FACING::EAST, 2, 3, 0, structureBB);
            this->func_189926_a(worldIn, FACING::NORTH, 1, 3, 1, structureBB);
            this->func_189926_a(worldIn, FACING::WEST, 0, 3, 0, structureBB);
            this->func_189926_a(worldIn, FACING::SOUTH, 1, 3, -1, structureBB);
            return true;
        }
    };





    class MU WoodHut final : public Village {
    public:
        WoodHut() = delete;

        MU explicit WoodHut(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 6 - 1, 0);
            }

            const lce::blocks::Block* iblockstate = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iblockstate1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iblockstate2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, EnumFacing.NORTH));
            const lce::blocks::Block* iblockstate3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iblockstate4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 1, 1, 1, 3, 5, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR, false);
            fillWithBlocks(worldIn, piece, 0, 0, 0, 3, 0, 4, iblockstate, iblockstate, false);
            fillWithBlocks(worldIn, piece, 1, 0, 1, 2, 0, 3, &lce::blocks::BlocksInit::DIRT, &lce::blocks::BlocksInit::DIRT, false);

            if (this->piece.additionalData >> 8 & 1) { // isTallHouse
                fillWithBlocks(worldIn, piece, 1, 4, 1, 2, 4, 3, iblockstate3, iblockstate3, false);
            } else {
                fillWithBlocks(worldIn, piece, 1, 5, 1, 2, 5, 3, iblockstate3, iblockstate3, false);
            }

            this->setBlockState(worldIn, iblockstate3, 1, 4, 0,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 2, 4, 0,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 1, 4, 4,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 2, 4, 4,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 0, 4, 1,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 0, 4, 2,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 0, 4, 3,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 3, 4, 1,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 3, 4, 2,  structureBB);
            this->setBlockState(worldIn, iblockstate3, 3, 4, 3,  structureBB);
            fillWithBlocks(worldIn, piece, 0, 1, 0, 0, 3, 0, iblockstate3, iblockstate3, false);
            fillWithBlocks(worldIn, piece, 3, 1, 0, 3, 3, 0, iblockstate3, iblockstate3, false);
            fillWithBlocks(worldIn, piece, 0, 1, 4, 0, 3, 4, iblockstate3, iblockstate3, false);
            fillWithBlocks(worldIn, piece, 3, 1, 4, 3, 3, 4, iblockstate3, iblockstate3, false);
            fillWithBlocks(worldIn, piece, 0, 1, 1, 0, 3, 3, iblockstate1, iblockstate1, false);
            fillWithBlocks(worldIn, piece, 3, 1, 1, 3, 3, 3, iblockstate1, iblockstate1, false);
            fillWithBlocks(worldIn, piece, 1, 1, 0, 2, 3, 0, iblockstate1, iblockstate1, false);
            fillWithBlocks(worldIn, piece, 1, 1, 4, 2, 3, 4, iblockstate1, iblockstate1, false);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::GLASS_PANE, 0, 2, 2,  structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::GLASS_PANE, 3, 2, 2,  structureBB);

            int tablePosition = piece.additionalData & 3;
            if (tablePosition > 0) {
                this->setBlockState(worldIn, iblockstate4, tablePosition, 1, 3,  structureBB);
                this->setBlockState(worldIn, &lce::blocks::BlocksInit::WOODEN_PRESSURE_PLATE, tablePosition, 2, 3,  structureBB);
            }

            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 1, 1, 0,  structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 1, 2, 0,  structureBB);
            this->func_189927_a(worldIn,  structureBB, rng, 1, 1, 0, FACING::NORTH);

            if (lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 1, 0, -1,  structureBB)->getID()) &&
                !lce::blocks::ids::isReplaceableBlock(this->getBlockStateFromPos(worldIn, 1, -1, -1,  structureBB)->getID())) {
                this->setBlockState(worldIn, iblockstate2, 1, 0, -1,  structureBB);

                if (this->getBlockStateFromPos(worldIn, 1, -1, -1,  structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                    this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, 1, -1, -1,  structureBB);
                }
            }

            for (int i=0; i < 5; ++i) {
                for (int j=0; j < 4; ++j) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, j, 6, i,  structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iblockstate, j, -1, i,  structureBB);
                }
            }

            // this->spawnVillagers(worldIn,  structureBB, 1, 1, 2, 1);
            return true;
        }
    };
    
    
    class MU Well final : public Village {
    public:
        Well() = delete;

        MU explicit Well(generation::Village* villageIn, const Piece& p, int structureType)
            : Village(villageIn, structureType) {
            piece = p;
        }

        bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->piece.offset(0, this->averageGroundLvl - this->piece.maxY + 3, 0);
            }

            const lce::blocks::Block* iblockstate = this->getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iblockstate1 = this->getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, piece, 1, 0, 1, 4, 12, 4, iblockstate, &lce::blocks::BlocksInit::FLOWING_WATER, false);
            this->setBlockState(worldIn, lce::blocks::BlocksInit::AIR, 2, 12, 2, structureBB);
            this->setBlockState(worldIn, lce::blocks::BlocksInit::AIR, 3, 12, 2, structureBB);
            this->setBlockState(worldIn, lce::blocks::BlocksInit::AIR, 2, 12, 3, structureBB);
            this->setBlockState(worldIn, lce::blocks::BlocksInit::AIR, 3, 12, 3, structureBB);
            this->setBlockState(worldIn, iblockstate1, 1, 13, 1, structureBB);
            this->setBlockState(worldIn, iblockstate1, 1, 14, 1, structureBB);
            this->setBlockState(worldIn, iblockstate1, 4, 13, 1, structureBB);
            this->setBlockState(worldIn, iblockstate1, 4, 14, 1, structureBB);
            this->setBlockState(worldIn, iblockstate1, 1, 13, 4, structureBB);
            this->setBlockState(worldIn, iblockstate1, 1, 14, 4, structureBB);
            this->setBlockState(worldIn, iblockstate1, 4, 13, 4, structureBB);
            this->setBlockState(worldIn, iblockstate1, 4, 14, 4, structureBB);
            fillWithBlocks(worldIn, piece, 1, 15, 1, 4, 15, 4, iblockstate, iblockstate, false);
            
            for (int i=0; i <= 5; ++i) {
                for (int j=0; j <= 5; ++j) {
                    if (j == 0 || j == 5 || i == 0 || i == 5) {
                        this->setBlockState(worldIn, iblockstate, j, 11, i, structureBB);
                        this->clearCurrentPositionBlocksUpwards(worldIn, j, 12, i, structureBB);
                    }
                }
            }

            return true;
        }
    };
    
    


};

