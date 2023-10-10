#pragma once

namespace generation {
    struct PieceWeight {
        int pieceType;
        int weight;
        int PlaceCountMin;
        int PlaceCountMax;
    };
    struct FinalPieceWeight {
        int pieceType;
        int weight;
        int maxPlaceCount;
        int amountPlaced;
        FinalPieceWeight(int pieceType, int weight, int maxPlaceCount, int amountPlaced)
                : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {}
    };
}