#include "WorldGenSand.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"

using namespace lce::blocks;


bool WorldGenSand::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    if (!ids::isWaterMaterial(worldIn->getBlockId(position))) { return false; }

    c_int tempRadius = rand.nextInt(this->radius - 2) + 2;

    for (int k = position.getX() - tempRadius; k <= position.getX() + tempRadius; ++k) {
        for (int l = position.getZ() - tempRadius; l <= position.getZ() + tempRadius; ++l) {
            c_int i1 = k - position.getX();
            c_int j1 = l - position.getZ();

            if (i1 * i1 + j1 * j1 <= tempRadius * tempRadius) {
                for (int k1 = position.getY() - 2; k1 <= position.getY() + 2; ++k1) {
                    Pos3D blockPos(k, k1, l);
                    c_int blockId = worldIn->getBlockId(blockPos);

                    if (blockId == ids::DIRT_ID || blockId == ids::GRASS_ID) {
                        worldIn->setBlock(blockPos, this->block);
                    }
                }
            }
        }
    }

    return true;
}
