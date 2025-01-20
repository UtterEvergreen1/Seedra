#pragma once

#include "World.hpp"
#include "CaveGenerator.hpp"
#include "ChunkGenerator.hpp"
#include "ChunkPrimer.hpp"
#include "RavineGenerator.hpp"
#include "WaterCaveGenerator.hpp"
#include "WaterRavineGenerator.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenDungeons.hpp"

namespace Chunk {

    static bool GENERATE_VILLAGES = true;
    static bool GENERATE_MINESHAFTS = true;
    static bool GENERATE_STRONGHOLDS = true;

    template<bool checkWaterCaves = false,
        bool generateCaves = true,
        bool generateRavines = true,
        bool generateSkyLight = false>
    ChunkPrimer *provideChunk(const Generator &g, c_int chunkX, c_int chunkZ,  bool accurate = true);

    MU extern void populateChunk(World& world, const Generator &g, int chunkX, int chunkZ);

    MU extern void populateStructures(World& world, const Generator& g, int chunkX, int chunkZ);

    MU extern void populateDecorations(World& world, Generator &g, int chunkX, int chunkZ);

}

