#pragma once

namespace village_generator {
    struct PieceWeight {
        int pieceType;
        int weight;
        int PlaceCountMin;
        int PlaceCountMax;
    };
    struct finalPieceWeight {
        int pieceType;
        int weight;
        int maxPlaceCount;
        int amountPlaced;
        finalPieceWeight(int pieceType, int weight, int maxPlaceCount, int amountPlaced)
                : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {}
    };
}