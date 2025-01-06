#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"

namespace structure_rolls {
    class RollsBase {
    public:
        static bool isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                   const BoundingBox& pieceBoundingBox, ChunkPrimer* chunk);
        static bool intersectsWithBlock(const BoundingBox& chunkBoundingBox, int x, int y, int z);
        MU static bool validToPlace(const BoundingBox& chunkBoundingBox, const BoundingBox& bb, int x, int y, int z);
        static void fillWithRandomizedBlocks(const BoundingBox& chunkBoundingBox, const Piece& piece, int minX,
                                             int minY, int minZ, int maxX, int maxY, int maxZ, RNG& rng,
                                             ChunkPrimer* chunk);
        static void generateChest(const BoundingBox& chunkBB, const Piece& piece, RNG& rng, int x, int y, int z);


        // added for village
        static void fillWithBlocks(World& worldIn, const BoundingBox& boundingBoxIn,
                                   int xMin, int yMin, int zMin, int xMax, int yMax, int zMax,
                                   const lce::blocks::Block* boundaryBlockState,
                                   const lce::blocks::Block* insideBlockState);




    };

} // namespace structure_rolls
