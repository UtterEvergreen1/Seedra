#include "WorldGenShrub.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenShrub::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    Pos3D pos = position;
    while (isAirOrLeavesBlock(worldIn->getBlockId(pos)) && pos.getY() > 0) {
        pos = pos.down();
    }

    c_int block = worldIn->getBlockId(pos);
    if (block == DIRT_ID || block == GRASS_ID) {
        pos = pos.up();
        worldIn->setBlock(pos, this->woodMetadata);

        for (int y = pos.getY(); y <= pos.getY() + 2; ++y) {
            c_int y_off = y - pos.getY();
            int radius = 2 - y_off;

            for (int x = pos.getX() - radius; x <= pos.getX() + radius; ++x) {
                int x_off = x - pos.getX();

                for (int z = pos.getZ() - radius; z <= pos.getZ() + radius; ++z) {
                    int z_off = z - pos.getZ();

                    if (std::abs(x_off) != radius || std::abs(z_off) != radius || rng.nextInt(2) != 0) {
                        Pos3D blockPos(x, y, z);
                        c_int material = worldIn->getBlockId(blockPos);

                        if (lce::blocks::isAirOrLeavesBlock(material)) {
                            worldIn->setBlock(blockPos, this->leavesMetadata);
                        }
                    }
                }
            }
        }
    }

    return true;
}
