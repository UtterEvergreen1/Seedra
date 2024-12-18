#pragma once
#include "WorldGenHugeTrees.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenMegaPineTree final : public WorldGenHugeTrees {
    bool useBaseHeight;

public:
    explicit WorldGenMegaPineTree(const bool useBaseHeight)
        : WorldGenHugeTrees(13, 15, &lce::blocks::SPRUCE_WOOD, &lce::blocks::SPRUCE_LEAVES),
          useBaseHeight(useBaseHeight) {
    }

    bool generate(ChunkPrimer *worldIn, RNG &rand, const Pos3D &position) const override;

    void generateSaplings(ChunkPrimer *worldIn, RNG &random, const Pos3D &pos) const override;

private:
    void createCrown(ChunkPrimer *worldIn, int x, int z, int y, int p_150541_5_, RNG &rand) const;

    static void placePodzolCircle(ChunkPrimer *worldIn, const Pos3D &center) ;

    static void placePodzolAt(ChunkPrimer *worldIn, const Pos3D &pos) ;
};
