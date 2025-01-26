#include "WorldGenAbstractTree.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


void WorldGenAbstractTree::setDirtAt(World* worldIn, const Pos3D& pos) {
    if (worldIn->getBlockId(pos) != DIRT_ID) { worldIn->setBlockId(pos, DIRT_ID); }
}

bool WorldGenAbstractTree::canGrowInto(const int blockId) {
    switch (blockId) {
        case AIR_ID:
        case OAK_LEAVES_ID:
        case ACACIA_LEAVES_ID:
        case GRASS_ID:
        case DIRT_ID:
        case OAK_WOOD_ID:
        case ACACIA_WOOD_ID:
        case OAK_SAPLING_ID:
        case VINES_ID:
            return true;
        default:
            return false;
    }
}

void WorldGenAbstractTree::addVine(World* worldIn, const Pos3D& pos, const EnumFacing facing) {
    worldIn->setBlock(pos, VINES_ID,
        states::Vine::withProperty(facing));
}

void WorldGenAbstractTree::placeFallenTrunk(World* worldIn, RNG& rand, const Pos3D& pos, const int height,
                                            const lce::Block* woodType) {
    const EnumFacing facing = FACING_HORIZONTAL[rand.nextInt(4)];
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
                if (blockId != AIR_ID) return;
                const int belowBlockId = worldIn->getBlockId(checkPos.down());
                if (belowBlockId == AIR_ID) {
                    airCount++;
                    if (airCount > 2) return;
                } else {
                    airCount = 0;
                }
            }

            Pos3D placePos = blockPos;
            for (int i = trunkLength; i > 0; --i) {
                placePos = placePos.offset(facing);
                worldIn->setBlock(placePos, woodType->getStateFromMeta(
                    states::Log::withProperty(woodType->getDataTag(), getAxis(facing))));

                if (rand.nextInt(10) == 0) {
                    const int aboveBlockId = worldIn->getBlockId(placePos.up());
                    if (aboveBlockId == AIR_ID) {
                        const double chance = rand.nextFloat();
                        if (chance < vineChance) {
                            if (facing == EnumFacing::NORTH || facing == EnumFacing::SOUTH) {
                                addVine(worldIn, placePos.east(), EnumFacing::WEST);
                            } else {
                                addVine(worldIn, placePos.north(), EnumFacing::SOUTH);
                            }
                        } else {
                            if (facing == EnumFacing::NORTH || facing == EnumFacing::SOUTH) {
                                addVine(worldIn, placePos.west(), EnumFacing::EAST);
                            } else {
                                addVine(worldIn, placePos.south(), EnumFacing::NORTH);
                            }
                        }
                    }
                }
            }
        }
    }
}

int WorldGenAbstractTree::placeTrunk(World* worldIn, RNG& rand, const Pos3D& pos, int height,
                                     const lce::Block* woodType, c_bool vinesGrow) {
    const int fallenTrunk = rand.nextInt(80);
    double vineGrowthChance = 0.0;

    if (woodType != &lce::BlocksInit::BIRCH_WOOD) {
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
            if (lce::blocks::isAirOrLeavesBlock(blockId) || blockId == VINES_ID) {
                worldIn->setBlock(pos.up(j3), woodType);
            }
        }
    } else {
        for (int j3 = 0; j3 < height; ++j3) {
            const int blockId = worldIn->getBlockId(pos.up(j3));
            if (lce::blocks::isAirOrLeavesBlock(blockId) || blockId == VINES_ID) {
                worldIn->setBlock(pos.up(j3), woodType);

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(-1, j3, 0))) {
                    addVine(worldIn, pos.add(-1, j3, 0), EnumFacing::EAST);
                }

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(1, j3, 0))) {
                    addVine(worldIn, pos.add(1, j3, 0), EnumFacing::WEST);
                }

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(0, j3, -1))) {
                    addVine(worldIn, pos.add(0, j3, -1), EnumFacing::SOUTH);
                }

                if (rand.nextFloat() < vineGrowthChance && worldIn->isAirBlock(pos.add(0, j3, 1))) {
                    addVine(worldIn, pos.add(0, j3, 1), EnumFacing::NORTH);
                }
            }
        }
    }
    return fallenTrunk == 0 ? 0 : 1;
}
