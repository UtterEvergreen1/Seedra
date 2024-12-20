#include "WorldGenCucurbitsBlock.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenCucurbitsBlock::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {
    for (int i = 0; i < 64; ++i) {
        Pos3D blockPos = pos.add(rng.nextInt(8) - rng.nextInt(8), rng.nextInt(4) - rng.nextInt(4),
                                 rng.nextInt(8) - rng.nextInt(8));

        if (lce::blocks::ids::isReplaceableBlock(worldIn->getBlockId(blockPos)) &&
            worldIn->getBlockId(blockPos.down()) == lce::blocks::ids::GRASS_ID) {
            worldIn->setBlock(blockPos, this->blockId, this->hasRotation ? getRandomRotationMetaData(rng) : 0);
        }
    }

    return true;
}

int WorldGenCucurbitsBlock::getRandomRotationMetaData(RNG &rng) {
    return getMetaFromHorizontalFacing(FACING_HORIZONTAL[rng.nextInt(4)]);
}
