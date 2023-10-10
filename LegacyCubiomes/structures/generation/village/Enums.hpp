#pragma once

#include "PieceWeight.hpp"

namespace generation {

    enum PieceType {
        NONE,
        House4Garden,
        Church,
        House1,
        WoodHut,
        Hall,
        Field1,
        Field2,
        House2,
        House3,
        Torch,
        Start,
        Road,
    };

    const int VILLAGE_SIZE = 0;
    const PieceWeight PIECE_WEIGHTS[9] = {
            {PieceType::House4Garden,    4, 2 + VILLAGE_SIZE,   4 + VILLAGE_SIZE * 2},
            {PieceType::Church,         20, 0 + VILLAGE_SIZE,   1 + VILLAGE_SIZE},
            {PieceType::House1,         20, 0 + VILLAGE_SIZE,   2 + VILLAGE_SIZE},
            {PieceType::WoodHut,         3, 2 + VILLAGE_SIZE,   5 + VILLAGE_SIZE * 3},
            {PieceType::Hall,           15, 0 + VILLAGE_SIZE,   2 + VILLAGE_SIZE},
            { PieceType::Field1,         3, 1 + VILLAGE_SIZE,   4 + VILLAGE_SIZE},
            { PieceType::Field2,         3, 2 + VILLAGE_SIZE,   4 + VILLAGE_SIZE * 2},
            { PieceType::House2,        15, 0,                  1 + VILLAGE_SIZE},
            { PieceType::House3,         8, 0 + VILLAGE_SIZE,   3 + VILLAGE_SIZE * 2}
    };




}