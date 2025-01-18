#include "WorldGenAbstractTree.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/block_states.hpp"
#include "lce/blocks/blocks.hpp"

using namespace lce::blocks;


void WorldGenAbstractTree::setDirtAt(World* worldIn, const Pos3D& pos) {
    if (worldIn->getBlockId(pos) != ids::DIRT_ID) { worldIn->setBlock(pos, ids::DIRT_ID); }
}

bool WorldGenAbstractTree::canGrowInto(const int blockId) {
    switch (blockId) {
        case ids::AIR_ID:
        case ids::OAK_LEAVES_ID:
        case ids::ACACIA_LEAVES_ID:
        case ids::GRASS_ID:
        case ids::DIRT_ID:
        case ids::OAK_WOOD_ID:
        case ids::ACACIA_WOOD_ID:
        case ids::OAK_SAPLING_ID:
        case ids::VINES_ID:
            return true;
        default:
            return false;
    }
}

void WorldGenAbstractTree::addVine(World* worldIn, const Pos3D& pos, const enumFacing facing) {
    worldIn->setBlock(pos, ids::VINES_ID,
        states::Vine::withProperty(facing));
}

void WorldGenAbstractTree::placeFallenTrunk(World* worldIn, RNG& rand, const Pos3D& pos, const int height,
                                            const Block* woodType) {
    const enumFacing facing = FACING_HORIZONTAL[rand.nextInt(4)];
    constexpr double vineChance = 0.5;
    const int trunkLength = height - 2;
    const int offset = rand.nextInt(2) + 2;
    const Pos3D blockPos = pos.offset(facing, offset);
    //int groundHeightPos = worldIn->getHeight(pos.add(blockPos.getX(), 0, blockPos.getZ()));

    if (blockPos.getY() <= pos.getY() + 1) {
        if (trunkLength > 0) {
            int airCount = 0;
            Pos3D checkPos = blockPos;

            for (int i = trunkLength; i > 0; --i) {
                checkPos = checkPos.offset(facing);
                const int blockId = worldIn->getBlockId(checkPos);
                if (blockId != ids::AIR_ID) return;
                const int belowBlockId = worldIn->getBlockId(checkPos.down());
                if (belowBlockId == ids::AIR_ID) {
                    airCount++;
                    if (airCount > 2) return;
                } else {
                    airCount = 0;
                }
            }

            Pos3D placePos = blockPos;
            for (int i = trunkLength; i > 0; --i) {
                placePos = placePos.offset(facing);
                worldIn->setBlock(placePos, woodType);

                if (rand.nextInt(10) == 0) {
                    const int aboveBlockId = worldIn->getBlockId(placePos.up());
                    if (aboveBlockId == ids::AIR_ID) {
                        const double chance = rand.nextFloat();
                        if (chance < vineChance) {
                            if (facing == enumFacing::NORTH || facing == enumFacing::SOUTH) {
                                addVine(worldIn, placePos.east(), enumFacing::WEST);
                            } else {
                                addVine(worldIn, placePos.north(), enumFacing::SOUTH);
                            }
                        } else {
                            if (facing == enumFacing::NORTH || facing == enumFacing::SOUTH) {
                                addVine(worldIn, placePos.west(), enumFacing::EAST);
                            } else {
                                addVine(worldIn, placePos.south(), enumFacing::NORTH);
                            }
                        }
                    }
                }
            }
        }
    }
}

int WorldGenAbstractTree::placeTrunk(World* worldIn, RNG& rand, const Pos3D& pos, int height,
                                     const Block* woodType, c_bool vinesGrow) {
    const int fallenTrunk = rand.nextInt(80);
    double vineGrowthChance = 0.0;

    if (woodType != &BlocksInit::BIRCH_WOOD) {
        if (fallenTrunk == 0) {
            vineGrowthChance = 0.75;
        } else if (vinesGrow) {
            vineGrowthChance = 0.33333334;
        } else if (rand.nextInt(12) == 0) {
            vineGrowthChance = 1.0;
        }
    }

    if (fallenTrunk == 0) {
        placeFallenTrunk(worldIn, rand, pos, height, woodType);
        height = 1;
    }

    if (vineGrowthChance == 0.0) {
        for (int j3 = 0; j3 < height; ++j3) {
            const int blockId = worldIn->getBlockId(pos.up(j3));
            if (ids::isAirOrLeavesBlock(blockId) || blockId == ids::VINES_ID) {
                worldIn->setBlock(pos.up(j3), woodType);
            }
        }
    } else {
        for (int j3 = 0; j3 < height; ++j3) {
            const int blockId = worldIn->getBlockId(pos.up(j3));
            if (ids::isAirOrLeavesBlock(blockId) || blockId == ids::VINES_ID) {
                worldIn->setBlock(pos.up(j3), woodType);

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(-1, j3, 0))) {
                    addVine(worldIn, pos.add(-1, j3, 0), enumFacing::EAST);
                }

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(1, j3, 0))) {
                    addVine(worldIn, pos.add(1, j3, 0), enumFacing::WEST);
                }

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(0, j3, -1))) {
                    addVine(worldIn, pos.add(0, j3, -1), enumFacing::SOUTH);
                }

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(0, j3, 1))) {
                    addVine(worldIn, pos.add(0, j3, 1), enumFacing::NORTH);
                }
            }
        }
    }
    return fallenTrunk == 0 ? 0 : 1;
}
