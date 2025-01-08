#include "WorldGenShrub.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenShrub::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    Pos3D pos = position;
    while (lce::blocks::ids::isAirOrLeavesBlock(worldIn->getBlockId(pos)) && pos.getY() > 0) {
        pos = pos.down();
    }

    const int block = worldIn->getBlockId(pos);
    if (block == lce::blocks::ids::DIRT_ID || block == lce::blocks::ids::GRASS_ID) {
        pos = pos.up();
        worldIn->setBlock(pos, this->woodMetadata);

        for (int y = pos.getY(); y <= pos.getY() + 2; ++y) {
            const int y_off = y - pos.getY();
            int radius = 2 - y_off;

            for (int x = pos.getX() - radius; x <= pos.getX() + radius; ++x) {
                int x_off = x - pos.getX();

                for (int z = pos.getZ() - radius; z <= pos.getZ() + radius; ++z) {
                    int z_off = z - pos.getZ();

                    if (std::abs(x_off) != radius || std::abs(z_off) != radius || rng.nextInt(2) != 0) {
                        Pos3D blockpos(x, y, z);
                        const int material = worldIn->getBlockId(blockpos);

                        if (lce::blocks::ids::isAirOrLeavesBlock(material)) {
                            worldIn->setBlock(blockpos, this->leavesMetadata);
                        }
                    }
                }
            }
        }
    }

    return true;
}
