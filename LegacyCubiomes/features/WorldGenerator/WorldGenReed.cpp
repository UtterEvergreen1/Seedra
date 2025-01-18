#include "WorldGenReed.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "lce/blocks/block_ids.hpp"

using namespace lce::blocks;


bool BlockReed::canBlockStay(World* worldIn, const Pos3D& pos) {
    const int blockId = worldIn->getBlockId(pos.down());
    if (blockId == ids::SUGAR_CANES_ID) { return true; }

    if (blockId != ids::GRASS_ID && blockId != ids::DIRT_ID && blockId != ids::SAND_ID) { return false; }

    const Pos3D blockPos = pos.down();

    for (const enumFacing facing: FACING_HORIZONTAL) {
        const int id = worldIn->getBlockId(blockPos.offset(facing));
        if (ids::isWaterMaterial(id) || id == ids::FROSTED_ICE_ID) { return true; }
    }

    return blockId == ids::GRASS_ID || blockId == ids::DIRT_ID;
}


bool WorldGenReed::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 20; ++i) {
        Pos3D offset = getWorldGenPos3D<4, 0, 4>(worldIn, rng);
        Pos3D blockPos = position + offset;

        if (worldIn->isAirBlock(blockPos)) {
            Pos3D blockPos1 = blockPos.down();

            if (ids::isWaterMaterial(worldIn->getBlockId(blockPos1.west())) ||
                ids::isWaterMaterial(worldIn->getBlockId(blockPos1.east())) ||
                ids::isWaterMaterial(worldIn->getBlockId(blockPos1.north())) ||
                ids::isWaterMaterial(worldIn->getBlockId(blockPos1.south()))) {
                const int j = 2 + rng.nextInt(rng.nextInt(3) + 1);

                for (int k = 0; k < j; ++k) {
                    if (BlockReed::canBlockStay(worldIn, blockPos)) {
                        worldIn->setBlock(blockPos.up(k), ids::SUGAR_CANES_ID);
                    }
                }
            }
        }
    }

    return true;
}
