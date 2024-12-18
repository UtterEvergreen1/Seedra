#include "WorldGenDeadBush.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "lce/blocks/blocks.hpp"

bool WorldGenDeadBush::generate(ChunkPrimer *worldIn, RNG &rand, const Pos3D &pos) const {
    using namespace lce::blocks;
    Pos3D position = pos;
    for (int blockId = worldIn->getBlockId(position);
         (blockId == ids::AIR_ID || ids::isLeavesBlock(blockId)) && position.getY() > 0;
         blockId = worldIn->getBlockId(position)) {
        position = position.down();
    }

    for (int i = 0; i < 4; ++i) {
        Pos3D blockPos = position.add(rand.nextInt(8) - rand.nextInt(8), rand.nextInt(4) - rand.nextInt(4),
                                      rand.nextInt(8) - rand.nextInt(8));

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, &DEAD_BUSH);
        }
    }

    return true;
}
