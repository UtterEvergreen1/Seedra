#include "WorldGenFlowers.hpp"
#include "WorldGenBush.hpp"

#include "terrain/World.hpp"


bool WorldGenFlowers::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 64; ++i) {
        Pos3D blockPos = position + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, this->flowerId, static_cast<int>(this->flowerType) == 0
                ? 0 : static_cast<int>(this->flowerType) - 1);
        }
    }
    return true;
}
