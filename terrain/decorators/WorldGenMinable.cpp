#include "WorldGenMinable.hpp"

#include "lce/blocks/__include.hpp"
#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "terrain/World.hpp"


bool WorldGenMinable::generate(World *world, RNG &rng, const Pos3D &pos) const {
    // TODO: jerrin changed PI -> PI_FLOAT to silence warning
    const float f = rng.nextFloat() * PI_FLOAT;
    const float f2 = static_cast<float>(this->blockCount) / 8.0F;
    const auto d0 = (double) ((float) (pos.getX() + 8) + MathHelper::sin(f) * f2);
    const auto d1 = (double) ((float) (pos.getX() + 8) - MathHelper::sin(f) * f2);
    const auto d2 = (double) ((float) (pos.getZ() + 8) + MathHelper::cos(f) * f2);
    const auto d3 = (double) ((float) (pos.getZ() + 8) - MathHelper::cos(f) * f2);
    const auto d4 = (double) (pos.getY() + rng.nextInt(3) - 2);
    const auto d5 = (double) (pos.getY() + rng.nextInt(3) - 2);
    for (int i = 0; i <= this->blockCount; ++i) {
        const float f1 = (float) i / (float) this->blockCount;
        const double d6 = d0 + (d1 - d0) * (double) f1;
        const double d7 = d4 + (d5 - d4) * (double) f1;
        const double d8 = d2 + (d3 - d2) * (double) f1;
        const double d9 = rng.nextDouble() * (double) this->blockCount / 16.0;
        const double d10 = ((MathHelper::sin(PI_FLOAT * f1) + 1.0F) * d9 + 1.0) / 2.0;
        const int j = MathHelper::floor(d6 - d10);
        const int k = MathHelper::floor(d7 - d10);
        const int l = MathHelper::floor(d8 - d10);
        const int i1 = MathHelper::floor(d6 + d10);
        const int j1 = MathHelper::floor(d7 + d10);
        const int k1 = MathHelper::floor(d8 + d10);

        for (int l1 = j; l1 <= i1; ++l1) {
            const double d12 = (static_cast<double>(l1) + 0.5 - d6) / d10;

            if (d12 * d12 < 1.0) {
                for (int i2 = k; i2 <= j1; ++i2) {
                    const double d13 = (static_cast<double>(i2) + 0.5 - d7) / d10;

                    if (d12 * d12 + d13 * d13 < 1.0) {
                        for (int j2 = l; j2 <= k1; ++j2) {
                            const double d14 = (static_cast<double>(j2) + 0.5 - d8) / d10;

                            if (d12 * d12 + d13 * d13 + d14 * d14 < 1.0) {
                                Pos3D blockPos(l1, i2, j2);
                                const lce::BlockState block = world->getBlock(blockPos);
                                if (block == lce::BlocksInit::STONE.getState() ||
                                    block == lce::BlocksInit::GRANITE.getState() ||
                                    block == lce::BlocksInit::DIORITE.getState() ||
                                    block == lce::BlocksInit::ANDESITE.getState()) {
                                    world->setBlock(blockPos, this->oreBlock);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}
