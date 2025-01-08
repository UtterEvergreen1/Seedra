#pragma once

#include "WorldGenerator.hpp"


class World;

class BlockDoublePlant {
public:
    enum class EnumPlantType {
        SUNFLOWER,
        SYRINGA,
        GRASS,
        FERN,
        ROSE,
        PAEONIA
    };

    static bool canPlaceBlockAt(World* chunk, const Pos3D& pos);
    static void placeAt(World *chunk, const Pos3D& lowerPos, EnumPlantType variant);
};

class WorldGenDoublePlant final : public WorldGenerator {
    mutable BlockDoublePlant::EnumPlantType plantType = BlockDoublePlant::EnumPlantType::SUNFLOWER;

public:
    void setPlantType(const BlockDoublePlant::EnumPlantType type) const {
        this->plantType = type;
    }

    bool generate(World * worldIn, RNG& rng, const Pos3D& pos) const override;
};
