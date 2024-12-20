#pragma once
#include "WorldGenerator.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

class ChunkPrimer;

class BlockBush {
public:
    static bool canPlaceBlockAt(const World *world, const Pos3D &pos);

    static bool canSustainBush(int blockId);

    static bool canBlockStay(const World *world, const Pos3D &pos);
};

class WorldGenBush final : public WorldGenerator {
    const lce::blocks::Block *block;

public:
    WorldGenBush(const lce::blocks::Block *blockIn) {
        this->block = blockIn;
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};
