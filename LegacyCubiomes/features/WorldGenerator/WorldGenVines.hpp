#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class BlockVine {
public:
    static bool canPlaceBlockOnSide(const ChunkPrimer* worldIn, const Pos3D& pos, const FACING& facing);

private:
    static bool canBlockStay(int blockId);
};

class WorldGenVines final : public WorldGenerator {
public:
    bool generate(ChunkPrimer* worldIn, RNG& rng, const Pos3D& position) const override;
};