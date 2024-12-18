#pragma once
#include "WorldGenBush.hpp"
#include "WorldGenerator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
class ChunkPrimer;

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

    static bool canPlaceBlockAt(const ChunkPrimer* chunk, const Pos3D& pos);
    static void placeAt(ChunkPrimer* chunk, const Pos3D& lowerPos, EnumPlantType variant);
};
class WorldGenDoublePlant final : public WorldGenerator {
    mutable BlockDoublePlant::EnumPlantType plantType = BlockDoublePlant::EnumPlantType::SUNFLOWER;

public:
    void setPlantType(const BlockDoublePlant::EnumPlantType type) const {
        this->plantType = type;
    }

    bool generate(ChunkPrimer* worldIn, RNG& rng, const Pos3D& pos) const override;
};
