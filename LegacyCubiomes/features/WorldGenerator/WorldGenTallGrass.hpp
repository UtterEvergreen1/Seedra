#pragma once
#include "WorldGenerator.hpp"
#include "WorldGenBush.hpp"

class BlockTallGrass : public BlockBush {
public:
    enum class EnumType {
        DEAD_BUSH,
        FERN,
        GRASS,
        TALL_GRASS
    };

    static bool canBlockStay(const ChunkPrimer *worldIn, const Pos3D &pos);
};


class WorldGenTallGrass : public WorldGenerator {
public:
    explicit WorldGenTallGrass(const BlockTallGrass::EnumType typeIn) : type(typeIn) {
    }

    bool generate(ChunkPrimer *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    BlockTallGrass::EnumType type;
};