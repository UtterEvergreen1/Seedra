#include "WorldGenDoublePlant.hpp"
#include "WorldGenBush.hpp"
#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool BlockDoublePlant::canPlaceBlockAt(World* chunk, const Pos3D& pos) {
    return BlockBush::canPlaceBlockAt(chunk, pos) && chunk->isAirBlock(pos.up());
}

void BlockDoublePlant::placeAt(World* chunk, const Pos3D& lowerPos, const EnumPlantType variant) {
    chunk->setBlock(lowerPos, DOUBLE_PLANT_ID, static_cast<int>(variant));
    chunk->setBlock(lowerPos.up(), DOUBLE_PLANT_ID, 10); // 8 is the top | 2 is facing north
}

bool WorldGenDoublePlant::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    bool flag = false;

    for (int i = 0; i < 64; ++i) {
        Pos3D blockPos = pos + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockDoublePlant::canPlaceBlockAt(worldIn, blockPos)) {
            BlockDoublePlant::placeAt(worldIn, blockPos, this->plantType);
            flag = true;
        }
    }

    return flag;
}