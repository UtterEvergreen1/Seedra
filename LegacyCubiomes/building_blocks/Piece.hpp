#pragma once

#include "LegacyCubiomes/utils/processor.hpp"
#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/Pos2D.hpp"


class Piece : public BoundingBox {
public:
    DIRECTION orientation;
    int8_t type;
    int8_t depth;

    //union {
        int32_t additionalData;

        // TODO: figure out endianness so this doesn't randomly break
        /*struct {
            uint32_t bit0 : 1;
            uint32_t bit1 : 1;
            uint32_t bit2 : 1;
            uint32_t bit3 : 1;
            uint32_t remaining : 28;
        } int1_data;
        struct {
            uint8_t byte0: 8;
            uint8_t byte1: 8;
            uint8_t byte2: 8;
            uint8_t byte3: 8;
        } int8_data;*/
    //};

    Piece() : BoundingBox(), type(0), depth(0), additionalData(0) {}

    Piece(BoundingBox boundingBox, DIRECTION orientation) : BoundingBox(boundingBox), orientation(orientation), type(0), depth(0), additionalData(0) {}

    Piece(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, DIRECTION orientation) : BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), orientation(orientation), type(0), depth(0), additionalData(0) {}

    Piece(int8_t type, int8_t depth, const BoundingBox& boundingBox, DIRECTION orientation, int additionalData)
            : BoundingBox(boundingBox), orientation(orientation), type(type), depth(depth), additionalData(additionalData) {}

    ND MU Pos2D getBoundingBoxCenter() const;
    ND int getWorldX(int offsetWidth, int offsetDepth) const;
    ND inline int getWorldY(int offsetHeight) const {return minY + offsetHeight;}
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;
    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;
    MU static BoundingBox makeBoundingBox(int x, int y, int z, DIRECTION direction, int width, int height, int depth);
};
