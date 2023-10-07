#pragma once

#include "LegacyCubiomes/building_blocks/BasePiece.hpp"
#include "Enums.hpp"

namespace mineshaft_generator {
    class Piece : public BasePiece {
    public:
        int type;
        int depth;
        int additionalData;


        Piece() : BasePiece(), type(PieceType::NONE), depth(0), additionalData(0) {}
        Piece(int type, int depth, const BoundingBox& boundingBox, DIRECTION orientation, int additionalData)
                :  BasePiece(boundingBox, orientation), type(type), depth(depth), additionalData(additionalData) {
        }
    };
}
