#include "WorldGenLiquids.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenLiquids::generate(World * worldIn, RNG& rand, const Pos3D& position) const {
    using namespace lce::blocks;

    if (worldIn->getBlockId(position.up()) != ids::STONE_ID)
        return false;

    if (worldIn->getBlockId(position.down()) != ids::STONE_ID)
        return false;

    const int tempBlockId = worldIn->getBlockId(position);

    if (tempBlockId != ids::AIR_ID && tempBlockId != ids::STONE_ID)
        return false;

    int numStoneBlocks = 0;
    int numAirBlocks = 0;
    for (const enumFacing facing : FACING_HORIZONTAL) {
        if (worldIn->getBlockId(position.offset(facing)) == ids::STONE_ID) {
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
