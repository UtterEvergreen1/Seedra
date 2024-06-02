#pragma once

#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "lce/processor.hpp"


namespace generation {
    class Stronghold {
    public:
        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################

        enum class GenerationStep : int8_t {
            PORTAL, // Generates the layout up to the portal room, Y level will be inaccurate
            LAYOUT, // Generates full layout, Y level will be inaccurate
            FULL,   // Generates full layout and calculates Y level for all pieces
        };

        enum class PieceType : int8_t {
            NONE,
            STRAIGHT,
            PRISON_HALL,
            LEFT_TURN,
            RIGHT_TURN,
            ROOM_CROSSING,
            STRAIGHT_STAIRS_DOWN,
            STAIRS_DOWN,
            FIVE_CROSSING,
            CHEST_CORRIDOR,
            LIBRARY,
            PORTAL_ROOM,
            FILLER_CORRIDOR,
        };

        struct PiecePlaceCount {
            PieceType pieceType;
            int placeCount;

            ND static const PieceWeight* get(const PieceType pieceType) {
                return &PIECE_WEIGHTS[static_cast<int8_t>(pieceType)];
            }

            ND bool isValid() const {
                const int maxPlaceCount = PIECE_WEIGHTS[static_cast<int8_t>(pieceType)].maxPlaceCount;
                return maxPlaceCount == 0 || placeCount < maxPlaceCount;
            }

            ND bool canPlace(const int depth) const {
                return isValid() && depth >= PIECE_WEIGHTS[static_cast<int8_t>(pieceType)].minDepth;
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
        Piece pieceArray[512]{};
        int pendingPieceArray[512]{};
        PiecePlaceCount piecePlaceCounts[11]{};
        Piece* altarChestsArray[4]{};

        BoundingBox structureBoundingBox;

        Piece* portalRoomPiece = nullptr;

        int startX = 0;
        int startZ = 0;
        int pieceArraySize = 0;
        int pendingPiecesArraySize = 0;
        int altarChestArraySize = 0;
        int totalWeight = 145;
        int piecePlaceCountsSize = 11;

        GenerationStep generationStep = GenerationStep::FULL;
        PieceType forcedPiece = PieceType::NONE;
        PieceType previousPiece = PieceType::NONE;
        bool generationStopped = false;

        Stronghold();

        /**
        * \n
        * Generates a stronghold with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkX x coordinate of the chunk
        * @param chunkZ z coordinate of the chunk
        */
        void generate(int64_t worldSeed, int chunkX, int chunkZ);

        /**
        * \n
        * Overload function. Generates a stronghold with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkPos coordinates of the chunk
        */
        void generate(const int64_t worldSeed, const Pos2D chunkPos) {
            return generate(worldSeed, chunkPos.x, chunkPos.z);
        }

        void resetPieces();

        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        void createPiece(PieceType pieceType, DIRECTION direction, int8_t depth, BoundingBox boundingBox);

        bool tryAddPieceFromType(PieceType pieceType, const Pos3D& pos, DIRECTION direction, int8_t depth);
        void genAndAddPiece(const Pos3D& pos, DIRECTION direction, int8_t depth);

        /// piece gen
        bool genPieceFromSmallDoor(const Pos3D& pos, DIRECTION direction, int8_t depth);
        void genSmallDoorChildForward(const Piece& piece, int n, int n2);
        void genSmallDoorChildLeft(const Piece& piece, int n, int n2);
        void genSmallDoorChildRight(const Piece& piece, int n, int n2);

        void addChildren(const Piece& piece);

        Piece* findCollisionPiece(const BoundingBox& boundingBox);
        bool collidesWithPiece(const BoundingBox& boundingBox);

        static BoundingBox createPieceBoundingBox(PieceType pieceType, const Pos3D& pos, DIRECTION facing);
        static bool isOkBox(const BoundingBox& boundingBox);
    };
} // namespace generation
