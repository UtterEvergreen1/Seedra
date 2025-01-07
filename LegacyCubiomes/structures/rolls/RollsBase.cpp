#include "RollsBase.hpp"

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"


namespace structure_rolls {

    bool RollsBase::isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                   const BoundingBox& pieceBoundingBox, const ChunkPrimer* chunk) {
        c_int minX = std::max(pieceBoundingBox.minX - 1, (int) chunkBoundingBoxIn.minX) & 15;
        c_int minY = std::max(pieceBoundingBox.minY - 1, (int) chunkBoundingBoxIn.minY);
        c_int minZ = std::max(pieceBoundingBox.minZ - 1, (int) chunkBoundingBoxIn.minZ) & 15;
        c_int maxX = std::min(pieceBoundingBox.maxX + 1, (int) chunkBoundingBoxIn.maxX) & 15;
        c_int maxY = std::min(pieceBoundingBox.maxY + 1, (int) chunkBoundingBoxIn.maxY);
        c_int maxZ = std::min(pieceBoundingBox.maxZ + 1, (int) chunkBoundingBoxIn.maxZ) & 15;
        for (int x = minX; x <= maxX; ++x) {
            for (int z = minZ; z <= maxZ; ++z) {
                c_u16 block = chunk->getBlockId(x, minY, z);

                if (lce::blocks::ids::isLiquidBlock(block)) return true;

                c_u16 block1 = chunk->getBlockId(x, maxY, z);
                if (lce::blocks::ids::isLiquidBlock(block1)) return true;
            }
        }


        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                c_u16 block = chunk->getBlockId(x, y, minZ);
                if (lce::blocks::ids::isLiquidBlock(block)) return true;

                c_u16 block1 = chunk->getBlockId(x, y, maxZ);
                if (lce::blocks::ids::isLiquidBlock(block1)) return true;
            }
        }


        for (int z = minZ; z <= maxZ; ++z) {
            for (int y = minY; y <= maxY; ++y) {
                c_u16 block = chunk->getBlockId(minX, y, z);
                if (lce::blocks::ids::isLiquidBlock(block)) return true;

                c_u16 block1 = chunk->getBlockId(maxX, y, z);
                if (lce::blocks::ids::isLiquidBlock(block1)) return true;
            }
        }

        return false;
    }


    bool RollsBase::intersectsWithBlock(const BoundingBox& chunkBoundingBox, int x, int y, int z) {
        return chunkBoundingBox.maxX >= x && chunkBoundingBox.minX <= x && chunkBoundingBox.maxY >= y &&
               chunkBoundingBox.minY <= y && chunkBoundingBox.maxZ >= z && chunkBoundingBox.minZ <= z;
    }


    MU bool RollsBase::validToPlace(const BoundingBox& chunkBoundingBox, const BoundingBox& bb, int x, int y, int z) {
        if (intersectsWithBlock(chunkBoundingBox, x, y, z)) {
            return bb.maxX >= x && bb.minX <= x && bb.maxY >= y && bb.minY <= y && bb.maxZ >= z && bb.minZ <= z;
        } else {
            return false;
        }
    }


    void RollsBase::fillWithRandomizedBlocks(const BoundingBox& chunkBoundingBox, const Piece& piece, const int minX,
                                             const int minY, const int minZ, const int maxX, const int maxY,
                                             const int maxZ, RNG& rng, const ChunkPrimer* chunk) {

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                for (int z = minZ; z <= maxZ; z++) {
                    if (y == minY || y == maxY || x == minX || x == maxX || z == minZ || z == maxZ) {
                        const int worldX = piece.getWorldX(x, z);
                        const int worldY = piece.getWorldY(y);
                        const int worldZ = piece.getWorldZ(x, z);

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
    void RollsBase::generateChest(const BoundingBox& chunkBB, const Piece& piece, RNG& rng, const int x, const int y,
                                  const int z) {
        const int xPos = piece.getWorldX(x, z);
        const int yPos = piece.getWorldY(y);
        const int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBB, xPos & 15, yPos, zPos & 15)) {
            rng.advance<2>(); // nextLong for loot table seed
        }
    }


    void RollsBase::fillWithBlocks(World& worldIn, const Piece& piece, const int minX, const int minY, const int minZ,
                                   const int maxX, const int maxY, const int maxZ,
                                   const lce::blocks::Block* boundaryBlockState,
                                   const lce::blocks::Block* insideBlockState, const bool override) {


        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                for (int z = minZ; z <= maxZ; z++) {

                    const int wX = piece.getWorldX(x, z);
                    const int wY = piece.getWorldY(y);
                    const int wZ = piece.getWorldZ(x, z);

                    // if (bbIn.isVecInside({wX, wY, wZ})) {
                    if (override || worldIn.getBlock(wX, wY, wZ)->getID() != lce::blocks::ids::AIR_ID) {
                        if (y != minY && y != maxY && x != minX && x != maxX && z != minZ && z != maxZ) {
                            worldIn.setBlock(wX, wY, wZ, insideBlockState->getID());
                        } else {
                            worldIn.setBlock(wX, wY, wZ, boundaryBlockState->getID());
                        }
                    }
                    // }
                }
            }
        }
    }


} // namespace structure_rolls
