#pragma once

#include "LegacyCubiomes/building_blocks/BasePiece.hpp"
#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

namespace structure_rolls {
    class BaseRolls {
    public:
        static bool isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                   const BoundingBox& pieceBoundingBox, ChunkPrimer* chunk);
        static bool intersectsWithBlock(const BoundingBox &chunkBoundingBox, int x, int y, int z);
        static bool validToPlace(const BoundingBox &chunkBoundingBox, const BoundingBox& bb, int x, int y, int z);
        static void fillWithRandomizedBlocks(const BoundingBox &chunkBoundingBox, const BasePiece &piece,
                                             int minX, int minY, int minZ, int maxX, int maxY, int maxZ,
                                             uint64_t *random, ChunkPrimer *chunk);
        static void generateChest(const BoundingBox &chunkBoundingBox, const BasePiece &piece,
                                  uint64_t *random, int x, int y, int z);
    };

}
