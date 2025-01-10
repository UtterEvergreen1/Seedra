#include "StructureComponent.hpp"


#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"


MU void StructureComponent::setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn, const int x,
                                          const int y, const int z, const BoundingBox& structureBB) const {
    if (const auto blockPos = Pos3D(getWorldX(x, z), getWorldY(y), getWorldZ(x, z));
        structureBB.isVecInside(blockPos)) {
        /*
                if (this->mirror != Mirror::NONE) {
                    blockStateIn = blockStateIn.withMirror(this->mirror);
                }
                if (this->rotation != Rotation::NONE) {
                    blockStateIn = blockStateIn.withRotation(this->rotation);
                }
                 */
        worldIn.setBlock(blockPos, blockStateIn); // flags = 2
    }
}


void StructureComponent::setBlockState(World& worldIn, const lce::blocks::Block& blockStateIn, const int x, const int y,
                                       const int z, const BoundingBox& structureBB) const {
    setBlockState(worldIn, &blockStateIn, x, y, z, structureBB);
}


MU ND const lce::blocks::Block* StructureComponent::getBlockStateFromPos(World& worldIn, const int x, const int y,
                                                                         const int z,
                                                                         const BoundingBox& boundingBoxIn) const {
    const int i = getWorldX(x, z);
    const int j = getWorldY(y);
    const int k = getWorldZ(x, z);
    const Pos3D blockPos = {i, j, k};
    return !boundingBoxIn.isVecInside(blockPos) ? &lce::blocks::BlocksInit::AIR : worldIn.getBlock(blockPos);
}


/**
 * Deletes all continuous blocks from selected position upwards. Stops at hitting air.
 */
void StructureComponent::clearCurrentPositionBlocksUpwards(World worldIn, const int x, const int y, const int z,
                                                           const BoundingBox& structureBB) const {

    if (Pos3D blockPos(getWorldX(x, z), getWorldY(y), getWorldZ(x, z)); structureBB.isVecInside(blockPos)) {
        while (!worldIn.isAirBlock(blockPos) && blockPos.getY() < 255) {
            worldIn.setBlock(blockPos, lce::blocks::ids::AIR_ID); // 2
            blockPos = blockPos.up();
        }
    }
}


/**
 * Replaces air and liquid from given position downwards. Stops when hitting anything else than air or liquid
 */
void StructureComponent::replaceAirAndLiquidDownwards(World& worldIn, const lce::blocks::Block* blockStateIn,
                                                      const int x, const int y, const int z,
                                                      const BoundingBox& boundingBoxIn) const {
    const int i = getWorldX(x, z);
    int j = getWorldY(y);
    const int k = getWorldZ(x, z);

    if (boundingBoxIn.isVecInside(Pos3D(i, j, k))) {
        while ((worldIn.isAirBlock(Pos3D(i, j, k)) ||
                lce::blocks::ids::isLiquidBlock(worldIn.getBlock(i, j, k)->getID())) &&
               j > 1) {
            worldIn.setBlock(i, j, k, blockStateIn); // 2
            --j;
        }
    }
}


void StructureComponent::fillWithBlocks(World& worldIn, const BoundingBox& bbIn, const int minX, const int minY,
                                        const int minZ, const int maxX, const int maxY, const int maxZ,
                                        const lce::blocks::Block* boundaryBlockState,
                                        const lce::blocks::Block* insideBlockState, const bool existingOnly) {


    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {

                const int wX = getWorldX(x, z);
                const int wY = getWorldY(y);
                const int wZ = getWorldZ(x, z);

                if (bbIn.isVecInside({wX, wY, wZ})) {

                    // TODO: probably replace with is MATERIAL::AIR? I cannot remember
                    if (!existingOnly || worldIn.getBlock(wX, wY, wZ)->getID() != lce::blocks::ids::AIR_ID) {
                        if (y != minY && y != maxY && x != minX && x != maxX && z != minZ && z != maxZ) {
                            worldIn.setBlock(wX, wY, wZ, insideBlockState);
                        } else {
                            worldIn.setBlock(wX, wY, wZ, boundaryBlockState);
                        }
                    }
                }
            }
        }
    }
}


bool StructureComponent::isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                        const StructureComponent& pieceBoundingBox,
                                                        const ChunkPrimer* chunk) {
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


bool StructureComponent::intersectsWithBlock(const BoundingBox& chunkBoundingBox, int x, int y, int z) {
    return chunkBoundingBox.maxX >= x && chunkBoundingBox.minX <= x && chunkBoundingBox.maxY >= y &&
           chunkBoundingBox.minY <= y && chunkBoundingBox.maxZ >= z && chunkBoundingBox.minZ <= z;
}


MU bool StructureComponent::validToPlace(const BoundingBox& chunkBoundingBox, const BoundingBox& bb, int x, int y,
                                         int z) {
    if (intersectsWithBlock(chunkBoundingBox, x, y, z)) {
        return bb.maxX >= x && bb.minX <= x && bb.maxY >= y && bb.minY <= y && bb.maxZ >= z && bb.minZ <= z;
    } else {
        return false;
    }
}


void StructureComponent::fillWithRandomizedBlocks(const BoundingBox& chunkBoundingBox, const Piece& piece,
                                                  const int minX, const int minY, const int minZ, const int maxX,
                                                  const int maxY, const int maxZ, RNG& rng, const ChunkPrimer* chunk) {

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
void StructureComponent::generateChest(const BoundingBox& chunkBB, const Piece& piece, RNG& rng, const int x,
                                       const int y, const int z) {
    const int xPos = piece.getWorldX(x, z);
    const int yPos = piece.getWorldY(y);
    const int zPos = piece.getWorldZ(x, z);
    if (intersectsWithBlock(chunkBB, xPos & 15, yPos, zPos & 15)) {
        rng.advance<2>(); // nextLong for loot table seed
    }
}


bool StructureComponent::isLiquidInStructureBoundingBox(World& worldIn, const BoundingBox& bbIn) {
    int i = std::max(minX - 1, (int)bbIn.minX);
    int j = std::max(minY - 1, (int)bbIn.minY);
    int k = std::max(minZ - 1, (int)bbIn.minZ);
    int l = std::min(maxX + 1, (int)bbIn.maxX);
    int i1 = std::min(maxY + 1, (int)bbIn.maxY);
    int j1 = std::min(maxZ + 1, (int)bbIn.maxZ);

    for (int k1 = i; k1 <= l; ++k1) {
        for (int l1 = k; l1 <= j1; ++l1) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(k1, j, l1))) {
                return true;
            }

            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(k1, i1, l1))) {
                return true;
            }
        }
    }

    for (int i2 = i; i2 <= l; ++i2) {
        for (int k2 = j; k2 <= i1; ++k2) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(i2, k2, k))) {
                return true;
            }

            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(i2, k2, j1))) {
                return true;
            }
        }
    }

    for (int j2 = k; j2 <= j1; ++j2) {
        for (int l2 = j; l2 <= i1; ++l2) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(i, l2, j2))) {
                return true;
            }

            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(l, l2, j2))) {
                return true;
            }
        }
    }

    return false;
}
