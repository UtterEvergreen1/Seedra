#pragma once

#include "StructureVariant.hpp"
#include "BoundingBox.hpp"
#include "Piece.hpp"

#include "lce/blocks/block.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/RotationAndMirror.hpp"


class World;
class ChunkPrimer;
class RNG;

class StructureComponent final : public Piece {
public:
    mutable StructureVariant variant{};
    Rotation rotation = Rotation::NONE;
    Mirror mirror = Mirror::NONE;

    StructureComponent() = default;

    MU StructureComponent(const BoundingBox bb, const EnumFacing facing)
        : Piece(bb, facing) {
        setCoordMode(facing);
    }

    MU StructureComponent(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                          const EnumFacing facing)
        : Piece(minX, minY, minZ, maxX, maxY, maxZ, facing) {
        setCoordMode(facing);
    }

    MU StructureComponent(const PieceType type, c_i8 depth, const BoundingBox &bb, const EnumFacing facing, c_int data)
        : Piece(type, depth, bb, facing, data) {
        setCoordMode(facing);
    }

    void setCoordMode(const EnumFacing facing) {
        switch (facing) {
            case EnumFacing::NORTH:
            default:
                rotation = Rotation::NONE;
                mirror = Mirror::NONE;
                break;
            case EnumFacing::EAST:
                rotation = Rotation::CLOCKWISE_90;
                mirror = Mirror::NONE;
                break;
            case EnumFacing::SOUTH:
                rotation = Rotation::NONE;
                mirror = Mirror::LEFT_RIGHT;
                break;
            case EnumFacing::WEST:
                rotation = Rotation::CLOCKWISE_90;
                mirror = Mirror::LEFT_RIGHT;
                break;
        }
    }

    MU void usingPiece(const StructureComponent &scIn) {
        minX = scIn.minX;
        maxX = scIn.maxX;
        minY = scIn.minY;
        maxY = scIn.maxY;
        minZ = scIn.minZ;
        maxZ = scIn.maxZ;
        type = scIn.type;
        facing = scIn.facing;
        rotation = scIn.rotation;
        mirror = scIn.mirror;
        data = scIn.data;
        depth = scIn.depth;
        variant = scIn.variant;
    }

    MU static void setBlockStateWithoutOffset(
        World &worldIn, const lce::Block *blockStateIn, int x, int y, int z, const BoundingBox &structureBB);

    MU void setBlockState(
        World &worldIn, const lce::Block *blockStateIn, int x, int y, int z,
        const BoundingBox &structureBB) const;

    void setBlockState(
        World &worldIn, const lce::Block &blockStateIn, int x, int y, int z,
        const BoundingBox &structureBB) const;


    MU ND const lce::Block *getBlockStateFromPos(
        World &worldIn, int x, int y, int z, const BoundingBox &boundingBoxIn) const;


    void clearCurrentPositionBlocksUpwards(
        World &worldIn, int x, int y, int z, const BoundingBox &structureBB) const;


    void replaceAirAndLiquidDownwards(
        World &worldIn, const lce::Block *blockStateIn, int x, int y, int z,
        const BoundingBox &boundingBoxIn) const;

    void fillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, const lce::Block *boundaryBlockState,
                        const lce::Block *insideBlockState, bool existingOnly) const;

    void fillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, const lce::Block *theBlockState, bool existingOnly) const;

    void fillWithAir(
        World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) const;


    static bool isLiquidInStructureBoundingBox(const BoundingBox &chunkBoundingBoxIn,
                                               const StructureComponent &piece, const ChunkPrimer *chunk);

    static bool intersectsWithBlock(const BoundingBox &chunkBoundingBox, int x, int y, int z);

    MU static bool validToPlace(const BoundingBox &chunkBoundingBox, const BoundingBox &bb, int x, int y, int z);

    static void fillWithRandomizedBlocksRNGOnly(const BoundingBox &chunkBoundingBox, const StructureComponent &piece,
                                                int minX, int minY,
                                                int minZ, int maxX, int maxY, int maxZ, RNG &rng,
                                                const ChunkPrimer *chunk);

    static void generateChest(const BoundingBox &chunkBB, const StructureComponent &piece, RNG &rng, int x, int y,
                              int z);


    bool isLiquidInStructureBoundingBox(World &worldIn, const BoundingBox &bbIn) const;


    void randomlyRareFillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX,
                                    int maxY, int maxZ, const lce::Block *blockStateIn, bool excludeAir) const;


    int getLightLevelAtBlock(MU World &world, MU int x, MU int y, MU int z, MU const BoundingBox &structureBB) const;

    void randomlyPlaceBlock(World &worldIn, const BoundingBox &bbIn, RNG &rand, float chance, int x, int y, int z,
                            const lce::Block *blockStateIn) const;


    void fillWithBlocksRandomLightCheck(World &world, const BoundingBox &structureBB, RNG &rng, float chance, int minX,
                                        int minY, int minZ, int maxX, int maxY, int maxZ,
                                        const lce::Block *blockState1, const lce::Block *blockState2,
                                        bool flag,
                                        int lightLevel) const;


    void fillWithRandomizedStrongholdStones(World &worldIn, const BoundingBox &structureBB, int minX, int minY,
                                            int minZ, int maxX, int maxY, int maxZ, bool alwaysReplace,
                                            RNG &rng) const;

    friend std::ostream &operator<<(std::ostream &out, const StructureComponent &structureComponent);
};
