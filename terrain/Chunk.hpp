#pragma once

#include "ChunkPrimer.hpp"
#include "World.hpp"



namespace Chunk {

    MU static bool GENERATE_VILLAGES = true;
    MU static bool GENERATE_MINESHAFTS = true;
    MU static bool GENERATE_STRONGHOLDS = true;


    MU extern ChunkPrimer *provideChunk(const Generator &g, Pos2D chunkPos);

    MU extern void populateChunk(World& world, Pos2D chunkPos);

    MU extern void populateCaves(World& world, Pos2D chunkPos);

    MU extern void populateLight(World& world, Pos2D chunkPos);

    MU extern void populateStructures(World& world, Pos2D chunkPos);

    MU extern void populateDecorations(World& world, Pos2D chunkPos);

}

