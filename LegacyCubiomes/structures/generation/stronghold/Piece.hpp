#pragma once

#include "LegacyCubiomes/building_blocks/BasePiece.hpp"
#include "Enums.hpp"

namespace stronghold_generator {
    class Piece : public BasePiece {
    public:
        PieceType type;
        int depth;
        int additionalData;

        Piece() : BasePiece(), type(PieceType::NONE), depth(0), additionalData(0) {}
        Piece(PieceType type, int depth, const BoundingBox& boundingBox, DIRECTION orientation, int additionalData)
                :  BasePiece(boundingBox, orientation), type(type), depth(depth), additionalData(additionalData) {}
    };
}
