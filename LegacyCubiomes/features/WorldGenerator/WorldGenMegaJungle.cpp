#include "WorldGenMegaJungle.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"
#include "LegacyCubiomes/utils/constants.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"


bool WorldGenMegaJungle::generate(World *worldIn, RNG &rng, const Pos3D &pos) const {
    const int height = this->getHeight(rng);

    if (!ensureGrowable(worldIn, rng, pos, height)) {
        return false;
    }

    this->createCrown(worldIn, pos.up(height), 2);

    for (int j = pos.getY() + height - 2 - rng.nextInt(4); j > pos.getY() + height / 2; j -= 2 + rng.nextInt(4)) {
        const float f = rng.nextFloat() * (PI_FLOAT * 2.F);
        int k = pos.getX() + static_cast<int>(0.5F + MathHelper::cos(f) * 4.0F);
        int l = pos.getZ() + static_cast<int>(0.5F + MathHelper::sin(f) * 4.0F);

        for (int i1 = 0; i1 < 5; ++i1) {
            k = pos.getX() + static_cast<int>(1.5F + MathHelper::cos(f) * static_cast<float>(i1));
            l = pos.getZ() + static_cast<int>(1.5F + MathHelper::sin(f) * static_cast<float>(i1));
            worldIn->setBlock(k, j - 3 + i1 / 2, l, this->woodMetadata);
        }

        const int j2 = 1 + rng.nextInt(2);
        const int j1 = j;

        for (int k1 = j - j2; k1 <= j1; ++k1) {
            const int l1 = k1 - j1;
            this->growLeavesLayer(worldIn, Pos3D(k, k1, l), 1 - l1);
        }
    }

    for (int i2 = 0; i2 < height; ++i2) {
        Pos3D blockpos = pos.up(i2);

        if (canGrowInto(worldIn->getBlockId(blockpos))) {
            worldIn->setBlock(blockpos, this->woodMetadata);

            if (i2 > 0) {
                placeVine(worldIn, rng, blockpos.west(), enumFacing::EAST);
                placeVine(worldIn, rng, blockpos.north(), enumFacing::SOUTH);
            }
        }

        if (i2 < height - 1) {
            Pos3D blockpos1 = blockpos.east();

            if (canGrowInto(worldIn->getBlockId(blockpos1))) {
                worldIn->setBlock(blockpos1, this->woodMetadata);

                if (i2 > 0) {
                    placeVine(worldIn, rng, blockpos1.east(), enumFacing::WEST);
                    placeVine(worldIn, rng, blockpos1.north(), enumFacing::SOUTH);
                }
            }

            Pos3D blockpos2 = blockpos.south().east();

            if (canGrowInto(worldIn->getBlockId(blockpos2))) {
                worldIn->setBlock(blockpos2, this->woodMetadata);

                if (i2 > 0) {
                    placeVine(worldIn, rng, blockpos2.east(), enumFacing::WEST);
                    placeVine(worldIn, rng, blockpos2.south(), enumFacing::NORTH);
                }
            }

            Pos3D blockpos3 = blockpos.south();

            if (canGrowInto(worldIn->getBlockId(blockpos3))) {
                worldIn->setBlock(blockpos3, this->woodMetadata);

                if (i2 > 0) {
                    placeVine(worldIn, rng, blockpos3.west(), enumFacing::EAST);
                    placeVine(worldIn, rng, blockpos3.south(), enumFacing::NORTH);
                }
            }
        }
    }

    return true;
}

void WorldGenMegaJungle::placeVine(World *worldIn, RNG &rand, const Pos3D &pos, enumFacing direction) {
    if (rand.nextInt(3) > 0 && worldIn->isAirBlock(pos)) {
        worldIn->setBlock(pos, &lce::blocks::BlocksInit::VINES);
    }
}

void WorldGenMegaJungle::createCrown(World *worldIn, const Pos3D &pos, const int width) const {
    for (int j = -2; j <= 0; ++j) {
        this->growLeavesLayerStrict(worldIn, pos.up(j), width + 1 - j);
    }
}
