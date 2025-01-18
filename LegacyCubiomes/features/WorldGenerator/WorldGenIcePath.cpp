#include "WorldGenIcePath.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"

using namespace lce::blocks;


const lce::blocks::Block* WorldGenIcePath::PATH_BLOCK = &lce::blocks::BlocksInit::PACKED_ICE;


bool WorldGenIcePath::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {
    Pos3D position = pos;
    while (worldIn->isAirBlock(position) && position.getY() > 2) {
        position = position.down();
    }

    if (worldIn->getBlockId(position) != ids::SNOW_BLOCK_ID) {
        return false;
    }

    const int i = rng.nextInt(this->pathWidth - 2) + 2;
    // int j = 1;

    for (int xPos = position.getX() - i; xPos <= position.getX() + i; ++xPos) {
        for (int zPos = position.getZ() - i; zPos <= position.getZ() + i; ++zPos) {
            const int i1 = xPos - position.getX();
            const int j1 = zPos - position.getZ();

            if (i1 * i1 + j1 * j1 <= i * i) {
                for (int k1 = position.getY() - 1; k1 <= position.getY() + 1; ++k1) {
                    Pos3D blockPos = {xPos, k1, zPos};
                    const int blockId = worldIn->getBlockId(blockPos);

                    if (blockId == ids::DIRT_ID || blockId == ids::SNOW_BLOCK_ID || blockId == ids::ICE_ID) {
                        worldIn->setBlock(blockPos, PATH_BLOCK);
                    }
                }
            }
        }
    }

    return true;
}
