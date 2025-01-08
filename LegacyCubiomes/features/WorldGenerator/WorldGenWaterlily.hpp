#pragma once

#include "WorldGenerator.hpp"


class World;

class BlockLilyPad {
public:
    static bool canPlaceBlockAt(World* worldIn, const Pos3D& pos);

private:
    static bool canSustainBush(int blockId);
};

class WorldGenWaterlily final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& position) const override;
};