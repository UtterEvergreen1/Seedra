#pragma once
#include "WorldGenerator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
class WorldGenDungeons final : public WorldGenerator {
    static std::string SPAWNERTYPES[];
    void pickMobSpawner(RNG &rng) const;
public:
    bool generate(World* worldIn, RNG& rng, const Pos3D& position) const override;
};