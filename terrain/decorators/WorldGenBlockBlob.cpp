#include "WorldGenBlockBlob.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool WorldGenBlockBlob::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    Pos3D position = pos;
    const int rad = this->startRadius;
    while (true) {
        if (position.getY() > 3) {
            if (worldIn->isAirBlock(position.down())) { goto label50; }

            const int blockId = worldIn->getBlockId(position.down());

            if (blockId != GRASS_ID && blockId != DIRT_ID && blockId != STONE_ID) { goto label50; }
        }

        if (position.getY() <= 3) { return false; }

        if (rad < 0) { return false; }

        for (int i = 0; i < 3; ++i) {

            const Pos3D jkl = getWorldGenPos3D<2, 2, 2>(worldIn, rng) + rad;

            const float f = (float) (jkl.x + jkl.y + jkl.z) * 0.333F + 0.5F;

            for (const Pos3D& blockPos: Pos3D::getAllInBox(position - jkl, position + jkl)) {
                if (blockPos.distanceSq(position) <= (double) (f * f)) { worldIn->setBlock(blockPos, this->block); }
            }

            const Pos3D offset = getWorldGenPos3D(worldIn, rng, 2 + rad * 2, 2, 2 + rad * 2);
            position = position.add(-(rad + 1) + offset.x, 0 - offset.y, -(rad + 1) + offset.z);
        }

        return true;
    label50:
        position = position.down();
    }
}
