#pragma once

#include "Enums.hpp"
#include "PieceWeight.hpp"
#include "PiecePlaceCount.hpp"
#include "Piece.hpp"

namespace mineshaft_generator {
    class MineshaftGenerator {
    public:
        PiecePlaceCount piecePlaceCounts[4];
        // GenerationStep generationStep = GenerationStep::FULL;
        PieceType imposedPiece = PieceType::NONE;
        PieceType previousPiece = PieceType::NONE;
        Piece *minecartChests[4];
        Piece pieces[1024];
        int piecePlaceCountsSize = 0;
        int totalWeight = 0;
        int piecesSize = 0;
        int pendingPieces[1024];
        int pendingPiecesSize = 0;
        int startX = 0;
        int startZ = 0;
        int numMinecartChests = 0;
        bool generationStopped = false;

        MineshaftGenerator();

        void generate(int64_t worldSeed, int chunkX, int chunkZ);
        void generate(int64_t worldSeed, Pos2D chunkPos);

        void resetPieces();
        void updatePieceWeight();
        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        static BoundingBox createPieceBoundingBox(PieceType pieceType, int x, int y, int z, DIRECTION direction);
        Piece createPiece(PieceType pieceType, uint64_t* random, DIRECTION direction, int depth, BoundingBox boundingBox);
        void addPiece(Piece piece);
        bool tryAddPieceFromType(PieceType pieceType, uint64_t* random, int x, int y, int z, DIRECTION direction, int depth);
        void generateAndAddPiece(uint64_t* random, int x, int y, int z, DIRECTION direction, int depth);
        void generateSmallDoorChildForward(Piece &piece, uint64_t* random, int n, int n2);
        void generateSmallDoorChildLeft(Piece &piece, uint64_t* random, int n, int n2);
        void generateSmallDoorChildRight(Piece &piece, uint64_t* random, int n, int n2);
        void addChildren(Piece &piece, uint64_t* random);

        Piece* findCollisionPiece(BoundingBox &boundingBox);

        static bool isOkBox(BoundingBox &boundingBox);
    };
}
