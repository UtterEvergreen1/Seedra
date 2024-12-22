#include "WorldGenDeadBush.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "lce/blocks/blocks.hpp"

bool BlockDeadBush::canPlaceBlockAt(World* chunk, const Pos3D& pos) {
    return BlockDeadBush::canSustainBush(chunk->getBlockId(pos.down()));
}

bool BlockDeadBush::canSustainBush(const int blockId) {
    return blockId == lce::blocks::ids::SAND_ID || blockId == lce::blocks::ids::HARDENED_CLAY_ID || blockId == lce::blocks::ids::STAINED_HARDENED_CLAY_ID || blockId == lce::blocks::ids::DIRT_ID;
}

bool BlockDeadBush::canBlockStay(World *world, const Pos3D &pos) {
    return BlockDeadBush::canSustainBush(world->getBlockId(pos.down()));
}

bool WorldGenDeadBush::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {
    using namespace lce::blocks;
    Pos3D position = pos;
    for (int blockId = worldIn->getBlockId(position);
         (blockId == ids::AIR_ID || ids::isLeavesBlock(blockId)) && position.getY() > 0;
         blockId = worldIn->getBlockId(position)) {
        position = position.down();
    }

    for (int i = 0; i < 4; ++i) {
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

        if (worldIn->isAirBlock(blockPos) && BlockDeadBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, &BlocksInit::DEAD_BUSH);
        }
    }

    return true;
}
