#include "WorldGenClay.hpp"

#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenClay::generate(World * worldIn, RNG& rand, const Pos3D& position) const {
    if (!lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(position))) {
        return false;
    }

    int radius = rand.nextInt(this->numberOfBlocks - 2) + 2;

    for (int k = position.getX() - radius; k <= position.getX() + radius; ++k) {
        for (int l = position.getZ() - radius; l <= position.getZ() + radius; ++l) {
            int i1 = k - position.getX();
            int j1 = l - position.getZ();

            if (i1 * i1 + j1 * j1 <= radius * radius) {
                for (int k1 = position.getY() - 1; k1 <= position.getY() + 1; ++k1) {
                    Pos3D blockPos(k, k1, l);
                    int blockId = worldIn->getBlockId(blockPos);

                    if (blockId == lce::blocks::ids::DIRT_ID || blockId == lce::blocks::ids::CLAY_BLOCK_ID) {
                        worldIn->setBlock(blockPos, &lce::blocks::BlocksInit::CLAY_BLOCK);
                    }
                }
            }
        }
    }

    return true;
}
