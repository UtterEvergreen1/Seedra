#include "WorldGenSand.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenSand::generate(World * worldIn, RNG& rand, const Pos3D& position) const {
    if (!lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(position))) {
        return false;
    }

    const int radius = rand.nextInt(this->radius - 2) + 2;

    for (int k = position.getX() - radius; k <= position.getX() + radius; ++k) {
        for (int l = position.getZ() - radius; l <= position.getZ() + radius; ++l) {
            const int i1 = k - position.getX();
            const int j1 = l - position.getZ();

            if (i1 * i1 + j1 * j1 <= radius * radius) {
                for (int k1 = position.getY() - 2; k1 <= position.getY() + 2; ++k1) {
                    Pos3D blockPos(k, k1, l);
                    const int blockId = worldIn->getBlockId(blockPos);

                    if (blockId == lce::blocks::ids::DIRT_ID || blockId == lce::blocks::ids::GRASS_ID) {
                        worldIn->setBlock(blockPos, this->block);
                    }
                }
            }
        }
    }

    return true;
}
