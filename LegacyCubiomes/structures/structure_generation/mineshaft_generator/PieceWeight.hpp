#pragma once

namespace mineshaft_generator {
    struct PieceWeight {
        static const PieceWeight PIECE_WEIGHTS[4];

        int weight;
        int maxPlaceCount;
        int minDepth;
    };

    inline const PieceWeight PieceWeight::PIECE_WEIGHTS[] = {
            { 0, 0, 0 },
            { 0, 5, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },

    };

}

