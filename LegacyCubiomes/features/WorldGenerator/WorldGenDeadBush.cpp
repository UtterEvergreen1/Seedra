#include "WorldGenDeadBush.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "lce/blocks/blocks.hpp"

bool BlockDeadBush::canPlaceBlockAt(const World* chunk, const Pos3D& pos) {
    return BlockDeadBush::canSustainBush(chunk->getBlockId(pos.down()));
}

bool BlockDeadBush::canSustainBush(const int blockId) {
    return blockId == lce::blocks::ids::SAND_ID || blockId == lce::blocks::ids::HARDENED_CLAY_ID || blockId == lce::blocks::ids::STAINED_HARDENED_CLAY_ID || blockId == lce::blocks::ids::DIRT_ID;
}

bool BlockDeadBush::canBlockStay(const World *world, const Pos3D &pos) {
    return BlockDeadBush::canSustainBush(world->getBlockId(pos.down()));
}

bool WorldGenDeadBush::generate(World * worldIn, RNG &rand, const Pos3D &pos) const {
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

        if (worldIn->isAirBlock(blockPos) && BlockDeadBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, &BlocksInit::DEAD_BUSH);
        }
    }

    return true;
}
