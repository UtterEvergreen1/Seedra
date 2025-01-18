#pragma once

#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"

#include "lce/enums.hpp"



class WorldGenerator {
public:
    virtual ~WorldGenerator() = default;

    virtual bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const = 0;

    /// if "yRange" is 0, it will not do Y-rng rolls
    template<int xRange, int yRange, int zRange>
    MU static Pos3D getWorldGenPos3D(World* worldIn, RNG& rng) {
        Pos3D pos;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 &&
            worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
            if constexpr (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
        } else {
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
            if constexpr (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
        }
        return pos;
    }

    MU static Pos3D getWorldGenPos3D(World* worldIn, RNG& rng, c_int xRange, c_int yRange, c_int zRange) {
        Pos3D pos;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 &&
            worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
            if (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
        } else {
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
            if (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
        }
        return pos;
    }
};
