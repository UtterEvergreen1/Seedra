#pragma once

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/structures/rolls/rolls_base.hpp"
#include "LegacyCubiomes/structures/generation/mineshaft/mineshaft.hpp"




namespace structure_rolls {

    class Mineshaft : public RollsBase {
    public:
        std::vector<std::pair<Pos3D, int64_t>> mineshaftChests;
        void generateStructure(generation::Mineshaft* mineshaftGenerator, ChunkPrimer *chunk, uint64_t *random, int chunkX, int chunkZ);
        /// Generate all chests for a given mineshaft
        MU void generateAllChests(generation::Mineshaft* mineshaftGenerator, const Generator& g, bool generateFullChunk);
        void generateChest(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const Piece &piece, uint64_t *random, int x, int y, int z);
        static void placeCobWeb(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const Piece &piece, uint64_t *random, int x, int z);
    };

}
