#pragma once

#include "ChunkPrimer.hpp"
#include "World.hpp"
#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/carve/ChunkGenerator.hpp"
#include "terrain/carve/RavineGenerator.hpp"
#include "terrain/carve/WaterCaveGenerator.hpp"
#include "terrain/carve/WaterRavineGenerator.hpp"

#include "terrain/decorators/WorldGenDungeons.hpp"
#include "terrain/decorators/WorldGenLakes.hpp"


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

    MU extern void populateDecorations(World& world, const Generator &g, int chunkX, int chunkZ);

}

