#pragma once



#include "LegacyCubiomes/cubiomes/processor.hpp"
#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"

namespace stronghold_generator {

    inline const PieceWeight PIECE_WEIGHTS[] = {
            { 40, 0, 0 },
            {  5, 5, 0 },
            { 20, 0, 0 },
            { 20, 0, 0 },
            { 10, 6, 0 },
            {  5, 5, 0 },
            {  5, 5, 0 },
            {  5, 4, 0 },
            {  5, 4, 0 },
            { 10, 2, 5 },
            { 20, 1, 6 },
    };


    struct PiecePlaceCount {


        PieceType pieceType;
        int placeCount;

        ND static const PieceWeight* get(PieceType pieceType) {
            return &PIECE_WEIGHTS[pieceType];
        }

        ND bool isValid() const {
            int maxPlaceCount = PIECE_WEIGHTS[pieceType].maxPlaceCount;
            return maxPlaceCount == 0 || placeCount < maxPlaceCount;
        }

        ND bool canPlace(int depth) const {
            return isValid() && depth >= PIECE_WEIGHTS[pieceType].minDepth;
        }
    };

    inline const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[11] = {
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

