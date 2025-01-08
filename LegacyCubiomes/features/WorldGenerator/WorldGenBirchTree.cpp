#include "WorldGenBirchTree.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenBirchTree::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    int height = rng.nextInt(3) + 5;
    if (this->useExtraRandomHeight) { height += rng.nextInt(7); }

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
        if (lce::blocks::ids::isGrassOrDirtOrFarmland(belowBlockId) && pos.getY() + height < 255) {
            setDirtAt(worldIn, pos.down());

            if (!placeTrunk(worldIn, rng, pos, height, &lce::blocks::BlocksInit::BIRCH_WOOD, false)) return true;

            for (int y = pos.getY() - 3 + height; y <= pos.getY() + height; ++y) {
                c_int yOffset = y - (pos.getY() + height);
                int radius = 1 - yOffset / 2;

                for (int x = pos.getX() - radius; x <= pos.getX() + radius; ++x) {
                    int xOffset = x - pos.getX();
                    for (int z = pos.getZ() - radius; z <= pos.getZ() + radius; ++z) {
                        int zOffset = z - pos.getZ();
                        if (std::abs(xOffset) != radius || std::abs(zOffset) != radius ||
                            rng.nextInt(2) != 0 && yOffset != 0) {
                            c_int blockId = worldIn->getBlockId(x, y, z);
                            if (lce::blocks::ids::isAirOrLeavesBlock(blockId) || blockId == lce::blocks::ids::VINES_ID) {
                                worldIn->setBlock(x, y, z, &lce::blocks::BlocksInit::JUNGLE_LEAVES);
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
