#include "WorldGenDoublePlant.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool BlockDoublePlant::canPlaceBlockAt(World* chunk, const Pos3D& pos) {
    return BlockBush::canPlaceBlockAt(chunk, pos) && chunk->isAirBlock(pos.up());
}

void BlockDoublePlant::placeAt(World *chunk, const Pos3D& lowerPos, const EnumPlantType variant)
{
    chunk->setBlock(lowerPos, lce::blocks::ids::DOUBLE_PLANT_ID, static_cast<int>(variant));
    chunk->setBlock(lowerPos.up(), lce::blocks::ids::DOUBLE_PLANT_ID, 10); // 8 is the top | 2 is facing north
}

bool WorldGenDoublePlant::generate(World * worldIn, RNG& rng, const Pos3D& pos) const {
    bool flag = false;

    for (int i = 0; i < 64; ++i)
    {
        int x_off;
        int y_off;
        int z_off;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 && worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            x_off = rng.nextInt(8) - rng.nextInt(8);
            y_off = rng.nextInt(4) - rng.nextInt(4);
            z_off = rng.nextInt(8) - rng.nextInt(8);
        }
        else {
            z_off = rng.nextInt(8) - rng.nextInt(8);
            y_off = rng.nextInt(4) - rng.nextInt(4);
            x_off = rng.nextInt(8) - rng.nextInt(8);
        }
        Pos3D blockPos = pos.add(x_off, y_off, z_off);

        if (worldIn->isAirBlock(blockPos) && BlockDoublePlant::canPlaceBlockAt(worldIn, blockPos))
        {
            BlockDoublePlant::placeAt(worldIn, blockPos, this->plantType);
            flag = true;
        }
    }

    return flag;
}