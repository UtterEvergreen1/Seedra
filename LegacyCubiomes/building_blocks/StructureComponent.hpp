#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/blocks/block.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/utils/RotationAndMirror.hpp"


class World;
class ChunkPrimer;

class StructureComponent final : public Piece {
public:
    mutable int structureType{};
    Rotation rotation = Rotation::NONE;
    Mirror mirror = Mirror::NONE;

    StructureComponent() = default;

    MU StructureComponent(const BoundingBox bb, const enumFacing facing)
        : Piece(bb, facing) {
        setCoordMode(facing);
    }

    MU StructureComponent(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                          const enumFacing facing)
        : Piece(minX, minY, minZ, maxX, maxY, maxZ, facing) {
        setCoordMode(facing);
    }

    MU StructureComponent(const PieceType type, c_i8 depth, const BoundingBox &bb, const enumFacing facing, c_int data)
        : Piece(type, depth, bb, facing, data) {
        setCoordMode(facing);
    }

    void setCoordMode(const enumFacing facing) {
        switch (facing) {
            case enumFacing::NORTH:
            default:
                rotation = Rotation::NONE;
                mirror = Mirror::NONE;
                break;
            case enumFacing::EAST:
                rotation = Rotation::CLOCKWISE_90;
                mirror = Mirror::NONE;
                break;
            case enumFacing::SOUTH:
                rotation = Rotation::NONE;
                mirror = Mirror::LEFT_RIGHT;
                break;
            case enumFacing::WEST:
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
        structureType = scIn.structureType;
    }

    MU static void setBlockStateWithoutOffset(
        World &worldIn, const lce::blocks::Block *blockStateIn, int x, int y, int z, const BoundingBox &structureBB);

    MU void setBlockState(
        World &worldIn, const lce::blocks::Block *blockStateIn, int x, int y, int z,
        const BoundingBox &structureBB) const;

    void setBlockState(
        World &worldIn, const lce::blocks::Block &blockStateIn, int x, int y, int z,
        const BoundingBox &structureBB) const;


    MU ND const lce::blocks::Block *getBlockStateFromPos(
        World &worldIn, int x, int y, int z, const BoundingBox &boundingBoxIn) const;


    void clearCurrentPositionBlocksUpwards(
        World &worldIn, int x, int y, int z, const BoundingBox &structureBB) const;


    void replaceAirAndLiquidDownwards(
        World &worldIn, const lce::blocks::Block *blockStateIn, int x, int y, int z,
        const BoundingBox &boundingBoxIn) const;

    void fillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, const lce::blocks::Block *boundaryBlockState,
                        const lce::blocks::Block *insideBlockState, bool existingOnly) const;

    void fillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, const lce::blocks::Block *theBlockState, bool existingOnly) const;

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
                                    int maxY, int maxZ, const lce::blocks::Block *blockStateIn, bool excludeAir) const;


    int getLightLevelAtBlock(MU World &world, MU int x, MU int y, MU int z, MU const BoundingBox &structureBB) const;

    void randomlyPlaceBlock(World &worldIn, const BoundingBox &bbIn, RNG &rand, float chance, int x, int y, int z,
                            const lce::blocks::Block *blockStateIn) const;


    void fillWithBlocksRandomLightCheck(World &world, const BoundingBox &structureBB, RNG &rng, float chance, int minX,
                                        int minY, int minZ, int maxX, int maxY, int maxZ,
                                        const lce::blocks::Block *blockState1, const lce::blocks::Block *blockState2,
                                        bool flag,
                                        int lightLevel) const;


    void fillWithRandomizedStrongholdStones(World &worldIn, const BoundingBox &structureBB, int minX, int minY,
                                            int minZ, int maxX, int maxY, int maxZ, bool alwaysReplace,
                                            RNG &rng) const;

    friend std::ostream &operator<<(std::ostream &out, const StructureComponent &structureComponent);
};
