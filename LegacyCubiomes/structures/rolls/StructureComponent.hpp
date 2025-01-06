#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "lce/blocks/block.hpp"


class StructureComponent {
    FACING coordBaseMode = FACING::NONE;
    // Mirror mirror;
    // Rotation rotation;


public:
    BoundingBox boundingBox;


    MU void setCoordBaseMode(FACING facing) { coordBaseMode = facing; }
    ND FACING getCoordBaseMode() const { return coordBaseMode; }

    MU ND virtual bool addComponentParts(World& worldIn, RNG& rng, BoundingBox structureBB) = 0;


    MU ND int getXWithOffset(int x, int z) const {
        FACING enumFacing = this->getCoordBaseMode();

        if (enumFacing == FACING::NONE) {
            return x;
        } else {
            switch (enumFacing) {
                case FACING::NORTH:
                case FACING::SOUTH:
                    return this->boundingBox.minX + x;
                case FACING::WEST:
                    return this->boundingBox.maxX - z;
                case FACING::EAST:
                    return this->boundingBox.minX + z;
                default:
                    return x;
            }
        }
    }

    MU ND int getYWithOffset(int y) const { return this->getCoordBaseMode() == FACING::NONE ? y : y + this->boundingBox.minY; }

    MU ND int getZWithOffset(int x, int z) const {
        FACING enumFacing = this->getCoordBaseMode();

        if (enumFacing == FACING::NONE) {
            return z;
        } else {
            switch (enumFacing) {
                case FACING::NORTH:
                    return this->boundingBox.maxZ - z;
                case FACING::SOUTH:
                    return this->boundingBox.minZ + z;
                case FACING::WEST:
                case FACING::EAST:
                    return this->boundingBox.minZ + x;
                default:
                    return z;
            }
        }
    }


    MU void setBlockState(World& worldIn, const lce::blocks::Block* blockStateIn, int x, int y, int z,
                       BoundingBox& structureBB) {
        Pos3D blockPos = Pos3D(this->getXWithOffset(x, z), this->getYWithOffset(y), this->getZWithOffset(x, z));
        if (structureBB.isVecInside(blockPos)) {
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

    void setBlockState(World& worldIn, const lce::blocks::Block& blockStateIn, int x, int y, int z, BoundingBox& structureBB) {
        setBlockState(worldIn, &blockStateIn, x, y, z, structureBB);
    }



    MU ND const lce::blocks::Block* getBlockStateFromPos(World worldIn, int x, int y, int z, const BoundingBox& boundingBoxIn) const {
        int i = this->getXWithOffset(x, z);
        int j = this->getYWithOffset(y);
        int k = this->getZWithOffset(x, z);
        Pos3D blockPos = {i, j, k};
        return !boundingBoxIn.isVecInside(blockPos) ? &lce::blocks::BlocksInit::AIR : worldIn.getBlock(blockPos);
    }



    /**
     * Deletes all continuous blocks from selected position upwards. Stops at hitting air.
     */
    void clearCurrentPositionBlocksUpwards(World worldIn, int x, int y, int z, BoundingBox& structureBB) const {
        Pos3D blockPos(this->getXWithOffset(x, z), this->getYWithOffset(y), this->getZWithOffset(x, z));

        if (structureBB.isVecInside(blockPos)) {
            while (!worldIn.isAirBlock(blockPos) && blockPos.getY() < 255) {
                worldIn.setBlock(blockPos, lce::blocks::ids::AIR_ID); // 2
                blockPos = blockPos.up();
            }
        }
    }


    /**
     * Replaces air and liquid from given position downwards. Stops when hitting anything else than air or liquid
     */
    void replaceAirAndLiquidDownwards(World worldIn, const lce::blocks::Block* blockStateIn, int x, int y, int z,
                                      BoundingBox boundingBoxIn) const {
        int i = this->getXWithOffset(x, z);
        int j = this->getYWithOffset(y);
        int k = this->getZWithOffset(x, z);

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