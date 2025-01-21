#include "WorldGenCactus.hpp"

#include "lce/blocks/__include.hpp"

#include "terrain/World.hpp"
#include "terrain/generator.hpp"

using namespace lce::blocks;


bool BlockCactus::canBlockStay(World* worldIn, const Pos3D& pos) {
    for (const EnumFacing enumFacing : FACING_HORIZONTAL) {
        c_int blockId = worldIn->getBlockId(pos.offset(enumFacing));
        if (lce::blocks::isSolidBlock(blockId) || lce::blocks::isLavaMaterial(blockId)) {
            return false;
        }
    }

    c_int blockId = worldIn->getBlockId(pos.down());
    return blockId == lce::blocks::CACTUS_ID || blockId == lce::blocks::SAND_ID && !lce::blocks::isLiquidBlock(worldIn->getBlockId(pos.up()));
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
                    worldIn->setBlock(blockPos.up(cactusYPos), lce::blocks::CACTUS_ID);
                }
                cactusYPos++;
            }
        }
    }

    return true;
}
