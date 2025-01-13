#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "Stones.hpp"
#include "lce/blocks/block.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"


class StructureComponent : public Piece {
public:
    int structureType;

    StructureComponent() = default;

    MU StructureComponent(const BoundingBox boundingBox, const FACING orientation)
        : Piece(boundingBox, orientation), structureType(0) {}


    MU StructureComponent(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                          const FACING orientation)
        : Piece(minX, minY, minZ, maxX, maxY, maxZ, orientation), structureType(0) {}

    MU StructureComponent(const PieceType type, c_i8 depth, const BoundingBox& boundingBox, const FACING orientation,
                          c_int additionalData)
        : Piece(type, depth, boundingBox, orientation, additionalData), structureType(0) {}

    virtual ~StructureComponent() = default;

    MU void usingPiece(const StructureComponent& scIn) {
        minX = scIn.minX;
        maxX = scIn.maxX;
        minY = scIn.minY;
        maxY = scIn.maxY;
        minZ = scIn.minZ;
        maxZ = scIn.maxZ;
        type = scIn.type;
        orientation = scIn.orientation;
        additionalData = scIn.additionalData;
        depth = scIn.depth;
        structureType = scIn.structureType;
    }

    // MU ND virtual bool addComponentParts(
    //     World& worldIn, RNG& rng, const BoundingBox& structureBB, StructureComponent& piece) {
    //     return false;
    // }

    MU void setBlockStateWithoutOffset(World& worldIn, const lce::blocks::Block* blockStateIn, int x, int y, int z,
                                       const BoundingBox& structureBB) const;

    MU void setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn, int x, int y, int z,
                          const BoundingBox& structureBB) const;

    void setBlockState(World& worldIn, const lce::blocks::Block& blockStateIn, int x, int y, int z,
                       const BoundingBox& structureBB) const;


    MU ND const lce::blocks::Block* getBlockStateFromPos(World& worldIn, int x, int y, int z,
                                                         const BoundingBox& boundingBoxIn) const;


    /**
     * Deletes all continuous blocks from selected position upwards. Stops at hitting air.
     */
    void clearCurrentPositionBlocksUpwards(World& worldIn, int x, int y, int z, const BoundingBox& structureBB) const;


    /**
     * Replaces air and liquid from given position downwards. Stops when hitting anything else than air or liquid
     */
    void replaceAirAndLiquidDownwards(World& worldIn, const lce::blocks::Block* blockStateIn, int x, int y, int z,
                                      const BoundingBox& boundingBoxIn) const;

    void fillWithBlocks(World& worldIn, const BoundingBox& bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, const lce::blocks::Block* boundaryBlockState,
                        const lce::blocks::Block* insideBlockState, bool existingOnly);


    static bool isLiquidInStructureBoundingBox(const BoundingBox& chunkBoundingBoxIn,
                                               const StructureComponent& pieceBoundingBox, const ChunkPrimer* chunk);

    static bool intersectsWithBlock(const BoundingBox& chunkBoundingBox, int x, int y, int z);
    MU static bool validToPlace(const BoundingBox& chunkBoundingBox, const BoundingBox& bb, int x, int y, int z);

    static void fillWithRandomizedBlocks(const BoundingBox& chunkBoundingBox, const Piece& piece, int minX, int minY,
                                         int minZ, int maxX, int maxY, int maxZ, RNG& rng, const ChunkPrimer* chunk);

    static void generateChest(const BoundingBox& chunkBB, const Piece& piece, RNG& rng, int x, int y, int z);


    bool isLiquidInStructureBoundingBox(World& worldIn, const BoundingBox& bbIn);


    void randomlyRareFillWithBlocks(World& worldIn, const BoundingBox& bbIn, int minX, int minY, int minZ, int maxX,
                                    int maxY, int maxZ, const lce::blocks::Block* blockStateIn, bool excludeAir) const {
        auto f = (float) (maxX - minX + 1);
        auto f1 = (float) (maxY - minY + 1);
        auto f2 = (float) (maxZ - minZ + 1);
        float f3 = (float) minX + f / 2.0F;
        float f4 = (float) minZ + f2 / 2.0F;

        for (int i = minY; i <= maxY; ++i) {
            float f5 = (float) (i - minY) / f1;

            for (int j = minX; j <= maxX; ++j) {
                float f6 = ((float) j - f3) / (f * 0.5F);

                for (int k = minZ; k <= maxZ; ++k) {
                    float f7 = ((float) k - f4) / (f2 * 0.5F);

                    if (!excludeAir ||
                        !lce::blocks::ids::isReplaceableBlock(getBlockStateFromPos(worldIn, j, i, k, bbIn)->getID())) {
                        float f8 = f6 * f6 + f5 * f5 + f7 * f7;

                        if (f8 <= 1.05F) { setBlockState(worldIn, blockStateIn, j, i, k, bbIn); }
                    }
                }
            }
        }
    }


    int getLightLevelAtBlock(World& world, int x, int y, int z, const BoundingBox& structureBB) {
        return 0;
        // int i = this->getWorldX(x, z);
        // int j = this->getWorldY(y + 1);
        // int k = this->getWorldZ(x, z);
        // Pos3D blockPos(i, j, k);
        // return !structureBB.isVecInside(blockPos)
        // ? EnumSkyBlock.SKY.defaultLightValue
        // : world.getLightFor(EnumSkyBlock.SKY, blockPos);
    }

    void randomlyPlaceBlock(World& worldIn, const BoundingBox& bbIn, RNG& rand, float chance, int x, int y, int z,
                            const lce::blocks::Block* blockStateIn) const {
        if (rand.nextFloat() < chance) { setBlockState(worldIn, blockStateIn, x, y, z, bbIn); }
    }


    MU void fillWithRandomizedBlocks(World& worldIn, const BoundingBox& bbIn, int minX, int minY, int minZ, int maxX,
                                     int maxY, int maxZ, bool alwaysReplace, RNG& rand
                                     /*, StructureComponentBlockSelector blockselector*/) const {
        for (int i = minY; i <= maxY; ++i) {
            for (int j = minX; j <= maxX; ++j) {
                for (int k = minZ; k <= maxZ; ++k) {
                    if (!alwaysReplace ||
                        lce::blocks::ids::isReplaceableBlock(getBlockStateFromPos(worldIn, j, i, k, bbIn)->getID())) {
                        // blockselector.selectBlocks(rand, j, i, k,
                        //                            i == minY || i == maxY || j == minX || j == maxX || k == minZ ||
                        //                                    k == maxZ);
                        const lce::blocks::Block* block = &lce::blocks::BlocksInit::BEDROCK;
                        setBlockState(worldIn, block /*blockselector.getBlockState()*/, j, i, k, bbIn);
                    }
                }
            }
        }
    }


    void func_189914_a(World& world, const BoundingBox& structureBB, RNG& rng, float chance, int p_189914_5_,
                       int p_189914_6_, int p_189914_7_, int p_189914_8_, int p_189914_9_, int p_189914_10_,
                       const lce::blocks::Block* blockState1, const lce::blocks::Block* blockState2, bool p_189914_13_,
                       int p_189914_14_) {
        for (int i = p_189914_6_; i <= p_189914_9_; ++i) {
            for (int j = p_189914_5_; j <= p_189914_8_; ++j) {
                for (int k = p_189914_7_; k <= p_189914_10_; ++k) {
                    if (rng.nextFloat() <= chance &&
                        (!p_189914_13_ || !lce::blocks::ids::isReplaceableBlock(
                                                  getBlockStateFromPos(world, j, i, k, structureBB)->getID())) &&
                        (p_189914_14_ <= 0 || getLightLevelAtBlock(world, j, i, k, structureBB) < p_189914_14_)) {
                        if (i != p_189914_6_ && i != p_189914_9_ && j != p_189914_5_ && j != p_189914_8_ &&
                            k != p_189914_7_ && k != p_189914_10_) {
                            setBlockState(world, blockState2, j, i, k, structureBB);
                        } else {
                            setBlockState(world, blockState1, j, i, k, structureBB);
                        }
                    }
                }
            }
        }
    }



    void fillWithRandomizedStones(World& worldIn, const BoundingBox& structureBB, int minX, int minY, int minZ,
                                  int maxX, int maxY, int maxZ, bool alwaysReplace, RNG& rng,
                                  Stones& stones) {
        for (int i = minY; i <= maxY; ++i) {
            for (int j = minX; j <= maxX; ++j) {
                for (int k = minZ; k <= maxZ; ++k) {
                    if (!alwaysReplace ||
                        !lce::blocks::ids::isReplaceableBlock(getBlockStateFromPos(worldIn, j, i, k, structureBB)->getID())) {
                        const lce::blocks::Block* block = stones.selectBlocks(rng,
                            i == minY || i == maxY || j == minX || j == maxX || k == minZ || k == maxZ);
                        setBlockState(worldIn, block, j, i, k, structureBB);
                    }
                }
            }
        }
    }


    friend std::ostream& operator<<(std::ostream& out, const StructureComponent& structureComponent);
};