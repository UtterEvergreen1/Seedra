#include "WorldGenTallGrass.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"

using namespace lce::blocks;


const Block* BlockTallGrass::getBlockVariantById(const EnumType type) {
    switch (type) {
        case EnumType::DEAD_BUSH:
            return &BlocksInit::DEAD_BUSH;
        case EnumType::FERN:
            return &BlocksInit::TALL_GRASS_FERN;
        case EnumType::GRASS:
            return &BlocksInit::TALL_GRASS_GRASS;
        case EnumType::TALL_GRASS:
            return &BlocksInit::DOUBLE_TALLGRASS;
    }
    return &BlocksInit::DEAD_BUSH;
}


bool BlockTallGrass::canBlockStay(World* worldIn, const Pos3D& pos) {
    return ids::isGrassOrDirtOrFarmland(worldIn->getBlockId(pos.down()));
}

bool WorldGenTallGrass::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    Pos3D position = pos;
    for (int blockId = worldIn->getBlockId(position); (ids::isAirOrLeavesBlock(blockId)) && position.getY() > 0;
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
