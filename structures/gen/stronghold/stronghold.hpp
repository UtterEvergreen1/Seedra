#pragma once

#include <map>
#include <string>

#include "components/GenerationStep.hpp"
#include "lce/processor.hpp"
#include "support/rng.hpp"

#include "components/PieceWeight.hpp"
#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"

class World;
class Generator;


namespace gen {

    static constexpr u32 STRONGHOLD_ARRAY_SIZE = 512;

    class Stronghold : public Structure<STRONGHOLD_ARRAY_SIZE> {
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

        MU static std::string PIECE_TYPE_NAMES[13];
        RNG rng;

        // StructureComponent pieceArray[STRONGHOLD_ARRAY_SIZE]{};
        // int pieceArraySize = 0;
        // BoundingBox structureBB;
        // Pos2D startPos;

    public:

        int pendingPieceArray[STRONGHOLD_ARRAY_SIZE]{};
        PiecePlaceCount piecePlaceCounts[11]{};
        StructureComponent* altarChestsArray[4]{};


        StructureComponent* portalRoomPiece = nullptr;
        int eyesCount = 0; // number of eyes in portal room (to be populated by the rolls)


        // int startX = 0;
        // int startZ = 0;
        int pendingPiecesArraySize = 0;
        int altarChestArraySize = 0;
        int totalWeight = 145;
        int piecePlaceCountsSize = 11;

        GenerationStep generationStep = GS_Stronghold_Full;
        PieceType forcedPiece = PT_NONE;
        PieceType previousPiece = PT_NONE;
        bool generationStopped = false;

        Stronghold();
        ~Stronghold();
        void deletePieces();

        void reset();


        void generate(i64 worldSeed, int chunkX, int chunkZ);
        void generate(c_i64 worldSeed, Pos2D chunkPos);

        MU static std::string getPieceName(PieceType pieceType);




    private:

        void onWeightedPiecePlaced(int piecePlaceCountIndex);
        void createPiece(PieceType pieceType, EnumFacing facing, i8 depth, BoundingBox boundingBox);

        bool tryAddPieceFromType(PieceType pieceType, const Pos3D& pos, EnumFacing facing, i8 depth);
        void genAndAddPiece(const Pos3D& pos, EnumFacing facing, i8 depth);

        /// piece gen
        bool genPieceFromSmallDoor(const Pos3D& pos, EnumFacing facing, i8 depth);
        void genSmallDoorChildForward(const StructureComponent& piece, int n, int n2);
        void genSmallDoorChildLeft(const StructureComponent& piece, int n, int n2);
        void genSmallDoorChildRight(const StructureComponent& piece, int n, int n2);

        void addChildren(const StructureComponent& piece);

        static BoundingBox createPieceBoundingBox(PieceType pieceType, const Pos3D& pos, EnumFacing facing);
        static bool isOkBox(const BoundingBox& boundingBox);



        StructureComponent* findCollisionPiece(const BoundingBox& boundingBox);
        bool collidesWithPiece(const BoundingBox& boundingBox);
    };
} // namespace gen



namespace build::stronghold {


    enum class Door {
        OPENING = 1,
        WOOD_DOOR = 2,
        GRATES = 3,
        IRON_DOOR = 4
    };


    MU static Door getRandomDoor(c_int data) {
        switch (data) {
            case 0:
            case 1:
            default:
                return Door::OPENING;
            case 2:
                return Door::WOOD_DOOR;
            case 3:
                return Door::GRATES;
            case 4:
                return Door::IRON_DOOR;
        }
    }



    extern void placeDoor(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                          const StructureComponent& piece, Door door, c_int x, c_int y, c_int z);



    class MU ChestCorridor final {
    public:
        MU ChestCorridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class Crossing final {
    public:
        MU Crossing() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU LeftTurn final {
    public:
        MU LeftTurn() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Library final {
    public:
        MU Library() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU PortalRoom final {
    public:
        MU PortalRoom() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Prison final {
    public:
        MU Prison() = delete;


        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU RightTurn final {
    public:
        MU RightTurn() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU RoomCrossing final {
    public:
        MU RoomCrossing() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Stairs final {
    public:
        MU Stairs() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };


    class MU StairsStraight final {
    public:
        MU StairsStraight() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Straight final {
    public:
        MU Straight() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };



    MU extern bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);


} // namespace build::stronghold



namespace rolls {
    class Stronghold {
    public:
        /// generate certain stronghold rolls in the chunk
        template<bool stopStrongholdChest, bool stopPortal>
        static bool additionalStrongholdRolls(ChunkPrimer* chunk, const gen::Stronghold* sg, RNG& rng, c_int xChunk,
                                              c_int zChunk, const StructureComponent& pieceStop);

        template<bool isStrongholdChest>
        MU static bool generateStructure(ChunkPrimer* chunk, gen::Stronghold* sg, RNG& rng, c_int xChunk, c_int zChunk,
                                         const StructureComponent& pieceStop);

        static void setEye(const BoundingBox& chunkBB, const StructureComponent *piece, int x, int z, RNG& random,
                           std::vector<bool>& portalRoomEyes, int& success, int index);

        static std::vector<bool> getEyePlacements(World& worldIn, const gen::Stronghold* sg, const Generator& g);

        /// get the number of eyes in the portal room
        MU static int getEyesCount(World& worldIn, const gen::Stronghold* sg, const Generator& g);
    };

} // namespace rolls



