#include "WorldGenWaterlily.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "WorldGenBush.hpp"
#include "lce/blocks/block_ids.hpp"

using namespace lce::blocks;


bool BlockLilyPad::canPlaceBlockAt(World* worldIn, const Pos3D& pos) {
    return BlockBush::canPlaceBlockAt(worldIn, pos) && canSustainBush(worldIn->getBlockId(pos.down()));
}

bool BlockLilyPad::canSustainBush(const int blockId) {
    return blockId == ids::STILL_WATER_ID || blockId == ids::ICE_ID || blockId == ids::PACKED_ICE_ID;
}

bool WorldGenWaterlily::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 10; ++i) {
        Pos3D blockPos = position + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockLilyPad::canPlaceBlockAt(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, ids::LILY_PAD_ID);
        }
    }

    return true;
}
