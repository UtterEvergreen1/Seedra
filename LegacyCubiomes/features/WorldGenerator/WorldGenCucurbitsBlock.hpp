#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenCucurbitsBlock final : public WorldGenerator {
public:
    int blockId;
    bool hasRotation;

    explicit WorldGenCucurbitsBlock(const int block, const bool has_rotation = false)
        : blockId(block), hasRotation(has_rotation) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    static int getRandomRotationMetaData(RNG &rng);
};
