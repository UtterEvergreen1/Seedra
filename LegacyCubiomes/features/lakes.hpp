#pragma once

#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"

#include "LegacyCubiomes/utils/pos3D.hpp"

namespace Features {
    Pos3D waterLake(Generator* g, int chunkX, int chunkZ) {
        uint64_t rng = getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if (biomeAt == desert || biomeAt == desert_hills || nextInt(&rng, 4) != 0)
            return {0, 0, 0};

        int xPos = (chunkX << 4) + nextInt(&rng, 16) + 8;
        int yPos = nextInt(&rng, 128);
        int zPos = (chunkZ << 4) + nextInt(&rng, 16) + 8;
        return {xPos, yPos, zPos};
    }

    Pos3D lavaLake(Generator* g, int chunkX, int chunkZ){
        uint64_t rng = getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if ((biomeAt != desert && biomeAt != desert_hills) && nextInt(&rng, 4) == 0) // water lake in chunk
            return {0, 0, 0};

        if (nextInt(&rng, 8) != 0) // no lava lake in chunk
            return {0, 0, 0};

        int xPos = (chunkX << 4) + nextInt(&rng, 16) + 8;
        int yPos = nextInt(&rng, nextInt(&rng, 120) + 8);
        int zPos = (chunkZ << 4) + nextInt(&rng, 16) + 8;
        if (yPos >= 63 && nextInt(&rng, 10) != 0) // no lava lake above sea level
            return {0, 0, 0};

        return {xPos, yPos, zPos};
    }
}
