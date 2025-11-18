#include "WorldGenFlowers.hpp"
#include "WorldGenBush.hpp"

#include "terrain/World.hpp"


bool WorldGenFlowers::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 64; ++i) {
        const Pos3D blockPos = position + getWorldGenPos3D<8, 4, 8>(worldIn, rng);

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlockAndData(blockPos, this->m_flowerId,
                                     static_cast<int>(this->m_flowerType) == 0
                                     ? 0
                                     : static_cast<int>(this->m_flowerType) - 1);
        }
    }
    return true;
}
