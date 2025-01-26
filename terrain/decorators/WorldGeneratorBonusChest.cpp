#include "WorldGeneratorBonusChest.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGeneratorBonusChest::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    Pos3D position = pos;
    while (lce::blocks::isAirOrLeavesBlock(worldIn->getBlockId(pos)) && position.getY() > 1) { position = pos.down(); }

    if (pos.getY() < 1) { return false; }

    position = position.up();

    for (int i = 0; i < 4; ++i) {
        Pos3D blockPos = position + getWorldGenPos3D<4, 3, 4>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && isFullyOpaqueBlock(worldIn->getBlockId(blockPos.down()))) {
            worldIn->setBlock(blockPos, &lce::BlocksInit::CHEST);
            // Assuming TileEntityChest and LootTableList are handled elsewhere in the C++ codebase
            // TileEntity* tileentity = worldIn->getTileEntity(blockPos);
            // if (tileentity && dynamic_cast<TileEntityChest*>(tileentity)) {
            //     static_cast<TileEntityChest*>(tileentity)->setLootTable(LootTableList::CHESTS_SPAWN_BONUS_CHEST, rand.nextLong());
            // }

            const Pos3D eastBlock = blockPos.east();
            const Pos3D westBlock = blockPos.west();
            const Pos3D northBlock = blockPos.north();
            const Pos3D southBlock = blockPos.south();

            if (worldIn->isAirBlock(eastBlock) && isFullyOpaqueBlock(worldIn->getBlockId(eastBlock.down()))) {
                worldIn->setBlockId(eastBlock, TORCH_ID);
            }

            if (worldIn->isAirBlock(westBlock) && isFullyOpaqueBlock(worldIn->getBlockId(westBlock.down()))) {
                worldIn->setBlockId(westBlock, TORCH_ID);
            }

            if (worldIn->isAirBlock(northBlock) && isFullyOpaqueBlock(worldIn->getBlockId(northBlock.down()))) {
                worldIn->setBlockId(northBlock, TORCH_ID);
            }

            if (worldIn->isAirBlock(southBlock) && isFullyOpaqueBlock(worldIn->getBlockId(southBlock.down()))) {
                worldIn->setBlockId(southBlock, TORCH_ID);
            }

            return true;
        }
    }

    return false;
}
