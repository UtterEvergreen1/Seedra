#pragma once

namespace stronghold_generator {
    struct PieceWeight {
        static const PieceWeight PIECE_WEIGHTS[11];

        int weight;
        int maxPlaceCount;
        int minDepth;
    };

    inline const PieceWeight PieceWeight::PIECE_WEIGHTS[] = {
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
}
