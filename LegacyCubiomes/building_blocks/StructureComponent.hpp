#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/blocks/block.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"


class StructureComponent : public Piece {
public:
    StructureComponent() = default;

    MU StructureComponent(const BoundingBox boundingBox, const FACING orientation) : Piece(boundingBox, orientation) {}


    MU StructureComponent(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                          const FACING orientation)
        : Piece(minX, minY, minZ, maxX, maxY, maxZ, orientation) {}

    MU StructureComponent(const PieceType type, c_i8 depth, const BoundingBox& boundingBox, const FACING orientation,
                          c_int additionalData)
        : Piece(type, depth, boundingBox, orientation, additionalData) {}

    virtual ~StructureComponent() = default;

    MU ND virtual bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) { return false; }


    MU void setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn, int x, int y, int z,
                          const BoundingBox& structureBB) const;

    void setBlockState(World& worldIn, const lce::blocks::Block& blockStateIn, int x, int y, int z,
                       const BoundingBox& structureBB) const;


    MU ND const lce::blocks::Block* getBlockStateFromPos(World& worldIn, int x, int y, int z,
                                                         const BoundingBox& boundingBoxIn) const;


    /**
     * Deletes all continuous blocks from selected position upwards. Stops at hitting air.
     */
    void clearCurrentPositionBlocksUpwards(World worldIn, int x, int y, int z, const BoundingBox& structureBB) const;


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


    void randomlyRareFillWithBlocks(World worldIn, const BoundingBox& bbIn, int minX, int minY, int minZ, int maxX,
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


    int func_189916_b(World& p_189916_1_, int p_189916_2_, int p_189916_3_, int p_189916_4_,
                      const BoundingBox& p_189916_5_) {
        return 0;
        // int i = this->getWorldX(p_189916_2_, p_189916_4_);
        // int j = this->getWorldY(p_189916_3_ + 1);
        // int k = this->getWorldZ(p_189916_2_, p_189916_4_);
        // Pos3D blockPos(i, j, k);
        // return !p_189916_5_.isVecInside(blockPos) ? EnumSkyBlock.SKY.defaultLightValue : p_189916_1_.getLightFor(EnumSkyBlock.SKY, blockPos);
    }

    void randomlyPlaceBlock(World& worldIn, const BoundingBox& bbIn, RNG& rand, float chance, int x, int y,
                            int z, const lce::blocks::Block* blockStateIn) {
        if (rand.nextFloat() < chance) { setBlockState(worldIn, blockStateIn, x, y, z, bbIn); }
    }


    MU void fillWithRandomizedBlocks(World& worldIn, const BoundingBox& bbIn, int minX, int minY, int minZ, int maxX,
                                     int maxY, int maxZ, bool alwaysReplace, RNG& rand
                                     /*, StructureComponentBlockSelector blockselector*/) {
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


    void func_189914_a(World& p_189914_1_, const BoundingBox& p_189914_2_, RNG& p_189914_3_, float p_189914_4_,
                       int p_189914_5_, int p_189914_6_, int p_189914_7_, int p_189914_8_, int p_189914_9_,
                       int p_189914_10_, const lce::blocks::Block* p_189914_11_, const lce::blocks::Block* p_189914_12_,
                       bool p_189914_13_, int p_189914_14_) {
        for (int i = p_189914_6_; i <= p_189914_9_; ++i) {
            for (int j = p_189914_5_; j <= p_189914_8_; ++j) {
                for (int k = p_189914_7_; k <= p_189914_10_; ++k) {
                    if (p_189914_3_.nextFloat() <= p_189914_4_ &&
                        (!p_189914_13_ || !lce::blocks::ids::isReplaceableBlock(
                                                  getBlockStateFromPos(p_189914_1_, j, i, k, p_189914_2_)->getID())) &&
                        (p_189914_14_ <= 0 || func_189916_b(p_189914_1_, j, i, k, p_189914_2_) < p_189914_14_)) {
                        if (i != p_189914_6_ && i != p_189914_9_ && j != p_189914_5_ && j != p_189914_8_ &&
                            k != p_189914_7_ && k != p_189914_10_) {
                            setBlockState(p_189914_1_, p_189914_12_, j, i, k, p_189914_2_);
                        } else {
                            setBlockState(p_189914_1_, p_189914_11_, j, i, k, p_189914_2_);
                        }
                    }
                }
            }
        }
    }
};