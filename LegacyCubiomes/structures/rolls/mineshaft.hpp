#pragma once

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/structures/generation/mineshaft/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/rolls_base.hpp"


namespace structure_rolls {


    class Mineshaft : public RollsBase {
    public:
        std::vector<std::pair<Pos3D, i64>> mineshaftChests;
        void generateStructure(const generation::Mineshaft* mineshaftGenerator, ChunkPrimer* chunk,
                               RNG& rng, int chunkX, int chunkZ);

        /// Generate all chests for a given mineshaft
        MU void generateAllChests(generation::Mineshaft* mineshaftGenerator,
                                  const Generator& g, bool generateFullChunk);

        void generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                           const Piece& piece, RNG& rng, int x, int y, int z);

    private:
        static void placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                const Piece& piece, RNG& rng, int x, int z);
    };

}