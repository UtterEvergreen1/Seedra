#include "WorldGenWaterlily.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockLilyPad::canPlaceBlockAt(World *worldIn, const Pos3D &pos) {
    return BlockBush::canPlaceBlockAt(worldIn, pos) && BlockLilyPad::canSustainBush(worldIn->getBlockId(pos.down()));
}

bool BlockLilyPad::canSustainBush(const int blockId) {
    return blockId == lce::blocks::ids::STILL_WATER_ID || blockId == lce::blocks::ids::ICE_ID ||
           blockId == lce::blocks::ids::PACKED_ICE_ID;
}

bool WorldGenWaterlily::generate(World *worldIn, RNG &rng, const Pos3D &position) const {
    for (int i = 0; i < 10; ++i) {
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

        Pos3D blockPos = position.add(x_off, y_off, z_off);

        if (worldIn->isAirBlock(blockPos) && BlockLilyPad::canPlaceBlockAt(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, lce::blocks::ids::LILY_PAD_ID);
        }
    }

    return true;
}
