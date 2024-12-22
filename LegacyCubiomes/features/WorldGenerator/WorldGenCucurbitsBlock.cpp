#include "WorldGenCucurbitsBlock.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenCucurbitsBlock::generate(World* worldIn, RNG &rng, const Pos3D &pos) const {
    for (int i = 0; i < 64; ++i) {
        int x_off;
        int y_off;
        int z_off;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 && worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            x_off = rng.nextInt(8) - rng.nextInt(8);
            y_off = rng.nextInt(4) - rng.nextInt(4);
            z_off = rng.nextInt(8) - rng.nextInt(8);
        }
        else {
            z_off = rng.nextInt(8) - rng.nextInt(8);
            y_off = rng.nextInt(4) - rng.nextInt(4);
            x_off = rng.nextInt(8) - rng.nextInt(8);
        }

        Pos3D blockPos = pos.add(x_off, y_off, z_off);

        if (lce::blocks::ids::isReplaceableBlock(worldIn->getBlockId(blockPos)) &&
            worldIn->getBlockId(blockPos.down()) == lce::blocks::ids::GRASS_ID) {
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
