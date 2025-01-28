#include "WorldGenTallGrass.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"


using namespace lce::blocks;


lce::BlockState BlockTallGrass::getBlockVariantById(const EnumType type) {
    switch (type) {
        case EnumType::DEAD_BUSH:
            return lce::BlocksInit::DEAD_BUSH.getState();
        case EnumType::FERN:
            return lce::BlocksInit::TALL_GRASS_FERN.getState();
        case EnumType::GRASS:
            return lce::BlocksInit::TALL_GRASS_GRASS.getState();
        case EnumType::TALL_GRASS:
            return lce::BlocksInit::DOUBLE_TALLGRASS.getState();
    }
    return lce::BlocksInit::DEAD_BUSH.getState();
}


bool BlockTallGrass::canBlockStay(World* worldIn, const Pos3D& pos) {
    return lce::blocks::isGrassOrDirtOrFarmland(worldIn->getBlockId(pos.down()));
}

bool WorldGenTallGrass::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    Pos3D position = pos;
    for (int blockId = worldIn->getBlockId(position); (lce::blocks::isAirOrLeavesBlock(blockId)) && position.getY() > 0;
         blockId = worldIn->getBlockId(position)) {
        position = position.down();
    }

    for (int i = 0; i < 128; ++i) {
        Pos3D blockPos = position + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockTallGrass::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, BlockTallGrass::getBlockVariantById(this->type));
        }
    }

    return true;
}
