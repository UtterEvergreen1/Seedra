#include "WorldGenTrees.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


const lce::blocks::Block* WorldGenTrees::DEFAULT_TRUNK = &lce::blocks::BlocksInit::OAK_WOOD;
const lce::blocks::Block* WorldGenTrees::DEFAULT_LEAF = &lce::blocks::BlocksInit::OAK_LEAVES;

bool WorldGenTrees::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    const int height = rng.nextInt(3) + this->minTreeHeight;

    if (pos.getY() >= 1 && pos.getY() + height <= 255) {
        bool flag = true;
        for (int y = pos.getY(); y <= pos.getY() + 1 + height; ++y) {
            int radius = 1;

            if (y == pos.getY()) { radius = 0; }

            if (y >= pos.getY() + 1 + height - 2) { radius = 2; }

            for (int x = pos.getX() - radius; x <= pos.getX() + radius && flag; ++x) {
                for (int z = pos.getZ() - radius; z <= pos.getZ() + radius && flag; ++z) {
                    if (y >= 0 && y < 256) {
                        if (!canGrowInto(worldIn->getBlockId(x, y, z))) { flag = false; }
                    } else {
                        flag = false;
                    }
                }
            }
        }

        if (!flag) { return false; }

        const int belowBlockId = worldIn->getBlockId(pos.down());

        using namespace lce::blocks;
        if ((belowBlockId == ids::GRASS_ID || belowBlockId == ids::DIRT_ID || belowBlockId == ids::FARMLAND_ID) &&
            pos.getY() < 256 - height - 1) {
            setDirtAt(worldIn, pos.down());
            //placeTrunk
            if (!placeTrunk(worldIn, rng, pos, height, this->metaWood, this->vinesGrow)) return true;
            const int startY = pos.getY();
            const int topY = startY + height;
            const int leafStartY = startY - 3 + height;
            for (int y = topY; y >= leafStartY; --y) {
                const int yOffset = y - topY;
                int radius = 1 - yOffset / 2;

                for (int x = pos.getX() - radius; x <= pos.getX() + radius; ++x) {
                    for (int z = pos.getZ() - radius; z <= pos.getZ() + radius; ++z) {
                        if (std::abs(x - pos.getX()) != radius || std::abs(z - pos.getZ()) != radius ||
                            (rng.nextInt(2) != 0 && yOffset != 0)) {
                            Pos3D blockPos = {x, y, z};
                            const int blockId = worldIn->getBlockId(blockPos);

                            if (ids::isAirOrLeavesBlock(blockId) || blockId == ids::VINES_ID) {
                                worldIn->setBlock(blockPos, this->metaLeaves);
                            }
                        }
                    }
                }
            }


            if (this->vinesGrow) {
                for (int k3 = pos.getY() - 3 + height; k3 <= pos.getY() + height; ++k3) {
                    const int j4 = k3 - (pos.getY() + height);
                    const int k4 = 2 - j4 / 2;
                    Pos3D mutablePosition1;

                    for (int l4 = pos.getX() - k4; l4 <= pos.getX() + k4; ++l4) {
                        for (int i5 = pos.getZ() - k4; i5 <= pos.getZ() + k4; ++i5) {
                            mutablePosition1.setPos(l4, k3, i5);

                            if (const int blockId = worldIn->getBlockId(mutablePosition1); ids::isLeavesBlock(blockId)) {
                                Pos3D blockPos2 = mutablePosition1.west();
                                Pos3D blockPos3 = mutablePosition1.east();
                                Pos3D blockPos4 = mutablePosition1.north();
                                Pos3D blockPos1 = mutablePosition1.south();

                                if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos2)) {
                                    addHangingVine(worldIn, blockPos2, FACING::EAST);
                                }

                                if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos3)) {
                                    addHangingVine(worldIn, blockPos3, FACING::WEST);
                                }

                                if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos4)) {
                                    addHangingVine(worldIn, blockPos4, FACING::SOUTH);
                                }

                                if (rng.nextInt(4) == 0 && worldIn->isAirBlock(blockPos1)) {
                                    addHangingVine(worldIn, blockPos1, FACING::NORTH);
                                }
                            }
                        }
                    }
                }

                if (rng.nextInt(5) == 0 && height > 5) {
                    for (int l3 = 0; l3 < 2; ++l3) {
                        for (const FACING enumFacing: FACING_HORIZONTAL) {
                            if (rng.nextInt(4 - l3) == 0) {
                                const FACING enumFacing1 = getOppositeFacing(enumFacing);
                                placeCocoa(worldIn,
                                           pos.add(getFrontOffsetX(enumFacing1), height - 5 + l3,
                                                   getFrontOffsetZ(enumFacing1)),
                                           enumFacing, rng.nextInt(3));
                            }
                        }
                    }
                }
            }
            return true;
        }
    }
    return false;
}

void WorldGenTrees::placeCocoa(World* worldIn, const Pos3D& pos, FACING side, int age) {
    worldIn->setBlock(pos, lce::blocks::ids::VINES_ID); //TODO: set rotation data and age data
}

void WorldGenTrees::addHangingVine(World* worldIn, const Pos3D& pos, const FACING direction) {
    addVine(worldIn, pos, direction);
    int i = 4;

    for (Pos3D blockPos = pos.down(); worldIn->getBlockId(blockPos) == lce::blocks::ids::AIR_ID && i > 0; --i) {
        addVine(worldIn, blockPos, direction);
        blockPos = blockPos.down();
    }
}
