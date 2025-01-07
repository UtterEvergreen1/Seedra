#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "lce/blocks/block.hpp"


class StructureComponent {
public:
    virtual ~StructureComponent() = default;
    Piece piece;

    MU ND virtual bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& structureBB) = 0;


    MU void setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn,
        const int x, const int y, const int z,
                       const BoundingBox& structureBB) const {
        if (const auto blockPos =
                    Pos3D(piece.getWorldX(x, z), piece.getWorldY(y), piece.getWorldZ(x, z));
            structureBB.isVecInside(blockPos)) {
            /*
                if (this->mirror != Mirror::NONE) {
                    blockStateIn = blockStateIn.withMirror(this->mirror);
                }
                if (this->rotation != Rotation::NONE) {
                    blockStateIn = blockStateIn.withRotation(this->rotation);
                }
                 */
            worldIn.setBlock(blockPos, blockStateIn->getID()); // flags = 2
        }
    }

    void setBlockState(World& worldIn, const lce::blocks::Block& blockStateIn,
        const int x, const int y, const int z, const BoundingBox& structureBB) const {
        setBlockState(worldIn, &blockStateIn, x, y, z, structureBB);
    }



    MU ND const lce::blocks::Block* getBlockStateFromPos(World worldIn,
        const int x, const int y, const int z, const BoundingBox& boundingBoxIn) const {
        const int i = piece.getWorldX(x, z);
        const int j = piece.getWorldY(y);
        const int k = piece.getWorldZ(x, z);
        const Pos3D blockPos = {i, j, k};
        return !boundingBoxIn.isVecInside(blockPos) ? &lce::blocks::BlocksInit::AIR : worldIn.getBlock(blockPos);
    }



    /**
     * Deletes all continuous blocks from selected position upwards. Stops at hitting air.
     */
    void clearCurrentPositionBlocksUpwards(World worldIn,
        const int x, const int y, const int z, const BoundingBox& structureBB) const {

        if (Pos3D blockPos(this->piece.getWorldX(x, z), this->piece.getWorldY(y), this->piece.getWorldZ(x, z));
            structureBB.isVecInside(blockPos)) {
            while (!worldIn.isAirBlock(blockPos) && blockPos.getY() < 255) {
                worldIn.setBlock(blockPos, lce::blocks::ids::AIR_ID); // 2
                blockPos = blockPos.up();
            }
        }
    }


    /**
     * Replaces air and liquid from given position downwards. Stops when hitting anything else than air or liquid
     */
    void replaceAirAndLiquidDownwards(World& worldIn, const lce::blocks::Block* blockStateIn,
        const int x, const int y, const int z, const BoundingBox& boundingBoxIn) const {
        const int i = this->piece.getWorldX(x, z);
        int j = this->piece.getWorldY(y);
        const int k = this->piece.getWorldZ(x, z);

        if (boundingBoxIn.isVecInside(Pos3D(i, j, k))) {
            while (
                    (worldIn.isAirBlock(Pos3D(i, j, k)) ||
                     lce::blocks::ids::isLiquidBlock(worldIn.getBlock(i, j, k)->getID())
                             ) && j > 1) {
                worldIn.setBlock(i, j, k, blockStateIn); // 2
                --j;
            }
        }
    }
};