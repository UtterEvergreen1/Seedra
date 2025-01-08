#include "WorldGenReed.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/block_ids.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"


bool BlockReed::canBlockStay(World *worldIn, const Pos3D &pos) {
    const int blockId = worldIn->getBlockId(pos.down());
    if (blockId == lce::blocks::ids::SUGAR_CANES_ID) {
        return true;
    }

    if (blockId != lce::blocks::ids::GRASS_ID &&
        blockId != lce::blocks::ids::DIRT_ID &&
        blockId != lce::blocks::ids::SAND_ID) {
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

bool WorldGenReed::generate(World * worldIn, RNG &rng, const Pos3D &position) const {
    for (int i = 0; i < 20; ++i) {
        int x_off;
        int z_off;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 && worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            x_off = rng.nextInt(4) - rng.nextInt(4);
            z_off = rng.nextInt(4) - rng.nextInt(4);
        }
        else {
            z_off = rng.nextInt(4) - rng.nextInt(4);
            x_off = rng.nextInt(4) - rng.nextInt(4);
        }
        Pos3D blockPos = position.add(x_off, 0, z_off);

        if (worldIn->isAirBlock(blockPos)) {
            Pos3D blockPos1 = blockPos.down();

            if (lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.west())) ||
                lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.east())) ||
                lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.north())) ||
                lce::blocks::ids::isWaterMaterial(worldIn->getBlockId(blockPos1.south()))) {
                const int j = 2 + rng.nextInt(rng.nextInt(3) + 1);

                for (int k = 0; k < j; ++k) {
                    if (BlockReed::canBlockStay(worldIn, blockPos)) {
                        worldIn->setBlock(blockPos.up(k), lce::blocks::ids::SUGAR_CANES_ID);
                    }
                }
            }
        }
    }

    return true;
}
