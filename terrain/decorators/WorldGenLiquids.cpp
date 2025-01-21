#include "WorldGenLiquids.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"


bool WorldGenLiquids::generate(World * worldIn, RNG& rand, const Pos3D& position) const {
    using namespace lce::blocks;

    if (worldIn->getBlockId(position.up()) != STONE_ID)
        return false;

    if (worldIn->getBlockId(position.down()) != STONE_ID)
        return false;

    const int tempBlockId = worldIn->getBlockId(position);

    if (tempBlockId != AIR_ID && tempBlockId != STONE_ID)
        return false;

    int numStoneBlocks = 0;
    int numAirBlocks = 0;
    for (const EnumFacing facing : FACING_HORIZONTAL) {
        if (worldIn->getBlockId(position.offset(facing)) == STONE_ID) {
            ++numStoneBlocks;
        }
        else if (worldIn->isAirBlock(position.offset(facing))) {
            ++numAirBlocks;
        }
    }

    if (numStoneBlocks == 3 && numAirBlocks == 1) {
        worldIn->setBlock(position, this->blockId);
        //worldIn->immediateBlockTick(position, this->block, rand);
    }

    return true;
}
