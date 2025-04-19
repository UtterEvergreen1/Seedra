#include "WorldGenReed.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool BlockReed::canBlockStay(World* worldIn, const Pos3D& pos) {
    const int blockId = worldIn->getBlockId(pos.down());
    if (blockId == SUGAR_CANES_ID) { return true; }

    if (blockId != GRASS_ID && blockId != DIRT_ID && blockId != SAND_ID) { return false; }

    const Pos3D blockPos = pos.down();

    for (const EnumFacing facing: FACING_HORIZONTAL) {
        const int id = worldIn->getBlockId(blockPos.offset(facing));
        if (isWaterMaterial(id) || id == FROSTED_ICE_ID) { return true; }
    }

    return blockId == GRASS_ID || blockId == DIRT_ID;
}


bool WorldGenReed::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 20; ++i) {
        Pos3D offset = getWorldGenPos3D<4, 0, 4>(worldIn, rng);
        Pos3D blockPos = position + offset;

        if (worldIn->isAirBlock(blockPos)) {
            Pos3D blockPos1 = blockPos.down();

            if (isWaterMaterial(worldIn->getBlockId(blockPos1.west())) ||
                isWaterMaterial(worldIn->getBlockId(blockPos1.east())) ||
                isWaterMaterial(worldIn->getBlockId(blockPos1.north())) ||
                isWaterMaterial(worldIn->getBlockId(blockPos1.south()))) {
                const int j = 2 + rng.nextInt(rng.nextInt(3) + 1);

                for (int k = 0; k < j; ++k) {
                    if (BlockReed::canBlockStay(worldIn, blockPos)) {
                        worldIn->setBlockId(blockPos.up(k), SUGAR_CANES_ID);
                    }
                }
            }
        }
    }

    return true;
}
