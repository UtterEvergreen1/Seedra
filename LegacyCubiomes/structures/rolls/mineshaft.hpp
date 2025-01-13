#pragma once

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/structures/gen/mineshaft/mineshaft.hpp"


namespace rolls {


    class MU Mineshaft {
    public:
        std::vector<std::pair<Pos3D, i64>> mineshaftChests{};
        void generateStructure(const gen::Mineshaft* mg, ChunkPrimer* chunk, RNG& rng, int chunkX, int chunkZ);

        /// Generate all chests for a given mineshaft
        MU void generateAllChests(World& world, const gen::Mineshaft* mg, const Generator& g, bool generateFullChunk);

        void generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                           const StructureComponent& piece, RNG& rng, int x, int y, int z);

    private:
        static void placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                const StructureComponent& piece, RNG& rng, int x, int z);
    };

}