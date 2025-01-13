#pragma once

#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/GenerationStep.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"
#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/processor.hpp"


namespace gen {
    class Stronghold {
    public:
        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################


        struct PiecePlaceCount {
            PieceType pieceType;
            int placeCount;

            ND static const PieceWeight* get(const PieceType pieceType) {
                return &PIECE_WEIGHTS[pieceType];
            }

            ND bool isValid() const {
                c_int maxPlaceCount = PIECE_WEIGHTS[pieceType].maxPlaceCount;
                return maxPlaceCount == 0 || placeCount < maxPlaceCount;
            }

            ND bool canPlace(c_int depth) const {
                return isValid() && depth >= PIECE_WEIGHTS[pieceType].minDepth;
            }
        };



        static const PieceWeight PIECE_WEIGHTS[12];
        static const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[11];

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################
    private:
        static constexpr u32 ARRAY_SIZE = 512;
        MU static std::string PIECE_TYPE_NAMES[13];
        RNG rng;


        bool generatedPortalRoomSpawner;

    public:
        StructureComponent pieceArray[ARRAY_SIZE]{};
        int pendingPieceArray[ARRAY_SIZE]{};
        PiecePlaceCount piecePlaceCounts[11]{};
        StructureComponent* altarChestsArray[4]{};

        BoundingBox structureBB;

        StructureComponent* portalRoomPiece = nullptr;
        int eyesCount = 0; // number of eyes in portal room (to be populated by the rolls)

        Pos2D startPos;
        // int startX = 0;
        // int startZ = 0;
        int pieceArraySize = 0;
        int pendingPiecesArraySize = 0;
        int altarChestArraySize = 0;
        int totalWeight = 145;
        int piecePlaceCountsSize = 11;

        GenerationStep generationStep = GS_Stronghold_Full;
        PieceType forcedPiece = PT_NONE;
        PieceType previousPiece = PT_NONE;
        bool generationStopped = false;

        Stronghold();

        void reset();


        void generate(i64 worldSeed, int chunkX, int chunkZ);
        void generate(c_i64 worldSeed, Pos2D chunkPos);

        MU static std::string getPieceName(PieceType pieceType);




    private:

        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        void createPiece(PieceType pieceType, FACING facing, i8 depth, BoundingBox boundingBox);

        bool tryAddPieceFromType(PieceType pieceType, const Pos3D& pos, FACING facing, i8 depth);
        void genAndAddPiece(const Pos3D& pos, FACING facing, i8 depth);

        /// piece gen
        bool genPieceFromSmallDoor(const Pos3D& pos, FACING facing, i8 depth);
        void genSmallDoorChildForward(const Piece& piece, int n, int n2);
        void genSmallDoorChildLeft(const Piece& piece, int n, int n2);
        void genSmallDoorChildRight(const Piece& piece, int n, int n2);

        void addChildren(const Piece& piece);

        Piece* findCollisionPiece(const BoundingBox& boundingBox);
        bool collidesWithPiece(const BoundingBox& boundingBox);

        static BoundingBox createPieceBoundingBox(PieceType pieceType, const Pos3D& pos, FACING facing);
        static bool isOkBox(const BoundingBox& boundingBox);
    };
} // namespace gen
