#include "rolls_base.hpp"

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"


namespace structure_rolls {

    bool RollsBase::isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                   const BoundingBox& pieceBoundingBox, ChunkPrimer* chunk) {
        c_int minX = std::max(pieceBoundingBox.minX - 1, (int) chunkBoundingBoxIn.minX) & 15;
        c_int minY = std::max(pieceBoundingBox.minY - 1, (int) chunkBoundingBoxIn.minY);
        c_int minZ = std::max(pieceBoundingBox.minZ - 1, (int) chunkBoundingBoxIn.minZ) & 15;
        c_int maxX = std::min(pieceBoundingBox.maxX + 1, (int) chunkBoundingBoxIn.maxX) & 15;
        c_int maxY = std::min(pieceBoundingBox.maxY + 1, (int) chunkBoundingBoxIn.maxY);
        c_int maxZ = std::min(pieceBoundingBox.maxZ + 1, (int) chunkBoundingBoxIn.maxZ) & 15;
        for (int x = minX; x <= maxX; ++x) {
            for (int z = minZ; z <= maxZ; ++z) {
                u16 block = chunk->getBlockId(x, minY, z);
                if (block >= lce::blocks::ids::FLOWING_WATER_ID && block <= lce::blocks::ids::STILL_LAVA_ID) return true;

                u16 block1 = chunk->getBlockId(x, maxY, z);
                if (block1 >= lce::blocks::ids::FLOWING_WATER_ID && block <= lce::blocks::ids::STILL_LAVA_ID) return true;
            }
        }


        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                u16 block = chunk->getBlockId(x, y, minZ);
                if (block >= lce::blocks::ids::FLOWING_WATER_ID && block <= lce::blocks::ids::STILL_LAVA_ID) return true;

                u16 block1 = chunk->getBlockId(x, y, maxZ);
                if (block1 >= lce::blocks::ids::FLOWING_WATER_ID && block <= lce::blocks::ids::STILL_LAVA_ID) return true;
            }
        }


        for (int z = minZ; z <= maxZ; ++z) {
            for (int y = minY; y <= maxY; ++y) {
                u16 block = chunk->getBlockId(minX, y, z);
                if (block >= lce::blocks::ids::FLOWING_WATER_ID && block <= lce::blocks::ids::STILL_LAVA_ID) return true;

                u16 block1 = chunk->getBlockId(maxX, y, z);
                if (block1 >= lce::blocks::ids::FLOWING_WATER_ID && block <= lce::blocks::ids::STILL_LAVA_ID) return true;
            }
        }

        return false;
    }


    bool RollsBase::intersectsWithBlock(const BoundingBox& chunkBoundingBox, int x, int y, int z) {
        return chunkBoundingBox.maxX >= x && chunkBoundingBox.minX <= x && chunkBoundingBox.maxY >= y &&
               chunkBoundingBox.minY <= y && chunkBoundingBox.maxZ >= z && chunkBoundingBox.minZ <= z;
    }


    bool RollsBase::validToPlace(const BoundingBox& chunkBoundingBox, const BoundingBox& bb, int x, int y, int z) {
        if (intersectsWithBlock(chunkBoundingBox, x, y, z)) {
            return bb.maxX >= x && bb.minX <= x && bb.maxY >= y && bb.minY <= y && bb.maxZ >= z && bb.minZ <= z;
        } else {
            return false;
        }
    }


    void RollsBase::fillWithRandomizedBlocks(const BoundingBox& chunkBoundingBox, const Piece& piece, int minX,
                                             int minY, int minZ, int maxX, int maxY, int maxZ, RNG& rng,
                                             ChunkPrimer* chunk) {
        int worldX;
        int worldY;
        int worldZ;
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                for (int z = minZ; z <= maxZ; z++) {
                    if (y == minY || y == maxY || x == minX || x == maxX || z == minZ || z == maxZ) {
                        worldX = piece.getWorldX(x, z);
                        worldY = piece.getWorldY(y);
                        worldZ = piece.getWorldZ(x, z);
                        if (intersectsWithBlock(chunkBoundingBox, worldX, worldY, worldZ) &&
                            (chunk == nullptr || chunk->getBlockId(worldX & 15, worldY, worldZ & 15))) {
                            rng.advance(); // advance rng
                        }
                    }
                }
            }
        }
    }


    // TODO: generate legacy chest where the loot is generated with the seed and doesn't use the loot table seed
    void RollsBase::generateChest(const BoundingBox& chunkBB, const Piece& piece, RNG& rng, int x, int y, int z) {
        int xPos = piece.getWorldX(x, z);
        int yPos = piece.getWorldY(y);
        int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBB, xPos & 15, yPos, zPos & 15)) {
            rng.advance<2>(); // nextLong for loot table seed
        }
    }
} // namespace structure_rolls
