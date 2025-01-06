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
    public:
        int averageGroundLvl = -1;
        MU int villagersSpawned{};

        generation::Village* villageData{};
        BoundingBox boundingBox;

        Village() = default;

        explicit Village(generation::Village* villageIn) {
            villageData = villageIn;
            boundingBox = villageData->structureBoundingBox;
        }

        static const lce::blocks::Block* getBiomeSpecificBlockState(const lce::blocks::Block* blockPtr) {
            return blockPtr;
            /*
                if (this->structureType == 1) {
                    if (blockstateIn.getBlock() == Blocks.LOG || blockstateIn.getBlock() == Blocks.LOG2) {
                        return Blocks.SANDSTONE.getDefaultState();
                    }

                    if (blockstateIn.getBlock() == Blocks.COBBLESTONE) {
                        return Blocks.SANDSTONE.getStateFromMeta(BlockSandStone.EnumType.DEFAULT.getMetadata());
                    }

                    if (blockstateIn.getBlock() == Blocks.PLANKS) {
                        return Blocks.SANDSTONE.getStateFromMeta(BlockSandStone.EnumType.SMOOTH.getMetadata());
                    }

                    if (blockstateIn.getBlock() == Blocks.OAK_STAIRS) {
                        return Blocks.SANDSTONE_STAIRS.getDefaultState().withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                    }

                    if (blockstateIn.getBlock() == Blocks.STONE_STAIRS) {
                        return Blocks.SANDSTONE_STAIRS.getDefaultState().withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                    }

                    if (blockstateIn.getBlock() == Blocks.GRAVEL) {
                        return Blocks.SANDSTONE.getDefaultState();
                    }
                } else if (this->structureType == 3) {
                    if (blockstateIn.getBlock() == Blocks.LOG || blockstateIn.getBlock() == Blocks.LOG2) {
                        return Blocks.LOG.getDefaultState().withProperty(BlockOldLog.VARIANT, BlockPlanks.EnumType.SPRUCE).withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                    }

                    if (blockstateIn.getBlock() == Blocks.PLANKS) {
                        return Blocks.PLANKS.getDefaultState().withProperty(BlockPlanks.VARIANT, BlockPlanks.EnumType.SPRUCE);
                    }

                    if (blockstateIn.getBlock() == Blocks.OAK_STAIRS) {
                        return Blocks.SPRUCE_STAIRS.getDefaultState().withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                    }

                    if (blockstateIn.getBlock() == Blocks.OAK_FENCE) {
                        return Blocks.SPRUCE_FENCE.getDefaultState();
                    }
                } else if (this->structureType == 2) {
                    if (blockstateIn.getBlock() == Blocks.LOG || blockstateIn.getBlock() == Blocks.LOG2) {
                        return Blocks.LOG2.getDefaultState().withProperty(BlockNewLog.VARIANT, BlockPlanks.EnumType.ACACIA).withProperty(BlockLog.LOG_AXIS, blockstateIn.getValue(BlockLog.LOG_AXIS));
                    }

                    if (blockstateIn.getBlock() == Blocks.PLANKS) {
                        return Blocks.PLANKS.getDefaultState().withProperty(BlockPlanks.VARIANT, BlockPlanks.EnumType.ACACIA);
                    }

                    if (blockstateIn.getBlock() == Blocks.OAK_STAIRS) {
                        return Blocks.ACACIA_STAIRS.getDefaultState().withProperty(BlockStairs.FACING, blockstateIn.getValue(BlockStairs.FACING));
                    }

                    if (blockstateIn.getBlock() == Blocks.COBBLESTONE) {
                        return Blocks.LOG2.getDefaultState().withProperty(BlockNewLog.VARIANT, BlockPlanks.EnumType.ACACIA).withProperty(BlockLog.LOG_AXIS, BlockLog.EnumAxis.Y);
                    }

                    if (blockstateIn.getBlock() == Blocks.OAK_FENCE) {
                        return Blocks.ACACIA_FENCE.getDefaultState();
                    }
                }

                return blockstateIn;
    */
        }

        MU ND int getAverageGroundLevel(World worldIn, BoundingBox structureBB) const {
            int i = 0;
            int j = 0;
            Pos3D blockPos;

            for (int k = boundingBox.minZ; k <= boundingBox.maxZ; ++k) {
                for (int l = boundingBox.minX; l <= boundingBox.maxX; ++l) {
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

    class MU Church : public structure_rolls::Village {
        Church() = default;

        MU Church(MU RNG& rng, BoundingBox boundingBox, FACING facing) {
            // super(start, type);
            this->setCoordBaseMode(facing);
            this->boundingBox = boundingBox;

        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 12 - 1, 0);
            }

            const lce::blocks::Block* iBlockState = &lce::blocks::BlocksInit::COBBLESTONE;
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::EAST));
            fillWithBlocks(worldIn, structureBB, 1, 1, 1, 3, 3, 7, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 1, 5, 1, 3, 9, 3, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 1, 0, 0, 3, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 1, 0, 3, 10, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 1, 1, 0, 10, 3, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 4, 1, 1, 4, 10, 3, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 0, 4, 0, 4, 7, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 4, 0, 4, 4, 4, 7, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 1, 8, 3, 4, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 5, 4, 3, 10, 4, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 5, 5, 3, 5, 7, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 9, 0, 4, 9, 4, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 4, 0, 4, 4, 4, iBlockState, iBlockState);
            this->setBlockState(worldIn, iBlockState, 0, 11, 2, structureBB);
            this->setBlockState(worldIn, iBlockState, 4, 11, 2, structureBB);
            this->setBlockState(worldIn, iBlockState, 2, 11, 0, structureBB);
            this->setBlockState(worldIn, iBlockState, 2, 11, 4, structureBB);
            this->setBlockState(worldIn, iBlockState, 1, 1, 6, structureBB);
            this->setBlockState(worldIn, iBlockState, 1, 1, 7, structureBB);
            this->setBlockState(worldIn, iBlockState, 2, 1, 7, structureBB);
            this->setBlockState(worldIn, iBlockState, 3, 1, 6, structureBB);
            this->setBlockState(worldIn, iBlockState, 3, 1, 7, structureBB);
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

            if (this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID() == lce::blocks::ids::AIR_ID &&
                this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID) { // material air
                this->setBlockState(worldIn, iBlockState1, 2, 0, -1, structureBB);

                if (this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB) == &lce::blocks::BlocksInit::GRASS_PATH) {
                    this->setBlockState(worldIn, &lce::blocks::BlocksInit::GRASS, 2, -1, -1, structureBB);
                }
            }

            for (int k=0; k < 9; ++k) {
                for (int j=0; j < 5; ++j) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, j, 12, k, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, iBlockState, j, -1, k, structureBB);
                }
            }

            // this->spawnVillagers(worldIn, structureBB, 2, 1, 2, 1);
            return true;
        }

    };

    class MU Field1 : public structure_rolls::Village {
        const lce::blocks::Block* cropTypeA{};
        const lce::blocks::Block* cropTypeB{};
        const lce::blocks::Block* cropTypeC{};
        const lce::blocks::Block* cropTypeD{};

        Field1() = default;

        MU Field1(RNG& rng, BoundingBox boundingBox, FACING facing) {
            this->setCoordBaseMode(facing);
            this->boundingBox = boundingBox;
            this->cropTypeA = getRandomCropType(rng);
            this->cropTypeB = getRandomCropType(rng);
            this->cropTypeC = getRandomCropType(rng);
            this->cropTypeD = getRandomCropType(rng);
        }

        MU static const lce::blocks::Block* getRandomCropType(RNG rng) {
            switch (rng.nextInt(10)) {
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

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 4 - 1, 0);
            }

            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            fillWithBlocks(worldIn, structureBB, 0, 1, 0, 12, 4, 8, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 1, 0, 1, 2, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND);
            fillWithBlocks(worldIn, structureBB, 4, 0, 1, 5, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND);
            fillWithBlocks(worldIn, structureBB, 7, 0, 1, 8, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND);
            fillWithBlocks(worldIn, structureBB, 10, 0, 1, 11, 0, 7, &lce::blocks::BlocksInit::FARMLAND, &lce::blocks::BlocksInit::FARMLAND);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 0, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 6, 0, 0, 6, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 12, 0, 0, 12, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 0, 11, 0, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 8, 11, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 3, 0, 1, 3, 0, 7, &lce::blocks::BlocksInit::FLOWING_WATER, &lce::blocks::BlocksInit::FLOWING_WATER);
            fillWithBlocks(worldIn, structureBB, 9, 0, 1, 9, 0, 7, &lce::blocks::BlocksInit::STILL_WATER, &lce::blocks::BlocksInit::FLOWING_WATER);

            for (int i = 1; i <= 7; ++i) {
                int j = 0; // ((BlockCrops) this->cropTypeA).getMaxAge();
                int k = j / 3;
                this->setBlockState(worldIn, this->cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, structureBB);
                this->setBlockState(worldIn, this->cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, structureBB);
                int l = 0; // ((BlockCrops) this->cropTypeB).getMaxAge();
                int i1 = l / 3;
                this->setBlockState(worldIn, this->cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, structureBB);
                this->setBlockState(worldIn, this->cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, structureBB);
                int j1 = 0; // ((BlockCrops) this->cropTypeC).getMaxAge();
                int k1 = j1 / 3;
                this->setBlockState(worldIn, this->cropTypeC->getStateFromMeta(rng.nextInt(k1, j1)), 7, 1, i, structureBB);
                this->setBlockState(worldIn, this->cropTypeC->getStateFromMeta(rng.nextInt(k1, j1)), 8, 1, i, structureBB);
                int l1 = 0; // ((BlockCrops) this->cropTypeD).getMaxAge();
                int i2 = l1 / 3;
                this->setBlockState(worldIn, this->cropTypeD->getStateFromMeta(rng.nextInt(i2, l1)), 10, 1, i, structureBB);
                this->setBlockState(worldIn, this->cropTypeD->getStateFromMeta(rng.nextInt(i2, l1)), 11, 1, i, structureBB);
            }

            for (int j2 = 0; j2 < 9; ++j2) {
                for (int k2 = 0; k2 < 13; ++k2) {
                    this->clearCurrentPositionBlocksUpwards(worldIn, k2, 4, j2, structureBB);
                    this->replaceAirAndLiquidDownwards(worldIn, &lce::blocks::BlocksInit::DIRT, k2, -1, j2, structureBB);
                }
            }

            return true;
        }
    };

    class MU Field2 : public structure_rolls::Village {
        const lce::blocks::Block* cropTypeA{};
        const lce::blocks::Block* cropTypeB{};

        Field2() = default;

        MU Field2(MU int p_i45569_2_, RNG& rand, BoundingBox p_i45569_4_, FACING facing) {
            // super(start, p_i45569_2_);
            this->setCoordBaseMode(facing);
            this->boundingBox=p_i45569_4_;
            this->cropTypeA = getRandomCropType(rand);
            this->cropTypeB = getRandomCropType(rand);
        }

        MU static const lce::blocks::Block* getRandomCropType(RNG rng) {
            switch (rng.nextInt(10)) {
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

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 4 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* farmLand = &lce::blocks::BlocksInit::FARMLAND;
            const lce::blocks::Block* stillWater = &lce::blocks::BlocksInit::STILL_WATER;
            fillWithBlocks(worldIn, structureBB, 0, 1, 0, 6, 4, 8, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 1, 0, 1, 2, 0, 7, farmLand, farmLand);
            fillWithBlocks(worldIn, structureBB, 4, 0, 1, 5, 0, 7, farmLand, farmLand);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 0, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 6, 0, 0, 6, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 0, 5, 0, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 8, 5, 0, 8, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 3, 0, 1, 3, 0, 7, stillWater, stillWater);

            for (int i = 1; i <= 7; ++i) {
                int j = 0; // ((BlockCrops) this->cropTypeA).getMaxAge();
                int k = j / 3;
                this->setBlockState(worldIn, this->cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 1, 1, i, structureBB);
                this->setBlockState(worldIn, this->cropTypeA->getStateFromMeta(rng.nextInt(k, j)), 2, 1, i, structureBB);
                int l = 0; // ((BlockCrops) this->cropTypeB).getMaxAge();
                int i1=l / 3;
                this->setBlockState(worldIn, this->cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 4, 1, i, structureBB);
                this->setBlockState(worldIn, this->cropTypeB->getStateFromMeta(rng.nextInt(i1, l)), 5, 1, i, structureBB);
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

    class MU Hall : public structure_rolls::Village {

        Hall() = default;

        MU Hall(MU int p_i45569_2_, MU RNG& rand, BoundingBox p_i45569_4_, FACING facing) {
            // super(start, p_i45569_2_);
            this->setCoordBaseMode(facing);
            this->boundingBox=p_i45569_4_;
        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 7 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::SOUTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, structureBB, 1, 1, 1, 7, 4, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 2, 1, 6, 8, 4, 10, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 2, 0, 6, 8, 0, 10, &lce::blocks::BlocksInit::DIRT, &lce::blocks::BlocksInit::DIRT);
            this->setBlockState(worldIn, iBlockState, 6, 0, 6, structureBB);
            fillWithBlocks(worldIn, structureBB, 2, 1, 6, 2, 1, 10, iBlockState6, iBlockState6);
            fillWithBlocks(worldIn, structureBB, 8, 1, 6, 8, 1, 10, iBlockState6, iBlockState6);
            fillWithBlocks(worldIn, structureBB, 3, 1, 10, 7, 1, 10, iBlockState6, iBlockState6);
            fillWithBlocks(worldIn, structureBB, 1, 0, 1, 7, 0, 4, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 0, 3, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 8, 0, 0, 8, 3, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 0, 7, 1, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 5, 7, 1, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 2, 0, 7, 3, 0, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 1, 2, 5, 7, 3, 5, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 0, 4, 1, 8, 4, 1, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 0, 4, 4, 8, 4, 4, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 0, 5, 2, 8, 5, 3, iBlockState4, iBlockState4);
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
            fillWithBlocks(worldIn, structureBB, 5, 0, 1, 7, 0, 3, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, 6, 1, 1, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::DOUBLE_STONE_SLAB, 6, 1, 2, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 1, 0, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, 2, 2, 0, structureBB);
            this->func_189926_a(worldIn, FACING::NORTH, 2, 3, 1, structureBB);
            this->func_189927_a(worldIn, structureBB, rng, 2, 1, 0, FACING::NORTH);

            if (this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID() == lce::blocks::ids::AIR_ID &&
                this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID) { // material air
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

    class MU House1 : public structure_rolls::Village {

        House1() = default;

        MU House1(MU int p_i45569_2_, MU RNG& rand, BoundingBox p_i45569_4_, FACING facing) {
            // super(start, p_i45569_2_);
            this->setCoordBaseMode(facing);
            this->boundingBox=p_i45569_4_;
        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 9 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::SOUTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::EAST));
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, structureBB, 1, 1, 1, 7, 5, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 8, 0, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 5, 0, 8, 5, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 6, 1, 8, 6, 4, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 7, 2, 8, 7, 3, iBlockState, iBlockState);

            for (int i=-1; i <= 2; ++i) {
                for (int j=0; j <= 8; ++j) {
                    this->setBlockState(worldIn, iBlockState1, j, 6 + i, i, structureBB);
                    this->setBlockState(worldIn, iBlockState2, j, 6 + i, 5 - i, structureBB);
                }
            }

            fillWithBlocks(worldIn, structureBB, 0, 1, 0, 0, 1, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 1, 5, 8, 1, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 8, 1, 0, 8, 1, 4, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 2, 1, 0, 7, 1, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 2, 0, 0, 4, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 2, 5, 0, 4, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 8, 2, 5, 8, 4, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 8, 2, 0, 8, 4, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 2, 1, 0, 4, 4, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 1, 2, 5, 7, 4, 5, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 8, 2, 1, 8, 4, 4, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 1, 2, 0, 7, 4, 0, iBlockState4, iBlockState4);
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
            fillWithBlocks(worldIn, structureBB, 1, 4, 1, 7, 4, 1, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 1, 4, 4, 7, 4, 4, iBlockState4, iBlockState4);
            fillWithBlocks(worldIn, structureBB, 1, 3, 4, 7, 3, 4, &lce::blocks::BlocksInit::BOOKSHELF, &lce::blocks::BlocksInit::BOOKSHELF);
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

            if (this->getBlockStateFromPos(worldIn, 1, 0, -1, structureBB)->getID() == lce::blocks::ids::AIR_ID &&
                this->getBlockStateFromPos(worldIn, 1, -1, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID) { // material air
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

    class MU House2 : public structure_rolls::Village {

        House2() = default;

        MU House2(MU int p_i45569_2_, MU RNG& rand, BoundingBox p_i45569_4_, FACING facing) {
            // super(start, p_i45569_2_);
            this->setCoordBaseMode(facing);
            this->boundingBox=p_i45569_4_;
        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 6 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = &lce::blocks::BlocksInit::COBBLESTONE;
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, structureBB, 0, 1, 0, 9, 4, 6, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 9, 0, 6, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 4, 0, 9, 4, 6, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 5, 0, 9, 5, 6, &lce::blocks::BlocksInit::STONE_SLAB, &lce::blocks::BlocksInit::STONE_SLAB);
            fillWithBlocks(worldIn, structureBB, 1, 5, 1, 8, 5, 5, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 1, 1, 0, 2, 3, 0, iBlockState3, iBlockState3);
            fillWithBlocks(worldIn, structureBB, 0, 1, 0, 0, 4, 0, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 3, 1, 0, 3, 4, 0, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 0, 1, 6, 0, 4, 6, iBlockState5, iBlockState5);
            this->setBlockState(worldIn, iBlockState3, 3, 3, 1, structureBB);
            fillWithBlocks(worldIn, structureBB, 3, 1, 2, 3, 3, 2, iBlockState3, iBlockState3);
            fillWithBlocks(worldIn, structureBB, 4, 1, 3, 5, 3, 3, iBlockState3, iBlockState3);
            fillWithBlocks(worldIn, structureBB, 0, 1, 1, 0, 3, 5, iBlockState3, iBlockState3);
            fillWithBlocks(worldIn, structureBB, 1, 1, 6, 5, 3, 6, iBlockState3, iBlockState3);
            fillWithBlocks(worldIn, structureBB, 5, 1, 0, 5, 3, 0, iBlockState6, iBlockState6);
            fillWithBlocks(worldIn, structureBB, 9, 1, 0, 9, 3, 0, iBlockState6, iBlockState6);
            fillWithBlocks(worldIn, structureBB, 6, 1, 4, 9, 4, 6, iBlockState, iBlockState);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::FLOWING_LAVA, 7, 1, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::FLOWING_LAVA, 8, 1, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, 9, 2, 5, structureBB);
            this->setBlockState(worldIn, &lce::blocks::BlocksInit::IRON_BARS, 9, 2, 4, structureBB);
            fillWithBlocks(worldIn, structureBB, 7, 2, 4, 8, 2, 5, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
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
                if (this->getBlockStateFromPos(worldIn, i, 0, -1, structureBB)->getID() == lce::blocks::ids::AIR_ID &&
                    this->getBlockStateFromPos(worldIn, i, -1, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID) { // material air
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

    class MU House3 : public structure_rolls::Village {

        House3() = default;

        MU House3(MU int p_i45569_2_, MU RNG& rand, BoundingBox p_i45569_4_, FACING facing) {
            // super(start, p_i45569_2_);
            this->setCoordBaseMode(facing);
            this->boundingBox=p_i45569_4_;
        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 7 - 1, 0);
            }

            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::SOUTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::EAST));
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_STAIRS); // .withProperty(BlockStairs.FACING, FACING::WEST));
            const lce::blocks::Block* iBlockState5 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState6 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            fillWithBlocks(worldIn, structureBB, 1, 1, 1, 7, 4, 4, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 2, 1, 6, 8, 4, 10, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);
            fillWithBlocks(worldIn, structureBB, 2, 0, 5, 8, 0, 10, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 1, 0, 1, 7, 0, 4, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 0, 3, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 8, 0, 0, 8, 3, 10, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 0, 7, 2, 0, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 0, 5, 2, 1, 5, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 2, 0, 6, 2, 3, 10, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 3, 0, 10, 7, 3, 10, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 1, 2, 0, 7, 3, 0, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 1, 2, 5, 2, 3, 5, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 0, 4, 1, 8, 4, 1, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 0, 4, 4, 3, 4, 4, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 0, 5, 2, 8, 5, 3, iBlockState5, iBlockState5);
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

            fillWithBlocks(worldIn, structureBB, 3, 4, 5, 3, 4, 10, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 7, 4, 2, 7, 4, 10, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 4, 5, 4, 4, 5, 10, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 6, 5, 4, 6, 5, 10, iBlockState5, iBlockState5);
            fillWithBlocks(worldIn, structureBB, 5, 6, 3, 5, 6, 10, iBlockState5, iBlockState5);

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
            fillWithBlocks(worldIn, structureBB, 1, 0, -1, 3, 2, -1, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);

            if (this->getBlockStateFromPos(worldIn, 2, 0, -1, structureBB)->getID() == lce::blocks::ids::AIR_ID &&
                this->getBlockStateFromPos(worldIn, 2, -1, -1, structureBB)->getID() != lce::blocks::ids::AIR_ID) { // material air
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

    class MU House4Garden : public structure_rolls::Village {
        bool isRoofAccessible{};

        House4Garden() = default;

        MU House4Garden(MU int p_i45569_2_, MU RNG& rand, BoundingBox p_i45569_4_, FACING facing) {
            // super(start, p_i45569_2_);
            this->setCoordBaseMode(facing);
            this->boundingBox=p_i45569_4_;
        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) override {
            if (this->averageGroundLvl < 0) {
                this->averageGroundLvl = getAverageGroundLevel(worldIn, structureBB);
                if (this->averageGroundLvl < 0) { return true; }
                this->boundingBox.offset(0, this->averageGroundLvl - this->boundingBox.maxY + 6 - 1, 0);
            }
            const lce::blocks::Block* iBlockState = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE);
            const lce::blocks::Block* iBlockState1 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD_PLANK);
            const lce::blocks::Block* iBlockState2 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::COBBLESTONE_STAIRS); // .withProperty(BlockStairs.FACING, FACING::NORTH));
            const lce::blocks::Block* iBlockState3 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_WOOD);
            const lce::blocks::Block* iBlockState4 = getBiomeSpecificBlockState(&lce::blocks::BlocksInit::OAK_FENCE);
            fillWithBlocks(worldIn, structureBB, 0, 0, 0, 4, 0, 4, iBlockState, iBlockState);
            fillWithBlocks(worldIn, structureBB, 0, 4, 0, 4, 4, 4, iBlockState3, iBlockState3);
            fillWithBlocks(worldIn, structureBB, 1, 4, 1, 3, 4, 3, iBlockState1, iBlockState1);
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
            fillWithBlocks(worldIn, structureBB, 0, 1, 1, 0, 3, 3, iBlockState1, iBlockState1);
            fillWithBlocks(worldIn, structureBB, 4, 1, 1, 4, 3, 3, iBlockState1, iBlockState1);
            fillWithBlocks(worldIn, structureBB, 1, 1, 4, 3, 3, 4, iBlockState1, iBlockState1);
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

            fillWithBlocks(worldIn, structureBB, 1, 1, 1, 3, 3, 3, &lce::blocks::BlocksInit::AIR, &lce::blocks::BlocksInit::AIR);

            if (this->isRoofAccessible) {
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

            if (this->isRoofAccessible) {
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


};

