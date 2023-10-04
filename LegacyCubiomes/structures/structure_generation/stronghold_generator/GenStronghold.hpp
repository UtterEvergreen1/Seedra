#pragma once

#include "Enums.hpp"
#include "PieceWeight.hpp"
#include "PiecePlaceCount.hpp"
#include "Piece.hpp"

namespace stronghold_generator {
    class StrongholdGenerator {
    public:
        PiecePlaceCount piecePlaceCounts[11]{};
        GenerationStep generationStep = GenerationStep::FULL;
        BoundingBox structureBoundingBox;
        bool generationStopped = false;

        int startX = 0;
        int startZ = 0;

        PieceType forcedPiece = PieceType::NONE;
        PieceType previousPiece = PieceType::NONE;
        Piece* portalRoomPiece = nullptr;

        Piece pieceArray[1024]{};
        int pieceArraySize = 0;

        /// holds the pieceArraySize when set
        int pendingPieceArray[1024]{};
        int pendingPiecesArraySize = 0;


        Piece* altarChestsArray[4]{};
        int altarChestArraySize = 0;

        int totalWeight = 145;
        int piecePlaceCountsSize = 11;

        StrongholdGenerator();

        void generate(int64_t worldSeed, int chunkX, int chunkZ);
        inline void generate(int64_t worldSeed, Pos2D chunkPos) {return generate(worldSeed, chunkPos.x, chunkPos.z);}

        void resetPieces();

        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        Piece createPiece(PieceType pieceType, uint64_t* rng, DIRECTION direction, int depth, BoundingBox boundingBox);
        void addPiece(Piece piece);
        bool tryAddPieceFromType(PieceType pieceType, uint64_t* rng, Pos3D pos, DIRECTION direction, int depth);
        void genAndAddPiece(uint64_t* rng, Pos3D pos, DIRECTION direction, int depth);

        /// piece gen
        bool genPieceFromSmallDoor(uint64_t* rng, Pos3D pos, DIRECTION direction, int depth);
        void genSmallDoorChildForward(Piece &piece, uint64_t* rng, int n, int n2);
        void genSmallDoorChildLeft(Piece &piece, uint64_t* rng, int n, int n2);
        void genSmallDoorChildRight(Piece &piece, uint64_t* rng, int n, int n2);

        void addChildren(Piece &piece, uint64_t* rng);

        Piece* findCollisionPiece(BoundingBox &boundingBox);
        bool collidesWithPiece(BoundingBox &boundingBox);

        static BoundingBox createPieceBoundingBox(PieceType pieceType, Pos3D pos, DIRECTION dir);
        static bool isOkBox(BoundingBox &boundingBox);
    };
}
