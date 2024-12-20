#include "WorldGenTallGrass.hpp"
#include "lce/blocks/block_ids.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockTallGrass::canBlockStay(const World *worldIn, const Pos3D &pos) {
    return BlockBush::canSustainBush(worldIn->getBlockId(pos.down()));
}

bool WorldGenTallGrass::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {
    Pos3D position = pos;
    for (int blockId = worldIn->getBlockId(position);
         (blockId == lce::blocks::ids::AIR_ID || lce::blocks::ids::isLeavesBlock(blockId)) && position.getY() > 0;
         blockId = worldIn->getBlockId(position)) {
        position = position.down();
    }

    for (int i = 0; i < 128; ++i) {
        Pos3D blockPos = position.add(rng.nextInt(8) - rng.nextInt(8), rng.nextInt(4) - rng.nextInt(4),
                                      rng.nextInt(8) - rng.nextInt(8));

        if (worldIn->isAirBlock(blockPos) && BlockTallGrass::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, BlockTallGrass::getBlockVarientById(this->type));
        }
    }

    return true;
}
