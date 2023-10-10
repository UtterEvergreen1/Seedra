#pragma once

#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"

#include "LegacyCubiomes/utils/Pos3D.hpp"

namespace Features {
    Pos3D waterLake(Generator* g, int chunkX, int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if (biomeAt == desert || biomeAt == desert_hills || rng.nextInt(4) != 0)
            return {0, 0, 0};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        return {xPos, yPos, zPos};
    }

    Pos3D lavaLake(Generator* g, int chunkX, int chunkZ){
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if ((biomeAt != desert && biomeAt != desert_hills) && rng.nextInt(4) == 0) // water lake in chunk
            return {0, 0, 0};

        if (rng.nextInt(8) != 0) // no lava lake in chunk
            return {0, 0, 0};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(rng.nextInt(120) + 8);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        if (yPos >= 63 && rng.nextInt(10) != 0) // no lava lake above sea level
            return {0, 0, 0};

        return {xPos, yPos, zPos};
    }
}
