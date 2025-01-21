#include "WorldGenBush.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool BlockBush::canPlaceBlockAt(World* world, const Pos3D& pos) {
    return isGrassOrDirtOrFarmland(world->getBlockId(pos.down()));
}


bool BlockBush::canBlockStay(World* world, const Pos3D& pos) {
    return isGrassOrDirtOrFarmland(world->getBlockId(pos.down()));
}

bool WorldGenBush::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    for (int i = 0; i < 64; ++i) {
        Pos3D blockPos = pos + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, this->block);
        }
    }

    return true;
}