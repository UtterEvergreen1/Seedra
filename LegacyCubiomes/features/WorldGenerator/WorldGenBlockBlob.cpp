#include "WorldGenBlockBlob.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenBlockBlob::generate(World* worldIn, RNG& rng, const Pos3D& pos) const {
    Pos3D position = pos;
    const int rad = this->startRadius;
    while (true) {
        if (position.getY() > 3) {
            if (worldIn->isAirBlock(position.down())) { goto label50; }

            const int blockId = worldIn->getBlockId(position.down());

            using namespace lce::blocks;
            if (blockId != ids::GRASS_ID && blockId != ids::DIRT_ID && blockId != ids::STONE_ID) { goto label50; }
        }

        if (position.getY() <= 3) { return false; }

        if (rad < 0) { return false; }

        for (int i = 0; i < 3; ++i) {
            const int j = rad + rng.nextInt(2);
            const int k = rad + rng.nextInt(2);
            const int l = rad + rng.nextInt(2);
            const float f = (float) (j + k + l) * 0.333F + 0.5F;

            for (const Pos3D& blockPos: Pos3D::getAllInBox(position.add(-j, -k, -l), position.add(j, k, l))) {
                if (blockPos.distanceSq(position) <= (double) (f * f)) { worldIn->setBlock(blockPos, this->block); }
            }

            position = position.add(-(rad + 1) + rng.nextInt(2 + rad * 2), 0 - rng.nextInt(2),
                                    -(rad + 1) + rng.nextInt(2 + rad * 2));
        }

        return true;
    label50:
        position = position.down();
    }
}
