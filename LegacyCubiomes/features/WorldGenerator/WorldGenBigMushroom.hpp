#pragma once
#include "WorldGenerator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class BlockHugeMushroom {
public:
    enum class EnumType {
        ALL_INSIDE,
        NORTH_WEST,
        NORTH,
        NORTH_EAST,
        WEST,
        CENTER,
        EAST,
        SOUTH_WEST,
        SOUTH,
        SOUTH_EAST,
        STEM,
        ALL_OUTSIDE = 14,
        ALL_STEM
    };
};

class WorldGenBigMushroom final : public WorldGenerator {
public:
    explicit WorldGenBigMushroom(lce::blocks::Block* mushroomType = nullptr) : mushroomType(mushroomType) {}

    bool generate(ChunkPrimer* worldIn, RNG& rand, const Pos3D& position) const override;

private:
    lce::blocks::Block* mushroomType;
};