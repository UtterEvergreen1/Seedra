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

        for (int i = pos.getY(); i <= pos.getY() + 2; ++i) {
            int j = i - pos.getY();
            int k = 2 - j;

            for (int l = pos.getX() - k; l <= pos.getX() + k; ++l) {
                int i1 = l - pos.getX();

                for (int j1 = pos.getZ() - k; j1 <= pos.getZ() + k; ++j1) {
                    int k1 = j1 - pos.getZ();

                    if (std::abs(i1) != k || std::abs(k1) != k || rng.nextInt(2) != 0) {
                        Pos3D blockpos(l, i, j1);
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
