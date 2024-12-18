#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class BlockLilyPad {
public:
    static bool canPlaceBlockAt(const ChunkPrimer* worldIn, const Pos3D& pos);

private:
    static bool canSustainBush(int blockId);
};

class WorldGenWaterlily final : public WorldGenerator {
public:
    bool generate(ChunkPrimer* worldIn, RNG& rng, const Pos3D& position) const override;
};