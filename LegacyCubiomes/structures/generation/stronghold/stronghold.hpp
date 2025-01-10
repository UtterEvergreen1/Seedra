#pragma once

#include <map>
#include <string>

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

        enum class GenerationStep : i8 {
            PORTAL, // Generates the layout up to the portal room, Y level will be inaccurate
            LAYOUT, // Generates full layout, Y level will be inaccurate
            FULL,   // Generates full layout and calculates Y level for all pieces
        };

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

        static const std::map<PieceType, std::string> PieceTypeName;
        static const PieceWeight PIECE_WEIGHTS[12];
        static const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[11];

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################
    private:
        RNG rng;

    public:
        StructureComponent pieceArray[512]{};
        int pendingPieceArray[512]{};
        PiecePlaceCount piecePlaceCounts[11]{};
        StructureComponent* altarChestsArray[4]{};

        BoundingBox structureBoundingBox;

        StructureComponent* portalRoomPiece = nullptr;
        int eyesCount = 0; // number of eyes in portal room (to be populated by the rolls)

        int startX = 0;
        int startZ = 0;
        int pieceArraySize = 0;
        int pendingPiecesArraySize = 0;
        int altarChestArraySize = 0;
        int totalWeight = 145;
        int piecePlaceCountsSize = 11;

        GenerationStep generationStep = GenerationStep::FULL;
        PieceType forcedPiece = NONE;
        PieceType previousPiece = NONE;
        bool generationStopped = false;

        Stronghold();

        /**
        * \n
        * Generates a stronghold with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkX x coordinate of the chunk
        * @param chunkZ z coordinate of the chunk
        */
        void generate(i64 worldSeed, int chunkX, int chunkZ);

        /**
        * \n
        * Overload function. Generates a stronghold with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkPos coordinates of the chunk
        */
        void generate(c_i64 worldSeed, const Pos2D chunkPos) {
            return generate(worldSeed, chunkPos.x, chunkPos.z);
        }

        void resetPieces();

        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        void createPiece(PieceType pieceType, FACING direction, i8 depth, BoundingBox boundingBox);

        bool tryAddPieceFromType(PieceType pieceType, const Pos3D& pos, FACING direction, i8 depth);
        void genAndAddPiece(const Pos3D& pos, FACING direction, i8 depth);

        /// piece gen
        bool genPieceFromSmallDoor(const Pos3D& pos, FACING direction, i8 depth);
        void genSmallDoorChildForward(const Piece& piece, int n, int n2);
        void genSmallDoorChildLeft(const Piece& piece, int n, int n2);
        void genSmallDoorChildRight(const Piece& piece, int n, int n2);

        void addChildren(const Piece& piece);

        Piece* findCollisionPiece(const BoundingBox& boundingBox);
        bool collidesWithPiece(const BoundingBox& boundingBox);

        static BoundingBox createPieceBoundingBox(PieceType pieceType, const Pos3D& pos, FACING facing);
        static bool isOkBox(const BoundingBox& boundingBox);
    };
} // namespace generation
