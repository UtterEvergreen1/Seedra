#include "StructureComponent.hpp"


#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

#include "StrongholdStones.hpp"

#include "LegacyCubiomes/utils/RotationAndMirror.hpp"
#include "LegacyCubiomes/utils/rng.hpp"




MU void StructureComponent::setBlockStateWithoutOffset(World& worldIn, const lce::blocks::Block* blockStateIn, c_int x,
                                                       c_int y, c_int z, const BoundingBox& structureBB) {
    if (const auto blockPos = Pos3D(x, y, z); structureBB.isVecInside(blockPos)) {
        /*if (this->mirror != Mirror::NONE)
            blockStateIn = blockStateIn.withMirror(this->mirror);
        if (this->rotation != Rotation::NONE)
            blockStateIn = blockStateIn.withRotation(this->rotation);*/
        worldIn.setBlock(blockPos, blockStateIn); // flags = 2
    }
}


MU void StructureComponent::setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn, c_int x, c_int y,
                                          c_int z, const BoundingBox& structureBB) const {
    if (const auto blockPos = Pos3D(getWorldX(x, z), getWorldY(y), getWorldZ(x, z));
        structureBB.isVecInside(blockPos)) {
        /*if (this->mirror != Mirror::NONE)
            blockStateIn = blockStateIn.withMirror(this->mirror);
        if (this->rotation != Rotation::NONE)
            blockStateIn = blockStateIn.withRotation(this->rotation);*/
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


/// Deletes all continuous blocks from selected position upwards. Stops at hitting air.
void StructureComponent::clearCurrentPositionBlocksUpwards(World& worldIn, c_int x, c_int y, c_int z,
                                                           const BoundingBox& structureBB) const {

    if (Pos3D blockPos(getWorldX(x, z), getWorldY(y), getWorldZ(x, z)); structureBB.isVecInside(blockPos)) {
        while (!worldIn.isAirBlock(blockPos) && blockPos.getY() < 255) {
            worldIn.setBlock(blockPos, lce::blocks::ids::AIR_ID); // 2
            blockPos = blockPos.up();
        }
    }
}


/// Replaces air and liquid from given position downwards. Stops when hitting anything else than air or liquid
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
                                        const lce::blocks::Block* insideBlockState, const bool existingOnly) const {

    // TODO: do BoundingBox::shrinkToFit on bounds

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


void StructureComponent::fillWithBlocks(World& worldIn, const BoundingBox& bbIn, c_int minX, c_int minY,
                                             c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                                             const lce::blocks::Block* theBlockState, c_bool existingOnly) const {
    // TODO: do BoundingBox::shrinkToFit on bounds

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {

                c_int wX = getWorldX(x, z);
                c_int wY = getWorldY(y);
                c_int wZ = getWorldZ(x, z);

                if (bbIn.isVecInside({wX, wY, wZ})) {

                    if (!existingOnly || worldIn.getBlock(wX, wY, wZ)->getID() != lce::blocks::ids::AIR_ID) {
                        setBlockState(worldIn, theBlockState, x, y, z, bbIn);
                    }
                }
            }
        }
    }
}


void StructureComponent::fillWithAir(World& worldIn, const BoundingBox& bbIn,
    c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ) const {

    // TODO: do BoundingBox::shrinkToFit on bounds

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {

                c_int wX = getWorldX(x, z);
                c_int wY = getWorldY(y);
                c_int wZ = getWorldZ(x, z);

                if (bbIn.isVecInside({wX, wY, wZ})) {
                    setBlockState(worldIn, &lce::blocks::BlocksInit::AIR, x, y, z, bbIn);
                }
            }
        }
    }
}


bool StructureComponent::isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                                        const StructureComponent& piece, const ChunkPrimer* chunk) {

    c_int minX = std::max(piece.minX - 1, static_cast<int>(chunkBoundingBoxIn.minX)) & 15;
    c_int minY = std::max(piece.minY - 1, static_cast<int>(chunkBoundingBoxIn.minY));
    c_int minZ = std::max(piece.minZ - 1, static_cast<int>(chunkBoundingBoxIn.minZ)) & 15;
    c_int maxX = std::min(piece.maxX + 1, static_cast<int>(chunkBoundingBoxIn.maxX)) & 15;
    c_int maxY = std::min(piece.maxY + 1, static_cast<int>(chunkBoundingBoxIn.maxY));
    c_int maxZ = std::min(piece.maxZ + 1, static_cast<int>(chunkBoundingBoxIn.maxZ)) & 15;
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


bool StructureComponent::intersectsWithBlock(const BoundingBox& chunkBoundingBox, c_int x, c_int y, c_int z) {
    return chunkBoundingBox.maxX >= x && chunkBoundingBox.minX <= x && chunkBoundingBox.maxY >= y &&
           chunkBoundingBox.minY <= y && chunkBoundingBox.maxZ >= z && chunkBoundingBox.minZ <= z;
}


MU bool StructureComponent::validToPlace(const BoundingBox& chunkBoundingBox, const BoundingBox& bb, c_int x, c_int y,
                                         c_int z) {
    if (intersectsWithBlock(chunkBoundingBox, x, y, z)) {
        return (bb.maxX >= x && bb.minX <= x) && (bb.maxY >= y && bb.minY) <= y && (bb.maxZ >= z && bb.minZ <= z);
    }
    return false;
}


void StructureComponent::fillWithRandomizedBlocksRNGOnly(const BoundingBox& chunkBoundingBox,
                                                         const StructureComponent& piece, c_int minX, c_int minY,
                                                         c_int minZ, c_int maxX, c_int maxY, c_int maxZ, RNG& rng,
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
void StructureComponent::generateChest(const BoundingBox& chunkBB, const StructureComponent& piece, RNG& rng, c_int x,
                                       c_int y, c_int z) {
    c_int xPos = piece.getWorldX(x, z);
    c_int yPos = piece.getWorldY(y);
    c_int zPos = piece.getWorldZ(x, z);
    if (intersectsWithBlock(chunkBB, xPos & 15, yPos, zPos & 15)) {
        rng.advance<2>(); // nextLong for loot table seed
    }
}


bool StructureComponent::isLiquidInStructureBoundingBox(World& worldIn, const BoundingBox& bbIn) const {
    c_int checkMinX = std::max(minX - 1, static_cast<int>(bbIn.minX));
    c_int checkMinY = std::max(minY - 1, static_cast<int>(bbIn.minY));
    c_int checkMinZ = std::max(minZ - 1, static_cast<int>(bbIn.minZ));
    c_int checkMaxX = std::min(maxX + 1, static_cast<int>(bbIn.maxX));
    c_int checkMaxY = std::min(maxY + 1, static_cast<int>(bbIn.maxY));
    c_int checkMaxZ = std::min(maxZ + 1, static_cast<int>(bbIn.maxZ));

    for (int x = checkMinX; x <= checkMaxX; ++x) {
        for (int z = checkMinZ; z <= checkMaxZ; ++z) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(x, checkMinY, z))) return true;
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(x, checkMaxY, z))) return true;
        }
    }
    for (int x = checkMinX; x <= checkMaxX; ++x) {
        for (int y = checkMinY; y <= checkMaxY; ++y) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(x, y, checkMinZ))) return true;
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(x, y, checkMaxZ))) return true;
        }
    }
    for (int z = checkMinZ; z <= checkMaxZ; ++z) {
        for (int y = checkMinY; y <= checkMaxY; ++y) {
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(checkMinX, y, z))) return true;
            if (lce::blocks::ids::isLiquidBlock(worldIn.getBlockId(checkMaxX, y, z))) return true;
        }
    }

    return false;
}


void StructureComponent::randomlyRareFillWithBlocks(World& worldIn, const BoundingBox& bbIn, c_int minX, c_int minY,
                                                    c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                                                    const lce::blocks::Block* blockStateIn, c_bool excludeAir) const {
    c_auto f = static_cast<float>(maxX - minX + 1);
    c_auto f1 = static_cast<float>(maxY - minY + 1);
    c_auto f2 = static_cast<float>(maxZ - minZ + 1);
    c_auto f3 = static_cast<float>(minX) + f / 2.0F;
    c_auto f4 = static_cast<float>(minZ) + f2 / 2.0F;

    for (int i = minY; i <= maxY; ++i) {
        c_auto f5 = static_cast<float>(i - minY) / f1;

        for (int j = minX; j <= maxX; ++j) {
            c_auto f6 = (static_cast<float>(j) - f3) / (f * 0.5F);

            for (int k = minZ; k <= maxZ; ++k) {
                c_auto f7 = (static_cast<float>(k) - f4) / (f2 * 0.5F);

                if (!excludeAir ||
                    !lce::blocks::ids::isReplaceableBlock(getBlockStateFromPos(worldIn, j, i, k, bbIn)->getID())) {
                    c_auto f8 = f6 * f6 + f5 * f5 + f7 * f7;

                    if (f8 <= 1.05F) { setBlockState(worldIn, blockStateIn, j, i, k, bbIn); }
                }
            }
        }
    }
}


int StructureComponent::getLightLevelAtBlock(MU World& world, MU int x, MU int y, MU int z,
                                             MU const BoundingBox& structureBB) const {
    return 0;
    // int i = this->getWorldX(x, z);
    // int j = this->getWorldY(y + 1);
    // int k = this->getWorldZ(x, z);
    // Pos3D blockPos(i, j, k);
    // return !structureBB.isVecInside(blockPos)
    // ? EnumSkyBlock.SKY.defaultLightValue
    // : world.getLightFor(EnumSkyBlock.SKY, blockPos);
}


void StructureComponent::randomlyPlaceBlock(World& worldIn, const BoundingBox& bbIn, RNG& rand, c_float chance, c_int x,
                                            c_int y, c_int z, const lce::blocks::Block* blockStateIn) const {
    if (rand.nextFloat() < chance) { setBlockState(worldIn, blockStateIn, x, y, z, bbIn); }
}


/// looks like fillWithBlocks but with a rng chance + light level checks?
void StructureComponent::fillWithBlocksRandomLightCheck(World& world, const BoundingBox& structureBB, RNG& rng,
                                                        c_float chance, c_int minX, c_int minY, c_int minZ, c_int maxX,
                                                        c_int maxY, c_int maxZ, const lce::blocks::Block* blockState1,
                                                        const lce::blocks::Block* blockState2, c_bool flag,
                                                        c_int lightLevel) const {
    for (int i = minY; i <= maxY; ++i) {
        for (int j = minX; j <= maxX; ++j) {
            for (int k = minZ; k <= maxZ; ++k) {
                if (rng.nextFloat() <= chance &&
                    (!flag || !lce::blocks::ids::isReplaceableBlock(
                                              getBlockStateFromPos(world, j, i, k, structureBB)->getID())) &&
                    (lightLevel <= 0 || getLightLevelAtBlock(world, j, i, k, structureBB) < lightLevel)) {
                    if (i != minY
                        && i != maxY
                        && j != minX
                        && j != maxX
                        && k != minZ
                        && k != maxZ) {
                        setBlockState(world, blockState2, j, i, k, structureBB);
                    } else {
                        setBlockState(world, blockState1, j, i, k, structureBB);
                    }
                }
            }
        }
    }
}


void StructureComponent::fillWithRandomizedStrongholdStones(World& worldIn, const BoundingBox& structureBB, c_int minX,
                                                            c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                                                            c_bool alwaysReplace, RNG& rng) const {
    for (int i = minY; i <= maxY; ++i) {
        for (int j = minX; j <= maxX; ++j) {
            for (int k = minZ; k <= maxZ; ++k) {
                if (!alwaysReplace || !lce::blocks::ids::isReplaceableBlock(
                                              getBlockStateFromPos(worldIn, j, i, k, structureBB)->getID())) {
                    const lce::blocks::Block* block = StrongholdStones::selectBlocks(
                            rng, i == minY || i == maxY || j == minX || j == maxX || k == minZ || k == maxZ);
                    setBlockState(worldIn, block, j, i, k, structureBB);
                }
            }
        }
    }
}


std::ostream& operator<<(std::ostream& out, const StructureComponent& structureComponent) {
    const std::string dir = facingToString(structureComponent.facing);
    out << "{" << structureComponent.toString() << ", FACE=" << dir << "}";
    return out;
}
