#pragma once
#include "WorldGenerator.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class BlockDeadBush {
public:
    static bool canPlaceBlockAt(const World *world, const Pos3D &pos);

    static bool canSustainBush(int blockId);

    static bool canBlockStay(const World *world, const Pos3D &pos);
};

class WorldGenDeadBush final : public WorldGenerator {
public:
    bool generate(World * worldIn, RNG &rand, const Pos3D &position) const override;
};