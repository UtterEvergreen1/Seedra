#pragma once

#include "base_rolls.hpp"
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/structures/generation/mineshaft/GenMineshaft.hpp"


using namespace mineshaft_generator;

namespace structure_rolls {

    class MineshaftRolls : public BaseRolls {
    public:
        std::vector<std::pair<Pos3D, int64_t>> mineshaftChests;
        void generateStructure(MineshaftGenerator* mineshaftGenerator, ChunkPrimer *chunk, uint64_t *random, int chunkX, int chunkZ);
        /// Generate all chests for a given mineshaft
        MU void generateAllChests(MineshaftGenerator* mineshaftGenerator, const Generator& g, bool generateFullChunk);
        void generateChest(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const Piece &piece, uint64_t *random, int x, int y, int z);
        static void placeCobWeb(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const Piece &piece, uint64_t *random, int x, int z);
    };

}
