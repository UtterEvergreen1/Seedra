#include "WorldGenBush.hpp"

#include "lce/blocks/block_ids.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockBush::canPlaceBlockAt(const World* chunk, const Pos3D& pos) {
    return BlockBush::canSustainBush(chunk->getBlockId(pos.down()));
}

bool BlockBush::canSustainBush(const int blockId) {
    return blockId == lce::blocks::ids::GRASS_ID || blockId == lce::blocks::ids::DIRT_ID || blockId == lce::blocks::ids::FARMLAND_ID;
}

bool BlockBush::canBlockStay(const World *world, const Pos3D &pos) {
    return BlockBush::canSustainBush(world->getBlockId(pos.down()));
}

bool WorldGenBush::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {
    for (int i = 0; i < 64; ++i)
    {
        const Pos3D blockPos = pos.add(rng.nextInt(8) - rng.nextInt(8), rng.nextInt(4) - rng.nextInt(4), rng.nextInt(8) - rng.nextInt(8));

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos))
        {
            worldIn->setBlock(blockPos, this->block);
        }
    }

    return true;
}