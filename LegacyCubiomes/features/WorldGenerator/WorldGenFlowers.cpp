#include "WorldGenFlowers.hpp"

#include "WorldGenBush.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"



bool WorldGenFlowers::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    for (int i = 0; i < 64; ++i) {
        int x_off;
        int y_off;
        int z_off;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 &&
            worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            x_off = rng.nextInt(8) - rng.nextInt(8);
            y_off = rng.nextInt(4) - rng.nextInt(4);
            z_off = rng.nextInt(8) - rng.nextInt(8);
        } else {
            z_off = rng.nextInt(8) - rng.nextInt(8);
            y_off = rng.nextInt(4) - rng.nextInt(4);
            x_off = rng.nextInt(8) - rng.nextInt(8);
        }
        Pos3D blockPos = position.add(x_off, y_off, z_off);

        if (worldIn->isAirBlock(blockPos) && BlockBush::canBlockStay(worldIn, blockPos)) {
            worldIn->setBlock(blockPos, this->flowerId, static_cast<int>(this->flowerType) == 0
                ? 0 : static_cast<int>(this->flowerType) - 1);
        }
    }
    return true;
}
