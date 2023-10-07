#pragma once

#include "LegacyCubiomes/cubiomes/processor.hpp"
#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"

class Piece : public BoundingBox {
public:
    DIRECTION orientation;
    int type;
    int depth;
    int additionalData;

    Piece() : BoundingBox(), type(0), depth(0), additionalData(0) {}

    Piece(BoundingBox boundingBox, DIRECTION orientation) : BoundingBox(boundingBox), orientation(orientation), type(0), depth(0), additionalData(0) {}

    Piece(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, DIRECTION orientation) : BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), orientation(orientation), type(0), depth(0), additionalData(0) {}

    Piece(int type, int depth, const BoundingBox& boundingBox, DIRECTION orientation, int additionalData)
            : BoundingBox(boundingBox), orientation(orientation), type(type), depth(depth), additionalData(additionalData) {}

    ND MU Pos2D getBoundingBoxCenter() const;
    ND int getWorldX(int offsetWidth, int offsetDepth) const;
    ND inline int getWorldY(int offsetHeight) const {return minY + offsetHeight;}
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;
    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;
    MU static BoundingBox makeBoundingBox(int x, int y, int z, DIRECTION direction, int width, int height, int depth);
};
