#include "WorldGenWaterlily.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockLilyPad::canPlaceBlockAt(const ChunkPrimer *worldIn, const Pos3D &pos) {
    return BlockBush::canPlaceBlockAt(worldIn, pos) && BlockLilyPad::canSustainBush(worldIn->getBlockId(pos.down()));
}

bool BlockLilyPad::canSustainBush(const int blockId) {
    return blockId == lce::blocks::ids::STILL_WATER_ID || blockId == lce::blocks::ids::ICE_ID ||
           blockId == lce::blocks::ids::PACKED_ICE_ID;
}

bool WorldGenWaterlily::generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &position) const {
    for (int i = 0; i < 10; ++i) {
        int x = position.getX() + rng.nextInt(8) - rng.nextInt(8);
        int y = position.getY() + rng.nextInt(4) - rng.nextInt(4);
        int z = position.getZ() + rng.nextInt(8) - rng.nextInt(8);

        if (worldIn->isAirBlock(x, y, z) && BlockLilyPad::canPlaceBlockAt(worldIn, {x, y, z})) {
            worldIn->setBlockId(x, y, z, lce::blocks::ids::LILY_PAD_ID);
        }
    }

    return true;
}
