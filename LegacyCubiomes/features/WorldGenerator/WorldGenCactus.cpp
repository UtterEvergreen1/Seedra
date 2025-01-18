#include "WorldGenCactus.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/block_ids.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"

using namespace lce::blocks;


bool BlockCactus::canBlockStay(World* worldIn, const Pos3D& pos) {
    for (const enumFacing enumFacing : FACING_HORIZONTAL) {
        c_int blockId = worldIn->getBlockId(pos.offset(enumFacing));
        if (ids::isSolidBlock(blockId) || ids::isLavaMaterial(blockId)) {
            return false;
        }
    }

    c_int blockId = worldIn->getBlockId(pos.down());
    return blockId == ids::CACTUS_ID || blockId == ids::SAND_ID && !ids::isLiquidBlock(worldIn->getBlockId(pos.up()));
}

bool WorldGenCactus::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    const Pos3D position = pos;
    for (int i = 10; i != 0; --i) {
        Pos3D blockPos = position + getWorldGenPos3D<8, 4, 8>(worldIn, rng);
        
        if (worldIn->isAirBlock(blockPos)) {
            int cactusYPos = 0;
            c_int height = 1 + rng.nextInt(rng.nextInt(3) + 1);

            for (int k = 0; k < height; ++k) {
                if (BlockCactus::canBlockStay(worldIn, blockPos)) {
                    worldIn->setBlock(blockPos.up(cactusYPos), ids::CACTUS_ID);
                }
                cactusYPos++;
            }
        }
    }

    return true;
}
