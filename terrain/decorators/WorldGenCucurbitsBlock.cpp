#include "WorldGenCucurbitsBlock.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"
#include "terrain/generator.hpp"


bool WorldGenCucurbitsBlock::generate(World* worldIn, RNG &rng, const Pos3D &pos) const {
    for (int i = 0; i < 64; ++i) {
        Pos3D blockPos = pos + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (lce::blocks::isReplaceableBlock(worldIn->getBlockId(blockPos)) &&
            worldIn->getBlockId(blockPos.down()) == lce::blocks::GRASS_ID) {
            worldIn->setBlock(blockPos, this->blockId, this->hasRotation ? getRandomRotationMetaData(rng) : 0);
        }
    }

    return true;
}

int WorldGenCucurbitsBlock::getRandomRotationMetaData(RNG &rng) {
    rng.nextInt(4);
    return 0;
    //return getMetaFromHorizontalFacing(FACING_HORIZONTAL[rng.nextInt(4)]);
}
