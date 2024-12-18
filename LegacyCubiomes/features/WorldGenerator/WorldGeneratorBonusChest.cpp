#include "WorldGeneratorBonusChest.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGeneratorBonusChest::generate(ChunkPrimer *worldIn, RNG &rand, const Pos3D &pos) const {
    using namespace lce::blocks;
    Pos3D position = pos;
    while ((worldIn->getBlockId(position) == ids::AIR_ID || ids::isLeavesBlock(worldIn->getBlockId(pos))) &&
        position.getY() > 1) {
        position = pos.down();
    }

    if (pos.getY() < 1) {
        return false;
    }

    position = position.up();

    for (int i = 0; i < 4; ++i) {
        Pos3D blockPos = position.add(rand.nextInt(4) - rand.nextInt(4), rand.nextInt(3) - rand.nextInt(3),
                                      rand.nextInt(4) - rand.nextInt(4));

        if (worldIn->isAirBlock(blockPos) && ids::isFullyOpaqueBlock(worldIn->getBlockId(blockPos.down()))) {
            worldIn->setBlock(blockPos, &CHEST);
            // Assuming TileEntityChest and LootTableList are handled elsewhere in the C++ codebase
            // TileEntity* tileentity = worldIn->getTileEntity(blockPos);
            // if (tileentity && dynamic_cast<TileEntityChest*>(tileentity)) {
            //     static_cast<TileEntityChest*>(tileentity)->setLootTable(LootTableList::CHESTS_SPAWN_BONUS_CHEST, rand.nextLong());
            // }

            Pos3D eastBlock = blockPos.east();
            Pos3D westBlock = blockPos.west();
            Pos3D northBlock = blockPos.north();
            Pos3D southBlock = blockPos.south();

            if (worldIn->isAirBlock(eastBlock) && ids::isFullyOpaqueBlock(worldIn->getBlockId(eastBlock.down()))) {
                worldIn->setBlockId(eastBlock, ids::TORCH_ID);
            }

            if (worldIn->isAirBlock(westBlock) && ids::isFullyOpaqueBlock(worldIn->getBlockId(westBlock.down()))) {
                worldIn->setBlockId(westBlock, ids::TORCH_ID);
            }

            if (worldIn->isAirBlock(northBlock) && ids::isFullyOpaqueBlock(worldIn->getBlockId(northBlock.down()))) {
                worldIn->setBlockId(northBlock, ids::TORCH_ID);
            }

            if (worldIn->isAirBlock(southBlock) && ids::isFullyOpaqueBlock(worldIn->getBlockId(southBlock.down()))) {
                worldIn->setBlockId(southBlock, ids::TORCH_ID);
            }

            return true;
        }
    }

    return false;
}
