#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "lce/processor.hpp"

#include "PieceType.hpp"


class Piece : public BoundingBox {
public:
    enumFacing facing;
    PieceType type;
    i8 depth;
    i32 data;

    Piece() : facing(), type(PT_NONE), depth(0), data(0) {}

    Piece(const BoundingBox bb, const enumFacing facing)
        : BoundingBox(bb), facing(facing), type(PT_NONE), depth(0), data(0) {}

    Piece(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ, const enumFacing facing)
        : BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), facing(facing), type(PT_NONE), depth(0), data(0) {}

    Piece(const PieceType type, c_i8 depth, const BoundingBox& bb, const enumFacing facing, c_int data)
        : BoundingBox(bb), facing(facing), type(type), depth(depth), data(data) {}

    MU ND Pos2D getBoundingBoxCenter() const;
    ND int getWorldX(int offsetWidth, int offsetDepth) const;
    ND int getWorldY(int offsetHeight) const;
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;
    MU ND Pos3D getWorldXYZ(int offsetWidth, int offsetHeight, int offsetDepth) const;

    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;
    MU static BoundingBox makeBoundingBox(int x, int y, int z, enumFacing direction, int width, int height, int depth);














};
