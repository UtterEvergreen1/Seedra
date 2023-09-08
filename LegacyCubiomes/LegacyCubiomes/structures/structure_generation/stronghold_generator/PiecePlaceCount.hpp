#pragma once

#include "Enums.hpp"
#include "PieceWeight.hpp"
namespace stronghold_generator {
    struct PiecePlaceCount {
        static const PiecePlaceCount DEFAULT[11];

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
}

inline const stronghold_generator::PiecePlaceCount stronghold_generator::PiecePlaceCount::DEFAULT[] = {
        { stronghold_generator::PieceType::STRAIGHT            , 0},
        { stronghold_generator::PieceType::PRISON_HALL         , 0},
        { stronghold_generator::PieceType::LEFT_TURN           , 0},
        { stronghold_generator::PieceType::RIGHT_TURN          , 0},
        { stronghold_generator::PieceType::ROOM_CROSSING       , 0},
        { stronghold_generator::PieceType::STRAIGHT_STAIRS_DOWN, 0},
        { stronghold_generator::PieceType::STAIRS_DOWN         , 0},
        { stronghold_generator::PieceType::FIVE_CROSSING       , 0},
        { stronghold_generator::PieceType::CHEST_CORRIDOR      , 0},
        { stronghold_generator::PieceType::LIBRARY             , 0},
        { stronghold_generator::PieceType::PORTAL_ROOM         , 0}
};
