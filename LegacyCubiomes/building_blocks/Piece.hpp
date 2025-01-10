#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "lce/processor.hpp"

#include "PieceType.hpp"


class Piece : public BoundingBox {
public:
    FACING orientation;
    PieceType type;
    i8 depth;

    i32 additionalData;

    Piece() : orientation(), type(NONE), depth(0), additionalData(0) {}

    Piece(const BoundingBox boundingBox, const FACING orientation)
        : BoundingBox(boundingBox), orientation(orientation), type(NONE), depth(0), additionalData(0) {}

    Piece(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
          const FACING orientation)
        : BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), orientation(orientation), type(NONE), depth(0),
          additionalData(0) {}

    Piece(const PieceType type, c_i8 depth, const BoundingBox& boundingBox,
          const FACING orientation, c_int additionalData)
        : BoundingBox(boundingBox), orientation(orientation), type(type), depth(depth), additionalData(additionalData) {
    }

    ND MU Pos2D getBoundingBoxCenter() const;
    ND int getWorldX(int offsetWidth, int offsetDepth) const;
    ND int getWorldY(c_int offsetHeight) const { return minY + offsetHeight; }
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;
    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;
    MU static BoundingBox makeBoundingBox(int x, int y, int z, FACING direction, int width, int height, int depth);
};
