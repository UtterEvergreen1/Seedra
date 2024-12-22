#include "WorldGenMinable.hpp"

#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/constants.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

bool WorldGenMinable::generate(World *world, RNG &rng, const Pos3D &pos) const {
    float f = rng.nextFloat() * PI;
    float f2 = (float) this->blockCount / 8.0F;
    double d0 = (double) ((float) (pos.getX() + 8) + MathHelper::sin(f) * f2);
    double d1 = (double) ((float) (pos.getX() + 8) - MathHelper::sin(f) * f2);
    double d2 = (double) ((float) (pos.getZ() + 8) + MathHelper::cos(f) * f2);
    double d3 = (double) ((float) (pos.getZ() + 8) - MathHelper::cos(f) * f2);
    double d4 = (double) (pos.getY() + rng.nextInt(3) - 2);
    double d5 = (double) (pos.getY() + rng.nextInt(3) - 2);
    for (int i = 0; i <= this->blockCount; ++i) {
        float f1 = (float) i / (float) this->blockCount;
        double d6 = d0 + (d1 - d0) * (double) f1;
        double d7 = d4 + (d5 - d4) * (double) f1;
        double d8 = d2 + (d3 - d2) * (double) f1;
        double d9 = rng.nextDouble() * (double) this->blockCount / 16.0;
        double d10 = (double) ((MathHelper::sin(PI_FLOAT * f1) + 1.0F) * d9 + 1.0) / 2.0;
        int j = MathHelper::floor(d6 - d10);
        int k = MathHelper::floor(d7 - d10);
        int l = MathHelper::floor(d8 - d10);
        int i1 = MathHelper::floor(d6 + d10);
        int j1 = MathHelper::floor(d7 + d10);
        int k1 = MathHelper::floor(d8 + d10);

        for (int l1 = j; l1 <= i1; ++l1) {
            double d12 = ((double) l1 + 0.5 - d6) / d10;

            if (d12 * d12 < 1.0) {
                for (int i2 = k; i2 <= j1; ++i2) {
                    double d13 = ((double) i2 + 0.5 - d7) / d10;

                    if (d12 * d12 + d13 * d13 < 1.0) {
                        for (int j2 = l; j2 <= k1; ++j2) {
                            double d14 = ((double) j2 + 0.5 - d8) / d10;

                            if (d12 * d12 + d13 * d13 + d14 * d14 < 1.0) {
                                Pos3D blockpos(l1, i2, j2);
                                const lce::blocks::Block *block = world->getBlock(blockpos);
                                if (block == &lce::blocks::BlocksInit::STONE || block == &
                                    lce::blocks::BlocksInit::GRANITE || block == &lce::blocks::BlocksInit::DIORITE ||
                                    block == &lce::blocks::BlocksInit::ANDESITE) {
                                    world->setBlock(blockpos, this->oreBlock);
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
