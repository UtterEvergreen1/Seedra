#include "WorldGenLiquids.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "lce/blocks/blocks.hpp"

bool WorldGenLiquids::generate(World * worldIn, RNG& rand, const Pos3D& position) const {
    using namespace lce::blocks;

    if (worldIn->getBlockId(position.up()) != ids::STONE_ID)
        return false;

    if (worldIn->getBlockId(position.down()) != ids::STONE_ID)
        return false;

    const int blockId = worldIn->getBlockId(position);

    if (blockId != ids::AIR_ID && blockId != ids::STONE_ID)
        return false;

    int numStoneBlocks = 0;
    int numAirBlocks = 0;
    for (const FACING facing : FACING_HORIZONTAL) {
        if (worldIn->getBlockId(position.offset(facing)) == ids::STONE_ID) {
            ++numStoneBlocks;
        }
        else if (worldIn->getBlockId(position.offset(facing)) == ids::AIR_ID) {
            ++numAirBlocks;
        }
    }

    if (numStoneBlocks == 3 && numAirBlocks == 1) {
        worldIn->setBlock(position, this->blockId);
        //worldIn->immediateBlockTick(position, this->block, rand);
    }

    return true;
}
