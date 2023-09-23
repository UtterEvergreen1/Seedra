#pragma once

#include "Enums.hpp"
#include "PieceWeight.hpp"

namespace netherbridge_generator {
    struct PiecePlaceCount {
        static const PiecePlaceCount DEFAULT[16];

        PieceType pieceType;
        int placeCount;

        ND bool isValid() const {
            int maxPlaceCount = PieceWeight::PIECE_WEIGHTS[static_cast<int>(this->pieceType)].maxPlaceCount;
            return maxPlaceCount == 0 || this->placeCount < maxPlaceCount;
        }

        ND bool canPlace(int depth) const {
            return this->isValid() && depth >= PieceWeight::PIECE_WEIGHTS[static_cast<int>(this->pieceType)].minDepth;
        }
    };

    inline const PiecePlaceCount PiecePlaceCount::DEFAULT[] = {
            { PieceType::BRIDGE_STRAIGHT, 0},
            { PieceType::NONE, 0},
            { PieceType::BRIDGE_CROSSING, 0},
            { PieceType::ROOM_CROSSING, 0},
            { PieceType::STAIRS_ROOM, 0},
            { PieceType::MONSTER_THRONE, 0},
            { PieceType::CASTLE_ENTRANCE, 0},
            { PieceType::CASTLE_STALK_ROOM, 0},
            { PieceType::CASTLE_SMALL_CORRIDOR_PIECE, 0},
            { PieceType::CASTLE_CORRIDOR_CROSSING_PIECE, 0},
            { PieceType::CASTLE_SMALL_CORRIDOR_RIGHT_TURN_PIECE, 0},
            { PieceType::CASTLE_SMALL_CORRIDOR_LEFT_TURN_PIECE, 0},
            { PieceType::CASTLE_CORRIDOR_STAIRS_PIECE, 0},
            { PieceType::CASTLE_CORRIDOR_BALCONY_PIECE, 0},
            { PieceType::NONE2, 0},
    };


}












