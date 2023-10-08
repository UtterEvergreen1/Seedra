#pragma once


#include "LegacyCubiomes/cubiomes/processor.hpp"
#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"

namespace generation {

    class NetherFortress {
    public:

        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################

        enum PieceType {
            CROSSING3,
            END,
            STRAIGHT,
            CORRIDOR3,
            CORRIDOR4,
            ENTRANCE,
            CROSSING2,
            CORRIDOR,
            CORRIDOR5,
            CORRIDOR2,
            NETHER_STALK_ROOM,
            THRONE,
            CROSSING,
            STAIRS,
            START,
            NONE
        };

        constexpr static const PieceWeight PIECE_WEIGHTS[16] = {
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
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


        const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[16] = {
                {CROSSING3, 0},
                {END, 0},
                {STRAIGHT, 0},
                {CORRIDOR3, 0},
                {CORRIDOR4, 0},
                {ENTRANCE, 0},
                {CROSSING2, 0},
                {CORRIDOR, 0},
                {CORRIDOR5, 0},
                {CORRIDOR2, 0},
                {NETHER_STALK_ROOM, 0},
                {THRONE, 0},
                {CROSSING, 0},
                {STAIRS, 0},
                {START, 0},
                {NONE, 0},
        };

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################

        // ...

    };

}

