#include "WorldGenReed.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"

bool BlockReed::canBlockStay(const ChunkPrimer *worldIn, const Pos3D &pos) {
    const int blockId = worldIn->getBlockId(pos.down());
    if (blockId == lce::blocks::ids::SUGAR_CANES_ID) {
        return true;
    }

    if (blockId != lce::blocks::ids::GRASS_ID && blockId != lce::blocks::ids::DIRT_ID && blockId !=
        lce::blocks::ids::SAND_ID) {
        return false;
    }

    const Pos3D blockPos = pos.down();

    for (const FACING facing: FACING_HORIZONTAL) {
        const int id = worldIn->getBlockId(blockPos.offset(facing));
        if (lce::blocks::ids::isWaterMaterial(id) || id == lce::blocks::ids::FROSTED_ICE_ID) {
            return true;
        }
    }

    return blockId == lce::blocks::ids::GRASS_ID || blockId == lce::blocks::ids::DIRT_ID;
}

bool WorldGenReed::generate(ChunkPrimer *worldIn, RNG &rand, const Pos3D &position) const {
    for (int i = 0; i < 20; ++i) {
        Pos3D blockPos = position.add(rand.nextInt(4) - rand.nextInt(4), 0, rand.nextInt(4) - rand.nextInt(4));

        if (worldIn->isAirBlock(blockPos)) {
            Pos3D blockPos1 = blockPos.down();

            if (lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.west())) ||
                lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.east())) ||
                lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.north())) ||
                lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.south()))) {
                int j = 2 + rand.nextInt(rand.nextInt(3) + 1);

                for (int k = 0; k < j; ++k) {
                    if (BlockReed::canBlockStay(worldIn, blockPos)) {
                        worldIn->setBlockId(blockPos.up(k), lce::blocks::ids::SUGAR_CANES_ID);
                    }
                }
            }
        }
    }

    return true;
}
