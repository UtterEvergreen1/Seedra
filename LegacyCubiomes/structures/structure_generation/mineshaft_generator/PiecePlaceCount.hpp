#pragma once

#include "Enums.hpp"
#include "PieceWeight.hpp"

namespace mineshaft_generator {
    struct PiecePlaceCount {
        static const PiecePlaceCount DEFAULT[4];

        PieceType pieceType;
        int placeCount;

        [[nodiscard]] bool isValid() const {
            int maxPlaceCount = PieceWeight::PIECE_WEIGHTS[static_cast<int>(this->pieceType)].maxPlaceCount;
            return maxPlaceCount == 0 || this->placeCount < maxPlaceCount;
        }

        [[nodiscard]] bool canPlace(int depth) const {
            return this->isValid() && depth >= PieceWeight::PIECE_WEIGHTS[static_cast<int>(this->pieceType)].minDepth;
        }
    };


    inline const PiecePlaceCount PiecePlaceCount::DEFAULT[] = {
            { PieceType::CORRIDOR, 0},
            { PieceType::CROSSING, 0},
            { PieceType::ROOM    , 0},
            { PieceType::STAIRS  , 0},
    };

}
