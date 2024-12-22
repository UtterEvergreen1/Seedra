#include "WorldGenCactus.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockCactus::canBlockStay(World *worldIn, const Pos3D &pos) {
    using namespace lce::blocks;
    for (FACING enumFacing: FACING_HORIZONTAL) {
        int blockId = worldIn->getBlockId(pos.offset(enumFacing));
        if (ids::isSolidBlock(blockId) || ids::isLavaMaterial(blockId)) {
            return false;
        }
    }

    int blockId = worldIn->getBlockId(pos.down());
    return blockId == ids::CACTUS_ID || blockId == ids::SAND_ID && !ids::isLiquidBlock(worldIn->getBlockId(pos.up()));
}

bool WorldGenCactus::generate(World *worldIn, RNG &rng, const Pos3D &pos) const {
    Pos3D position = pos;
    for (int i = 10; i != 0; --i) {
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

        if (worldIn->isAirBlock(blockPos)) {
            int cactusYPos = 0;
            int height = 1 + rng.nextInt(rng.nextInt(3) + 1);

            for (int k = 0; k < height; ++k) {
                if (BlockCactus::canBlockStay(worldIn, blockPos)) {
                    worldIn->setBlock(blockPos.up(cactusYPos), lce::blocks::ids::CACTUS_ID);
                }
                cactusYPos++;
            }
        }
    }

    return true;
}
