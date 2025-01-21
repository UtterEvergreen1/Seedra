#pragma once

#include "AbstractWorldGenerator.hpp"


class World;

class WorldGenDungeons final : public AbstractWorldGenerator {
    static std::string SPAWNER_TYPES[];
    static void pickMobSpawner(RNG& rng);

public:
    bool generate(World* worldIn, RNG& rng, const Pos3D& position) const override;
};