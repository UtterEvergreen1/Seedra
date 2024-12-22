#include "WorldGenShrub.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

bool WorldGenShrub::generate(World * worldIn, RNG &rng, const Pos3D &position) const {
    Pos3D pos = position;
    while ((worldIn->getBlockId(pos) == lce::blocks::ids::AIR_ID ||
            lce::blocks::ids::isLeavesBlock(worldIn->getBlockId(pos))) && pos.getY() > 0) {
        pos = pos.down();
    }

    int block = worldIn->getBlockId(pos);
    if (block == lce::blocks::ids::DIRT_ID || block == lce::blocks::ids::GRASS_ID) {
        pos = pos.up();
        worldIn->setBlock(pos, this->woodMetadata);

        for (int y = pos.getY(); y <= pos.getY() + 2; ++y) {
            int y_off = y - pos.getY();
            int radius = 2 - y_off;

            for (int x = pos.getX() - radius; x <= pos.getX() + radius; ++x) {
                int x_off = x - pos.getX();

                for (int z = pos.getZ() - radius; z <= pos.getZ() + radius; ++z) {
                    int z_off = z - pos.getZ();

                    if (std::abs(x_off) != radius || std::abs(z_off) != radius || rng.nextInt(2) != 0) {
                        Pos3D blockpos(x, y, z);
                        int material = worldIn->getBlockId(blockpos);

                        if (material == lce::blocks::ids::AIR_ID || lce::blocks::ids::isLeavesBlock(material)) {
                            worldIn->setBlock(blockpos, this->leavesMetadata);
                        }
                    }
                }
            }
        }
    }

    return true;
}
