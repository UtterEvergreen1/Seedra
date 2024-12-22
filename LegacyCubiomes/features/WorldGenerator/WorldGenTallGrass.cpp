#include "WorldGenTallGrass.hpp"
#include "lce/blocks/block_ids.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockTallGrass::canBlockStay(World *worldIn, const Pos3D &pos) {
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

        if (worldIn->isAirBlock(blockPos) && BlockTallGrass::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, BlockTallGrass::getBlockVarientById(this->type));
        }
    }

    return true;
}
