#include "WorldGenIceSpike.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


bool WorldGenIceSpike::generate(World * worldIn, RNG &rng, const Pos3D &pos) const {
    Pos3D position = pos;
    while (worldIn->isAirBlock(position) && position.getY() > 2) {
        position = position.down();
    }

    if (worldIn->getBlockId(position) != lce::blocks::ids::SNOW_BLOCK_ID) {
        return false;
    }

    position = position.up(rng.nextInt(4));
    const int i = rng.nextInt(4) + 7;
    const int j = i / 4 + rng.nextInt(2);

    if (j > 1 && rng.nextInt(60) == 0) {
        position = position.up(10 + rng.nextInt(30));
    }

    for (int k = 0; k < i; ++k) {
        const float f = (1.0F - (float) k / (float) i) * (float) j;
        const int l = std::ceil(f);

        for (int i1 = -l; i1 <= l; ++i1) {
            float f1 = static_cast<float>(std::abs(i1)) - 0.25F;

            for (int j1 = -l; j1 <= l; ++j1) {
                float f2 = static_cast<float>(std::abs(j1)) - 0.25F;

                if ((i1 == 0 && j1 == 0 || f1 * f1 + f2 * f2 <= f * f) && (
                        i1 != -l && i1 != l && j1 != -l && j1 != l || rng.nextFloat() <= 0.75F)) {
                    const int blockId = worldIn->getBlockId(position.add(i1, k, j1));

                    using namespace lce::blocks;
                    if (blockId == ids::AIR_ID ||
                        blockId == ids::DIRT_ID ||
                        blockId == ids::SNOW_BLOCK_ID || blockId == ids::ICE_ID) {
                        worldIn->setBlock(position.add(i1, k, j1), ids::PACKED_ICE_ID);
                    }

                    if (k != 0 && l > 1) {
                        const int blockId1 = worldIn->getBlockId(position.add(i1, -k, j1));

                        if (blockId1 == ids::AIR_ID ||
                            blockId1 == ids::DIRT_ID ||
                            blockId1 == ids::SNOW_BLOCK_ID ||
                            blockId1 == ids::ICE_ID) {
                            worldIn->setBlock(position.add(i1, -k, j1), ids::PACKED_ICE_ID);
                        }
                    }
                }
            }
        }
    }

    int k1 = j - 1;

    if (k1 < 0) {
        k1 = 0;
    } else if (k1 > 1) {
        k1 = 1;
    }

    for (int l1 = -k1; l1 <= k1; ++l1) {
        for (int i2 = -k1; i2 <= k1; ++i2) {
            Pos3D blockPos = position.add(l1, -1, i2);
            int j2 = 50;

            if (std::abs(l1) == 1 && std::abs(i2) == 1) {
                j2 = rng.nextInt(5);
            }

            while (blockPos.getY() > 50) {
                const int blockId = worldIn->getBlockId(blockPos);

                using namespace lce::blocks;
                if (blockId != ids::AIR_ID &&
                    blockId != ids::DIRT_ID &&
                    blockId != ids::SNOW_BLOCK_ID &&
                    blockId != ids::ICE_ID &&
                    blockId != ids::PACKED_ICE_ID) {
                    break;
                }

                worldIn->setBlock(blockPos, ids::PACKED_ICE_ID);
                blockPos = blockPos.down();
                --j2;

                if (j2 <= 0) {
                    blockPos = blockPos.down(rng.nextInt(5) + 1);
                    j2 = rng.nextInt(5);
                }
            }
        }
    }

    return true;
}
