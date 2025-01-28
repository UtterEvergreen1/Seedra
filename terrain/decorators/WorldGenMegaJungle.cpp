#include "WorldGenMegaJungle.hpp"

#include "lce/blocks/__include.hpp"
#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "terrain/World.hpp"


bool WorldGenMegaJungle::generate(World *worldIn, RNG &rng, const Pos3D &pos) const {
    c_int height = this->getHeight(rng);

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

        c_int j2 = 1 + rng.nextInt(2);
        c_int j1 = j;

        for (int k1 = j - j2; k1 <= j1; ++k1) {
            c_int l1 = k1 - j1;
            this->growLeavesLayer(worldIn, Pos3D(k, k1, l), 1 - l1);
        }
    }

    for (int i2 = 0; i2 < height; ++i2) {
        Pos3D blockPos = pos.up(i2);

        if (canGrowInto(worldIn->getBlockId(blockPos))) {
            worldIn->setBlock(blockPos, this->woodMetadata);

            if (i2 > 0) {
                placeVine(worldIn, rng, blockPos.west(), EnumFacing::EAST);
                placeVine(worldIn, rng, blockPos.north(), EnumFacing::SOUTH);
            }
        }

        if (i2 < height - 1) {
            Pos3D blockPos1 = blockPos.east();

            if (canGrowInto(worldIn->getBlockId(blockPos1))) {
                worldIn->setBlock(blockPos1, this->woodMetadata);

                if (i2 > 0) {
                    placeVine(worldIn, rng, blockPos1.east(), EnumFacing::WEST);
                    placeVine(worldIn, rng, blockPos1.north(), EnumFacing::SOUTH);
                }
            }

            Pos3D blockPos2 = blockPos.south().east();

            if (canGrowInto(worldIn->getBlockId(blockPos2))) {
                worldIn->setBlock(blockPos2, this->woodMetadata);

                if (i2 > 0) {
                    placeVine(worldIn, rng, blockPos2.east(), EnumFacing::WEST);
                    placeVine(worldIn, rng, blockPos2.south(), EnumFacing::NORTH);
                }
            }

            Pos3D blockPos3 = blockPos.south();

            if (canGrowInto(worldIn->getBlockId(blockPos3))) {
                worldIn->setBlock(blockPos3, this->woodMetadata);

                if (i2 > 0) {
                    placeVine(worldIn, rng, blockPos3.west(), EnumFacing::EAST);
                    placeVine(worldIn, rng, blockPos3.south(), EnumFacing::NORTH);
                }
            }
        }
    }

    return true;
}

void WorldGenMegaJungle::placeVine(World *worldIn, RNG &rand, const Pos3D &pos, MU EnumFacing facing) {
    if (rand.nextInt(3) > 0 && worldIn->isAirBlock(pos)) {
        // TODO: set correct vine metadata
        worldIn->setBlock(pos, lce::BlocksInit::VINES.getState());
    }
}

void WorldGenMegaJungle::createCrown(World *worldIn, const Pos3D &pos, c_int width) const {
    for (int j = -2; j <= 0; ++j) {
        this->growLeavesLayerStrict(worldIn, pos.up(j), width + 1 - j);
    }
}
