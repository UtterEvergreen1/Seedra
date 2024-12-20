#include "WorldGenFlowers.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenFlowers::generate(World * worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 64; ++i) {
        Pos3D blockPos = position.add(rng.nextInt(8) - rng.nextInt(8), rng.nextInt(4) - rng.nextInt(4), rng.nextInt(8) - rng.nextInt(8));

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, this->flowerId, (int)this->flowerType == 0 ? 0 : (int)this->flowerType - 1);
        }
    }
    return true;
}
