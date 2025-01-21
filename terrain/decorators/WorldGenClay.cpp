#include "WorldGenClay.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenClay::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    if (!isWaterMaterial(worldIn->getBlockId(position))) { return false; }

    const int radius = rand.nextInt(this->numberOfBlocks - 2) + 2;

    for (int k = position.getX() - radius; k <= position.getX() + radius; ++k) {
        for (int l = position.getZ() - radius; l <= position.getZ() + radius; ++l) {
            const int i1 = k - position.getX();
            const int j1 = l - position.getZ();

            if (i1 * i1 + j1 * j1 <= radius * radius) {
                for (int k1 = position.getY() - 1; k1 <= position.getY() + 1; ++k1) {
                    Pos3D blockPos(k, k1, l);
                    const int blockId = worldIn->getBlockId(blockPos);

                    if (blockId == DIRT_ID || blockId == CLAY_BLOCK_ID) {
                        worldIn->setBlock(blockPos, &lce::BlocksInit::CLAY_BLOCK);
                    }
                }
            }
        }
    }

    return true;
}
