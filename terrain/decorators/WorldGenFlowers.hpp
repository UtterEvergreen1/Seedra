#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class BlockFlower {
public:
    enum class EnumFlowerType : int8_t {
        DANDELION,
        POPPY,
        BLUE_ORCHID,
        ALLIUM,
        AZURE_BLUET,
        RED_TULIP,
        ORANGE_TULIP,
        WHITE_TULIP,
        PINK_TULIP,
        OXEYE_DAISY,
        NUM_TYPES
    };
};

class WorldGenFlowers final : public AbstractWorldGenerator {
    int flowerId;
    BlockFlower::EnumFlowerType flowerType;

public:
    WorldGenFlowers(const int flowerId, const BlockFlower::EnumFlowerType type)
        : flowerId(flowerId), flowerType(type) {
    }

    void setGeneratedBlock(const int flowerId, const BlockFlower::EnumFlowerType typeIn) {
        this->flowerId = flowerId;
        this->flowerType = typeIn;
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &position) const override;
};
