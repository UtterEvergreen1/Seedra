#include "rolls_base.hpp"

#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"


namespace structure_rolls {

    bool RollsBase::isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                   const BoundingBox& pieceBoundingBox, ChunkPrimer* chunk) {
        int minX = std::max(pieceBoundingBox.minX - 1, chunkBoundingBoxIn.minX) & 15;
        int minY = std::max(pieceBoundingBox.minY - 1, chunkBoundingBoxIn.minY);
        int minZ = std::max(pieceBoundingBox.minZ - 1, chunkBoundingBoxIn.minZ) & 15;
        int maxX = std::min(pieceBoundingBox.maxX + 1, chunkBoundingBoxIn.maxX) & 15;
        int maxY = std::min(pieceBoundingBox.maxY + 1, chunkBoundingBoxIn.maxY);
        int maxZ = std::min(pieceBoundingBox.maxZ + 1, chunkBoundingBoxIn.maxZ) & 15;
        for (int x = minX; x <= maxX; ++x) {
            for (int z = minZ; z <= maxZ; ++z) {
                uint16_t block = chunk->getBlock(x, minY, z);
                //if (block == Items::ItemID::FLOWING_WATER_ID || block == Items::ItemID::STILL_WATER_ID || block == Items::ItemID::FLOWING_LAVA_ID || block == Items::ItemID::STILL_LAVA_ID)
                if (block >= Items::ItemID::FLOWING_WATER_ID && block <= Items::ItemID::STILL_LAVA_ID)
                    return true;

                uint16_t block1 = chunk->getBlock(x, maxY, z);
                if (block1 >= Items::ItemID::FLOWING_WATER_ID && block <= Items::ItemID::STILL_LAVA_ID)
                    return true;
            }
        }


        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                uint16_t block = chunk->getBlock(x, y, minZ);
                if (block >= Items::ItemID::FLOWING_WATER_ID && block <= Items::ItemID::STILL_LAVA_ID)
                    return true;

                uint16_t block1 = chunk->getBlock(x, y, maxZ);
                if (block1 >= Items::ItemID::FLOWING_WATER_ID && block <= Items::ItemID::STILL_LAVA_ID)
                    return true;
            }
        }


        for (int z = minZ; z <= maxZ; ++z) {
            for (int y = minY; y <= maxY; ++y) {
                uint16_t block = chunk->getBlock(minX, y, z);
                if (block >= Items::ItemID::FLOWING_WATER_ID && block <= Items::ItemID::STILL_LAVA_ID)
                    return true;

                uint16_t block1 = chunk->getBlock(maxX, y, z);
                if (block1 >= Items::ItemID::FLOWING_WATER_ID && block <= Items::ItemID::STILL_LAVA_ID)
                    return true;
            }
        }

        return false;
    }


    bool RollsBase::intersectsWithBlock(const BoundingBox &chunkBoundingBox, int x, int y, int z) {
        return chunkBoundingBox.maxX >= x && chunkBoundingBox.minX <= x &&
               chunkBoundingBox.maxY >= y && chunkBoundingBox.minY <= y &&
               chunkBoundingBox.maxZ >= z && chunkBoundingBox.minZ <= z;
    }


    bool RollsBase::validToPlace(const BoundingBox &chunkBoundingBox, const BoundingBox& bb, int x, int y, int z) {
        if (intersectsWithBlock(chunkBoundingBox, x, y, z)) {
            return bb.maxX >= x && bb.minX <= x &&
                   bb.maxY >= y && bb.minY <= y &&
                   bb.maxZ >= z && bb.minZ <= z;
        } else {
            return false;
        }
    }


    void RollsBase::fillWithRandomizedBlocks(const BoundingBox &chunkBoundingBox, const Piece &piece, int minX, int minY,
                                             int minZ, int maxX, int maxY, int maxZ, uint64_t *random, ChunkPrimer *chunk) {
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
                        if (intersectsWithBlock(chunkBoundingBox, worldX, worldY, worldZ) && (chunk == nullptr || chunk->getBlock(worldX & 15, worldY, worldZ & 15))) {
                            *random = (*random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
                        }
                    }
                }
            }
        }
    }


    // TODO: generate legacy chest where the loot is generated with the seed and doesn't use the loot table seed
    void RollsBase::generateChest(const BoundingBox &chunkBoundingBox, const Piece &piece, uint64_t *random, int x, int y, int z) {
        int xPos = piece.getWorldX(x, z);
        int yPos = piece.getWorldY(y);
        int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBoundingBox, xPos & 15, yPos, zPos & 15)) {
            // nextLong for loot table seed
            *random = (*random * 0xBB20B4600A69 + 0x40942DE6BA) & 0xFFFFFFFFFFFF; // 2 rolls
        }
    }
}
