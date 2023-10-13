#pragma once

#include <map>
#include <string>

#include "LegacyCubiomes/utils/rng.hpp"
#include "LegacyCubiomes/utils/processor.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"


namespace generation {
    class Stronghold {
    public:

        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################

        enum class GenerationStep {
            PORTAL, // Generates the layout up to the portal room, Y level will be inaccurate
            LAYOUT, // Generates full layout, Y level will be inaccurate
            FULL,   // Generates full layout and calculates Y level for all pieces
        };

        enum PieceType {
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


        const std::map<PieceType, std::string> PieceTypeName = {
                { PieceType::NONE, "NONE -> something went wrong" },
                { PieceType::STRAIGHT, "STRAIGHT" },
                { PieceType::PRISON_HALL, "PRISON_HALL" },
                { PieceType::LEFT_TURN, "LEFT_TURN" },
                { PieceType::RIGHT_TURN, "RIGHT_TURN" },
                { PieceType::ROOM_CROSSING, "ROOM_CROSSING" },
                { PieceType::STRAIGHT_STAIRS_DOWN, "STRAIGHT_STAIRS_DOWN" },
                { PieceType::STAIRS_DOWN, "STAIRS_DOWN" },
                { PieceType::FIVE_CROSSING, "FIVE_CROSSING" },
                { PieceType::CHEST_CORRIDOR, "CHEST_CORRIDOR" },
                { PieceType::LIBRARY, "LIBRARY" },
                { PieceType::PORTAL_ROOM, "PORTAL_ROOM" },
                { PieceType::FILLER_CORRIDOR, "FILLER_CORRIDOR" }
        };

        static inline const PieceWeight PIECE_WEIGHTS[] = {
                {  0, 0, 0 }, // none
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

        struct PiecePlaceCount {


            PieceType pieceType;
            int placeCount;

            ND static const PieceWeight* get(PieceType pieceType) {
                return &PIECE_WEIGHTS[pieceType];
            }

            ND bool isValid() const {
                int maxPlaceCount = PIECE_WEIGHTS[pieceType].maxPlaceCount;
                return maxPlaceCount == 0 || placeCount < maxPlaceCount;
            }

            ND bool canPlace(int depth) const {
                return isValid() && depth >= PIECE_WEIGHTS[pieceType].minDepth;
            }
        };

        static inline const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[11] = {
                { PieceType::STRAIGHT            , 0},
                { PieceType::PRISON_HALL         , 0},
                { PieceType::LEFT_TURN           , 0},
                { PieceType::RIGHT_TURN          , 0},
                { PieceType::ROOM_CROSSING       , 0},
                { PieceType::STRAIGHT_STAIRS_DOWN, 0},
                { PieceType::STAIRS_DOWN         , 0},
                { PieceType::FIVE_CROSSING       , 0},
                { PieceType::CHEST_CORRIDOR      , 0},
                { PieceType::LIBRARY             , 0},
                { PieceType::PORTAL_ROOM         , 0}
        };

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################

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

        Stronghold();

        void generate(int64_t worldSeed, int chunkX, int chunkZ);
        inline void generate(int64_t worldSeed, Pos2D chunkPos) {return generate(worldSeed, chunkPos.x, chunkPos.z);}

        void resetPieces();

        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        Piece createPiece(PieceType pieceType, RNG& rng, DIRECTION direction, int depth, BoundingBox boundingBox);
        void addPiece(Piece piece);
        bool tryAddPieceFromType(PieceType pieceType, RNG& rng, Pos3D pos, DIRECTION direction, int depth);
        void genAndAddPiece(RNG& rng, Pos3D pos, DIRECTION direction, int depth);

        /// piece gen
        bool genPieceFromSmallDoor(RNG& rng, Pos3D pos, DIRECTION direction, int depth);
        void genSmallDoorChildForward(Piece &piece, RNG& rng, int n, int n2);
        void genSmallDoorChildLeft(Piece &piece, RNG& rng, int n, int n2);
        void genSmallDoorChildRight(Piece &piece, RNG& rng, int n, int n2);

        void addChildren(Piece &piece, RNG& rng);

        Piece* findCollisionPiece(BoundingBox &boundingBox);
        bool collidesWithPiece(BoundingBox &boundingBox);

        static BoundingBox createPieceBoundingBox(PieceType pieceType, Pos3D pos, DIRECTION dir);
        static bool isOkBox(BoundingBox &boundingBox);
    };
}
