#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockLilyPad {
public:
    static bool canPlaceBlockAt(World* worldIn, const Pos3D& pos);

private:
    static bool canSustainBush(int blockId);
};

class WorldGenWaterlily final : public AbstractWorldGenerator {
public:
    bool generate(World * worldIn, RNG& rng, const Pos3D& position) const override;
};