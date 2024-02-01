#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/processor.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"


class Piece : public BoundingBox {
public:
    DIRECTION orientation;
    int8_t type;
    int8_t depth;

    int32_t additionalData;

    Piece() : orientation(), type(0), depth(0), additionalData(0) {}

    Piece(const BoundingBox boundingBox, const DIRECTION orientation)
        : BoundingBox(boundingBox), orientation(orientation), type(0), depth(0), additionalData(0) {}

    Piece(const int minX, const int minY, const int minZ, const int maxX, const int maxY, const int maxZ,
          const DIRECTION orientation)
        : BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), orientation(orientation), type(0), depth(0),
          additionalData(0) {}

    Piece(const int8_t type, const int8_t depth, const BoundingBox& boundingBox,
          const DIRECTION orientation, const int additionalData)
        : BoundingBox(boundingBox), orientation(orientation), type(type), depth(depth), additionalData(additionalData) {
    }

    ND MU Pos2D getBoundingBoxCenter() const;
    ND int getWorldX(int offsetWidth, int offsetDepth) const;
    ND int getWorldY(const int offsetHeight) const { return minY + offsetHeight; }
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;
    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;
    MU static BoundingBox makeBoundingBox(int x, int y, int z, DIRECTION direction, int width, int height, int depth);
};
