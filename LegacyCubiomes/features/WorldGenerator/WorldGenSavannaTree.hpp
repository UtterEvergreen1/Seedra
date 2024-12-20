#pragma once
#include "WorldGenAbstractTree.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class WorldGenSavannaTree final : public WorldGenAbstractTree {
public:
    bool generate(World * worldIn, RNG &rand, const Pos3D &position) const override;

private:
    static void placeLogAt(World *worldIn, const Pos3D &pos) ;

    static void placeLeafAt(World *worldIn, const Pos3D &pos) ;
};
