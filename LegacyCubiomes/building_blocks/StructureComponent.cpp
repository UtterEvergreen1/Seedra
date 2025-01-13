#include "StructureComponent.hpp"


#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"


MU void StructureComponent::setBlockStateWithoutOffset(World& worldIn, const lce::blocks::Block* blockStateIn, int x,
                                                       int y, int z, const BoundingBox& structureBB) {
    if (const auto blockPos = Pos3D(x, y, z); structureBB.isVecInside(blockPos)) {
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


MU void StructureComponent::setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn, c_int x, c_int y,
                                          c_int z, const BoundingBox& structureBB) const {
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


void StructureComponent::setBlockState(World& worldIn, const lce::blocks::Block& blockStateIn, c_int x, c_int y,
                                       c_int z, const BoundingBox& structureBB) const {
    setBlockState(worldIn, &blockStateIn, x, y, z, structureBB);
}


MU ND const lce::blocks::Block* StructureComponent::getBlockStateFromPos(World& worldIn, c_int x, c_int y, c_int z,
                                                                         const BoundingBox& boundingBoxIn) const {
    c_int i = getWorldX(x, z);
    c_int j = getWorldY(y);
    c_int k = getWorldZ(x, z);
    const Pos3D blockPos = {i, j, k};
    return !boundingBoxIn.isVecInside(blockPos) ? &lce::blocks::BlocksInit::AIR : worldIn.getBlock(blockPos);
}


/**
 * Deletes all continuous blocks from selected position upwards. Stops at hitting air.
 */
void StructureComponent::clearCurrentPositionBlocksUpwards(World& worldIn, c_int x, c_int y, c_int z,
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
void StructureComponent::replaceAirAndLiquidDownwards(World& worldIn, const lce::blocks::Block* blockStateIn, c_int x,
                                                      c_int y, c_int z, const BoundingBox& boundingBoxIn) const {
    c_int i = getWorldX(x, z);
    int j = getWorldY(y);
    c_int k = getWorldZ(x, z);

    if (boundingBoxIn.isVecInside(Pos3D(i, j, k))) {
        while ((worldIn.isAirBlock(Pos3D(i, j, k)) ||
                lce::blocks::ids::isLiquidBlock(worldIn.getBlock(i, j, k)->getID())) &&
               j > 1) {
            worldIn.setBlock(i, j, k, blockStateIn); // 2
            --j;
        }
    }
}


void StructureComponent::fillWithBlocks(World& worldIn, const BoundingBox& bbIn, c_int minX, c_int minY, c_int minZ,
                                        c_int maxX, c_int maxY, c_int maxZ,
                                        const lce::blocks::Block* boundaryBlockState,
                                        const lce::blocks::Block* insideBlockState, const bool existingOnly) {


    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {

                c_int wX = getWorldX(x, z);
                c_int wY = getWorldY(y);
                c_int wZ = getWorldZ(x, z);

                if (bbIn.isVecInside({wX, wY, wZ})) {

                    // TODO: probably replace with is MATERIAL::AIR? I cannot remember
                    if (!existingOnly || worldIn.getBlock(wX, wY, wZ)->getID() != lce::blocks::ids::AIR_ID) {
                        if (y != minY && y != maxY && x != minX && x != maxX && z != minZ && z != maxZ) {
                            setBlockState(worldIn, insideBlockState, x, y, z, bbIn);
                        } else {
                            setBlockState(worldIn, boundaryBlockState, x, y, z, bbIn);
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
            if (lce::blocks::ids::isLiquidBlock(chunk->getBlockId(x, minY, z))) return true;
            if (lce::blocks::ids::isLiquidBlock(chunk->getBlockId(x, maxY, z))) return true;
        }
    }
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            if (lce::blocks::ids::isLiquidBlock(chunk->getBlockId(x, y, minZ))) return true;
            if (lce::blocks::ids::isLiquidBlock(chunk->getBlockId(x, y, maxZ))) return true;
        }
    }
    for (int z = minZ; z <= maxZ; ++z) {
        for (int y = minY; y <= maxY; ++y) {
            if (lce::blocks::ids::isLiquidBlock(chunk->getBlockId(minX, y, z))) return true;
            if (lce::blocks::ids::isLiquidBlock(chunk->getBlockId(maxX, y, z))) return true;
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


void StructureComponent::fillWithRandomizedBlocks(const BoundingBox& chunkBoundingBox, const Piece& piece, c_int minX,
                                                  c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ, RNG& rng,
                                                  const ChunkPrimer* chunk) {

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {
                if (y == minY || y == maxY || x == minX || x == maxX || z == minZ || z == maxZ) {
                    c_int worldX = piece.getWorldX(x, z);
                    c_int worldY = piece.getWorldY(y);
                    c_int worldZ = piece.getWorldZ(x, z);

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
void StructureComponent::generateChest(const BoundingBox& chunkBB, const Piece& piece, RNG& rng, c_int x, c_int y,
                                       c_int z) {
    c_int xPos = piece.getWorldX(x, z);
    c_int yPos = piece.getWorldY(y);
    c_int zPos = piece.getWorldZ(x, z);
    if (intersectsWithBlock(chunkBB, xPos & 15, yPos, zPos & 15)) {
        rng.advance<2>(); // nextLong for loot table seed
    }
}


bool StructureComponent::isLiquidInStructureBoundingBox(World& worldIn, const BoundingBox& bbIn) {
    int i = std::max(minX - 1, (int) bbIn.minX);
    int j = std::max(minY - 1, (int) bbIn.minY);
    int k = std::max(minZ - 1, (int) bbIn.minZ);
    int l = std::min(maxX + 1, (int) bbIn.maxX);
    int i1 = std::min(maxY + 1, (int) bbIn.maxY);
    int j1 = std::min(maxZ + 1, (int) bbIn.maxZ);

    for (int k1 = i; k1 <= l; ++k1) {
        for (int l1 = k; l1 <= j1; ++l1) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(k1, j, l1))) return true;
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(k1, i1, l1))) return true;
        }
    }
    for (int i2 = i; i2 <= l; ++i2) {
        for (int k2 = j; k2 <= i1; ++k2) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(i2, k2, k))) return true;
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(i2, k2, j1))) return true;
        }
    }
    for (int j2 = k; j2 <= j1; ++j2) {
        for (int l2 = j; l2 <= i1; ++l2) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(i, l2, j2))) return true;
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(l, l2, j2))) return true;
        }
    }

    return false;
}


std::ostream& operator<<(std::ostream& out, const StructureComponent& structureComponent) {
    std::string dir = facingToString(structureComponent.facing);
    out << "{" << structureComponent.toString() << ", FACE=" << dir << "}";
    return out;
}
