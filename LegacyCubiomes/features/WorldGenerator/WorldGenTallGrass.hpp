#pragma once
#include "WorldGenerator.hpp"
#include "WorldGenBush.hpp"
#include "lce/blocks/blocks.hpp"

class BlockTallGrass : public BlockBush {
public:
    enum class EnumType {
        DEAD_BUSH,
        FERN,
        GRASS,
        TALL_GRASS
    };

    static const lce::blocks::Block* getBlockVarientById(EnumType type) {
        switch (type) {
            case EnumType::DEAD_BUSH:
                return &lce::blocks::BlocksInit::DEAD_BUSH;
            case EnumType::FERN:
                return &lce::blocks::BlocksInit::TALL_GRASS_FERN;
            case EnumType::GRASS:
                return &lce::blocks::BlocksInit::TALL_GRASS_GRASS;
            case EnumType::TALL_GRASS:
                return &lce::blocks::BlocksInit::DOUBLE_TALLGRASS;
        }
    }

    static bool canBlockStay(const World *worldIn, const Pos3D &pos);
};


class WorldGenTallGrass : public WorldGenerator {
public:
    explicit WorldGenTallGrass(const BlockTallGrass::EnumType typeIn) : type(typeIn) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    BlockTallGrass::EnumType type;
};