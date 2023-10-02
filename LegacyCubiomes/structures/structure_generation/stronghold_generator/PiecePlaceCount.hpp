#pragma once

#include "Enums.hpp"
#include "PieceWeight.hpp"

#include "LegacyCubiomes/cubiomes/processor.hpp"

namespace stronghold_generator {
    struct PiecePlaceCount {
        static const PiecePlaceCount DEFAULT[11];

        PieceType pieceType;
        int placeCount;

        ND bool isValid() const {
            int maxPlaceCount = PieceWeight::PIECE_WEIGHTS[static_cast<int>(pieceType)].maxPlaceCount;
            return maxPlaceCount == 0 || placeCount < maxPlaceCount;
        }

        ND bool canPlace(int depth) const {
            return isValid() && depth >= PieceWeight::PIECE_WEIGHTS[static_cast<int>(pieceType)].minDepth;
        }
    };

    inline const PiecePlaceCount PiecePlaceCount::DEFAULT[] = {
            { PieceType::STRAIGHT            , 0},
            { PieceType::PRISON_HALL         , 0},
            { PieceType::LEFT_TURN           , 0},
            { PieceType::RIGHT_TURN          , 0},
            { PieceType::ROOM_CROSSING       , 0},
            { PieceType::STRAIGHT_STAIRS_DOWN, 0},
            { PieceType::STAIRS_DOWN         , 0},
            { PieceType::FIVE_CROSSING       , 0},
            { PieceType::CHEST_CORRIDOR      , 0},
            { PieceType::LIBRARY             , 0},
            { PieceType::PORTAL_ROOM         , 0}
    };


}

