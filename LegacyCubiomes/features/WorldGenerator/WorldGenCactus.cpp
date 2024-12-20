#include "WorldGenCactus.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockCactus::canBlockStay(const World *worldIn, const Pos3D &pos) {
    using namespace lce::blocks;
    for (FACING enumFacing: FACING_HORIZONTAL) {
        int blockId = worldIn->getBlockId(pos.offset(enumFacing));
        if (blockId == ids::STILL_LAVA_ID || blockId == ids::FLOWING_LAVA_ID) {
            return false;
        }
    }

    int blockId = worldIn->getBlockId(pos.down());
    return blockId == ids::CACTUS_ID || blockId == ids::SAND_ID && !ids::isLiquidBlock(worldIn->getBlockId(pos.up()));
}

bool WorldGenCactus::generate(World *worldIn, RNG &rng, const Pos3D &pos) const {
    Pos3D position = {pos.x, 0, pos.z};
    for (int i = 10; i != 0; --i) {
        Pos3D blockPos = position.add(rng.nextInt(8) - rng.nextInt(8), rng.nextInt(4) - rng.nextInt(4),
                                      rng.nextInt(8) - rng.nextInt(8));

        if (worldIn->isAirBlock(blockPos)) {
            int iVar1 = 0;
            int j = 1 + rng.nextInt(rng.nextInt(3) + 1);

            for (int k = 0; k < j; ++k) {
                if (BlockCactus::canBlockStay(worldIn, blockPos)) {
                    worldIn->setBlock(blockPos.up(iVar1), lce::blocks::ids::CACTUS_ID);
                }
                iVar1 = iVar1 + 1;
            }
        }
    }

    return true;
}
