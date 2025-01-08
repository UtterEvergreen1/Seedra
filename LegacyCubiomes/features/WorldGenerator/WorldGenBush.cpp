#include "WorldGenBush.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool BlockBush::canPlaceBlockAt(World* world, const Pos3D& pos) {
    return lce::blocks::ids::isGrassOrDirtOrFarmland(world->getBlockId(pos.down()));
}


bool BlockBush::canBlockStay(World* world, const Pos3D& pos) {
    return lce::blocks::ids::isGrassOrDirtOrFarmland(world->getBlockId(pos.down()));
}

bool WorldGenBush::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    for (int i = 0; i < 64; ++i) {
        const int x_off = rng.nextInt(8) - rng.nextInt(8);
        const int y_off = rng.nextInt(4) - rng.nextInt(4);
        const int z_off = rng.nextInt(8) - rng.nextInt(8);
        Pos3D blockPos = pos.add(x_off, y_off, z_off);

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, this->block);
        }
    }

    return true;
}