#include "WorldGenDeadBush.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool BlockDeadBush::canPlaceBlockAt(World* world, const Pos3D& pos) {
    return canSustainBush(world->getBlockId(pos.down()));
}

bool BlockDeadBush::canSustainBush(const int blockId) {
    return blockId == SAND_ID ||
           blockId == HARDENED_CLAY_ID ||
           blockId == STAINED_HARDENED_CLAY_ID ||
           blockId == DIRT_ID;
}

bool BlockDeadBush::canBlockStay(World *world, const Pos3D &pos) {
    return canSustainBush(world->getBlockId(pos.down()));
}

bool WorldGenDeadBush::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {

    Pos3D position = pos;
    for (int blockId = worldIn->getBlockId(position);
         lce::blocks::isAirOrLeavesBlock(blockId) && position.getY() > 0;
         blockId = worldIn->getBlockId(position)) {
        position = position.down();
    }

    for (int i = 0; i < 4; ++i) {
        Pos3D blockPos = position + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockDeadBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, lce::BlocksInit::DEAD_BUSH.getState());
        }
    }

    return true;
}
