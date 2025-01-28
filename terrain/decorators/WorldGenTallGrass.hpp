#pragma once

#include "WorldGenBush.hpp"


class World;

class BlockTallGrass : public BlockBush {
public:
    enum class EnumType {
        DEAD_BUSH,
        FERN,
        GRASS,
        TALL_GRASS
    };

    static lce::BlockState getBlockVariantById(EnumType type);

    static bool canBlockStay(World *worldIn, const Pos3D &pos);
};


class WorldGenTallGrass final : public AbstractWorldGenerator {
public:
    explicit WorldGenTallGrass(const BlockTallGrass::EnumType typeIn) : type(typeIn) {}

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    BlockTallGrass::EnumType type;
};