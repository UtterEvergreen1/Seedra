#pragma once

#include <map>
#include <string>

#include "components/GenerationStep.hpp"
#include "lce/processor.hpp"
#include "common/rng.hpp"

#include "components/PieceWeight.hpp"
#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"

class World;
class Generator;

namespace gen {
    static constexpr u32 STRONGHOLD_ARRAY_SIZE = 512;

    /**
     * @class Stronghold
     * @brief Represents the Stronghold structure in the world generation system.
     *
     * This class defines the components, weights, and placement logic for generating
     * Stronghold structures in the game.
     */
    class Stronghold : public Structure<STRONGHOLD_ARRAY_SIZE> {
    public:
        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################

        /**
         * @struct PiecePlaceCount
         * @brief Tracks the placement count and constraints for a specific piece type.
         */
        struct PiecePlaceCount {
            PieceType pieceType; ///< The type of the piece.
            int placeCount; ///< The number of times this piece has been placed.

            /**
             * @brief Retrieves the weight configuration for a specific piece type.
             * @param pieceType The type of the piece.
             * @return A pointer to the corresponding PieceWeight.
             */
            ND static const PieceWeight *get(const PieceType pieceType) {
                return &PIECE_WEIGHTS[pieceType];
            }

            /**
             * @brief Checks if the piece placement is valid based on its constraints.
             * @return True if the piece can be placed, false otherwise.
             */
            ND bool isValid() const {
                c_int maxPlaceCount = PIECE_WEIGHTS[pieceType].maxPlaceCount;
                return maxPlaceCount == 0 || placeCount < maxPlaceCount;
            }

            /**
             * @brief Determines if the piece can be placed at a specific depth.
             * @param depth The current depth in the structure hierarchy.
             * @return True if the piece can be placed, false otherwise.
             */
            ND bool canPlace(c_int depth) const {
                return isValid() && depth >= PIECE_WEIGHTS[pieceType].minDepth;
            }
        };

        /**
         * @var PIECE_WEIGHTS
         * @brief Defines the weights, maximum placement counts, and minimum depths for each piece type.
         */
        static const PieceWeight PIECE_WEIGHTS[12];

        /**
         * @var PIECE_PLACE_COUNT_DEFAULT
         * @brief Default placement counts for all piece types in the Stronghold.
         */
        static const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[11];

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################
    private:
        RNG rng; ///< Random number generator for structure generation.

        // StructureComponent pieceArray[STRONGHOLD_ARRAY_SIZE]{};
        // int pieceArraySize = 0;
        // BoundingBox structureBB;
        // Pos2D startPos;

    public:
        /**
         * @var PIECE_TYPE_NAMES
         * @brief Names of the different piece types in the Stronghold.
         */
        MU static std::string PIECE_TYPE_NAMES[13];

        int pendingPieceArray[STRONGHOLD_ARRAY_SIZE]{}; ///< Array of pending pieces to be placed.
        PiecePlaceCount piecePlaceCounts[11]{}; ///< Array of piece placement counts.
        StructureComponent *altarChestsArray[4]{}; ///< Array of altar chest components.

        StructureComponent *portalRoomPiece = nullptr; ///< Pointer to the portal room component.
        int eyesCount = 0; ///< Number of eyes in the portal room (to be populated by the rolls).

        // int startX = 0;
        // int startZ = 0;
        int pendingPiecesArraySize = 0; ///< Size of the pending pieces array.
        int altarChestArraySize = 0; ///< Size of the altar chest array.
        int totalWeight = 145; ///< Total weight of all pieces.
        int piecePlaceCountsSize = 11; ///< Size of the piece placement counts array.

        GenerationStep generationStep = GS_Stronghold_Full; ///< Current generation step.
        PieceType forcedPiece = PT_NONE; ///< Piece type to be forced during generation.
        PieceType previousPiece = PT_NONE; ///< Previously placed piece type.
        bool generationStopped = false; ///< Flag indicating if generation has stopped.

        /**
         * @brief Default constructor for the Stronghold class.
         */
        Stronghold();

        /**
         * @brief Destructor for the Stronghold class.
         */
        ~Stronghold();

        /**
         * @brief Deletes all pieces in the Stronghold.
         */
        void deletePieces();

        /**
         * @brief Resets the Stronghold to its initial state.
         */
        void reset();

        /**
         * @brief Generates the Stronghold structure.
         * @param worldSeed The seed of the world.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         */
        void generate(i64 worldSeed, int chunkX, int chunkZ);

        /**
         * @brief Generates the Stronghold structure.
         * @param worldSeed The seed of the world.
         * @param chunkPos The position of the chunk as a 2D coordinate.
         */
        void generate(c_i64 worldSeed, Pos2D chunkPos);

        /**
         * @brief Gets the name of a piece type.
         * @param pieceType The type of the piece.
         * @return The name of the piece type as a string.
         */
        MU static std::string getPieceName(PieceType pieceType);

    private:
        /**
         * @brief Updates the state when a weighted piece is placed.
         * @param piecePlaceCountIndex The index of the placed piece in the placement counts array.
         */
        void onWeightedPiecePlaced(int piecePlaceCountIndex);

        /**
         * @brief Creates a new piece of the specified type.
         * @param pieceType The type of the piece.
         * @param facing The orientation of the piece.
         * @param depth The depth of the piece in the structure hierarchy.
         * @param boundingBox The bounding box of the piece.
         */
        void createPiece(PieceType pieceType, EnumFacing facing, i8 depth, BoundingBox boundingBox);

        /**
         * @brief Attempts to add a piece of the specified type.
         * @param pieceType The type of the piece.
         * @param pos The position of the piece.
         * @param facing The orientation of the piece.
         * @param depth The depth of the piece in the structure hierarchy.
         * @return True if the piece was successfully added, false otherwise.
         */
        bool tryAddPieceFromType(PieceType pieceType, const Pos3D &pos, EnumFacing facing, i8 depth);

        /**
         * @brief Generates and adds a new piece to the Stronghold.
         * @param pos The position of the piece.
         * @param facing The orientation of the piece.
         * @param depth The depth of the piece in the structure hierarchy.
         */
        void genAndAddPiece(const Pos3D &pos, EnumFacing facing, i8 depth);

        /**
         * @brief Generates a piece from a small door.
         * @param pos The position of the piece.
         * @param facing The orientation of the piece.
         * @param depth The depth of the piece in the structure hierarchy.
         * @return True if the piece was successfully generated, false otherwise.
         */
        bool genPieceFromSmallDoor(const Pos3D &pos, EnumFacing facing, i8 depth);

        /**
         * @brief Generates a child piece forward from a small door.
         * @param piece The parent piece.
         * @param n The first parameter for generation.
         * @param n2 The second parameter for generation.
         */
        void genSmallDoorChildForward(const StructureComponent &piece, int n, int n2);

        /**
         * @brief Generates a child piece to the left from a small door.
         * @param piece The parent piece.
         * @param n The first parameter for generation.
         * @param n2 The second parameter for generation.
         */
        void genSmallDoorChildLeft(const StructureComponent &piece, int n, int n2);

        /**
         * @brief Generates a child piece to the right from a small door.
         * @param piece The parent piece.
         * @param n The first parameter for generation.
         * @param n2 The second parameter for generation.
         */
        void genSmallDoorChildRight(const StructureComponent &piece, int n, int n2);

        /**
         * @brief Adds child pieces to the specified parent piece.
         * @param piece The parent piece.
         */
        void addChildren(const StructureComponent &piece);

        /**
         * @brief Creates a bounding box for a piece.
         * @param pieceType The type of the piece.
         * @param pos The position of the piece.
         * @param facing The orientation of the piece.
         * @return The bounding box of the piece.
         */
        static BoundingBox createPieceBoundingBox(PieceType pieceType, const Pos3D &pos, EnumFacing facing);

        /**
         * @brief Checks if a bounding box is valid.
         * @param boundingBox The bounding box to check.
         * @return True if the bounding box is valid, false otherwise.
         */
        static bool isOkBox(const BoundingBox &boundingBox);

        /**
         * @brief Finds a structure component that collides with the given bounding box.
         * @param boundingBox The bounding box to check.
         * @return A pointer to the colliding structure component, or nullptr if none is found.
         */
        StructureComponent *findCollisionPiece(const BoundingBox &boundingBox);

        /**
         * @brief Checks if a bounding box collides with existing pieces.
         * @param boundingBox The bounding box to check.
         * @return True if a collision is detected, false otherwise.
         */
        bool collidesWithPiece(const BoundingBox &boundingBox);
    };
} // namespace gen

namespace build {
    namespace stronghold {
        /**
         * @enum Door
         * @brief Represents the different types of doors in the Stronghold.
         */
        enum class Door {
            OPENING = 1, ///< An open doorway.
            WOOD_DOOR = 2, ///< A wooden door.
            GRATES = 3, ///< Grates as a door.
            IRON_DOOR = 4 ///< An iron door.
        };

        /**
         * @brief Gets a random door type based on the given data.
         * @param data The input data.
         * @return The randomly selected door type.
         */
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

        /**
         * @brief Places a door in the world.
         * @param worldIn The world to modify.
         * @param rng The random number generator.
         * @param chunkBB The bounding box of the chunk.
         * @param piece The structure component.
         * @param door The type of door to place.
         * @param x The X coordinate.
         * @param y The Y coordinate.
         * @param z The Z coordinate.
         */
        extern void placeDoor(World &worldIn, MU RNG &rng, const BoundingBox &chunkBB,
                              const StructureComponent &piece, Door door, c_int x, c_int y, c_int z);

        /*
         * @class ChestCorridor
         * @brief Represents a chest corridor in the Stronghold.
         */
        class MU ChestCorridor final {
        public:
            MU ChestCorridor() = delete;

            /**
             * @brief Adds the chest corridor component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class Corridor
         * @brief Represents a corridor in the Stronghold.
         */
        class Corridor final {
        public:
            MU Corridor() = delete;

            /**
             * @brief Adds the corridor component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, MU RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class Crossing
         * @brief Represents a crossing in the Stronghold.
         */
        class Crossing final {
        public:
            MU Crossing() = delete;

            /**
             * @brief Adds the crossing component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class LeftTurn
         * @brief Represents a left turn in the Stronghold.
         */
        class MU LeftTurn final {
        public:
            MU LeftTurn() = delete;

            /**
             * @brief Adds the left turn component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class Library
         * @brief Represents a library in the Stronghold.
         */
        class MU Library final {
        public:
            MU Library() = delete;

            /**
             * @brief Adds the library component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class PortalRoom
         * @brief Represents the portal room in the Stronghold.
         */
        class MU PortalRoom final {
        public:
            MU PortalRoom() = delete;

            /**
             * @brief Adds the portal room component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class Prison
         * @brief Represents a prison in the Stronghold.
         */
        class MU Prison final {
        public:
            MU Prison() = delete;

            /**
             * @brief Adds the prison component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class RightTurn
         * @brief Represents a right turn in the Stronghold.
         */
        class MU RightTurn final {
        public:
            MU RightTurn() = delete;

            /**
             * @brief Adds the right turn component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class RoomCrossing
         * @brief Represents a room crossing in the Stronghold.
         */
        class MU RoomCrossing final {
        public:
            MU RoomCrossing() = delete;

            /**
             * @brief Adds the room crossing component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class StairsDown
         * @brief Represents stairs going down in the Stronghold.
         */
        class MU Stairs final {
        public:
            MU Stairs() = delete;

            /**
             * @brief Adds the stairs component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class StairsStraight
         * @brief Represents straight stairs in the Stronghold.
         */
        class MU StairsStraight final {
        public:
            MU StairsStraight() = delete;

            /**
             * @brief Adds the straight stairs component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /*
         * @class Straight
         * @brief Represents a straight corridor in the Stronghold.
         */
        class MU Straight final {
        public:
            MU Straight() = delete;

            /**
             * @brief Adds the straight corridor component parts to the world.
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
        };

        /**
         * @brief Adds the component parts to the world.
         * @param worldIn The world to modify.
         * @param rng The random number generator.
         * @param chunkBB The bounding box of the chunk.
         * @param piece The structure component.
         * @return True if the parts were added successfully, false otherwise.
         */
        MU extern bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &chunkBB, const StructureComponent &piece);
    }
} // namespace build::stronghold

namespace rolls {
    /**
     * @class Stronghold
     * @brief Handles the generation of Stronghold-related structures and features.
     */
    class Stronghold {
    public:
        /**
         * @brief Generates additional rolls for the Stronghold.
         * @tparam stopStrongholdChest Whether to stop generating Stronghold chests.
         * @tparam stopPortal Whether to stop generating the portal.
         * @param chunk The chunk primer.
         * @param sg The Stronghold generator.
         * @param rng The random number generator.
         * @param xChunk The X coordinate of the chunk.
         * @param zChunk The Z coordinate of the chunk.
         * @param pieceStop The structure component to stop at.
         * @return True if the rolls were successful, false otherwise.
         */
        template<bool stopStrongholdChest, bool stopPortal>
        static bool additionalStrongholdRolls(ChunkPrimer *chunk, const gen::Stronghold *sg, RNG &rng, c_int xChunk,
                                              c_int zChunk, const StructureComponent &pieceStop);

        /**
         * @brief Generates the Stronghold structure.
         * @tparam isStrongholdChest Whether the structure contains a Stronghold chest.
         * @param chunk The chunk primer.
         * @param sg The Stronghold generator.
         * @param rng The random number generator.
         * @param xChunk The X coordinate of the chunk.
         * @param zChunk The Z coordinate of the chunk.
         * @param pieceStop The structure component to stop at.
         * @return True if the structure was generated successfully, false otherwise.
         */
        template<bool isStrongholdChest>
        MU static bool generateStructure(ChunkPrimer *chunk, gen::Stronghold *sg, RNG &rng, c_int xChunk, c_int zChunk,
                                         const StructureComponent &pieceStop);

        /**
         * @brief Sets an eye in the portal room.
         * @param chunkBB The bounding box of the chunk.
         * @param piece The structure component.
         * @param x The X coordinate.
         * @param z The Z coordinate.
         * @param random The random number generator.
         * @param portalRoomEyes The vector of portal room eyes.
         * @param success The success count.
         * @param index The index of the eye.
         */
        static void setEye(const BoundingBox &chunkBB, const StructureComponent *piece, int x, int z, RNG &random,
                           std::vector<bool> &portalRoomEyes, int &success, int index);

        /**
         * @brief Gets the placements of eyes in the portal room.
         * @param sg The Stronghold generator.
         * @param g The generator.
         * @return A vector of boolean values representing eye placements.
         */
        static std::vector<bool> getEyePlacements(const gen::Stronghold *sg, const Generator &g);

        /**
         * @brief Gets the number of eyes in the portal room.
         * @param sg The Stronghold generator.
         * @param g The generator.
         * @return The number of eyes in the portal room.
         */
        MU static int getEyesCount(const gen::Stronghold *sg, const Generator &g);
    };
} // namespace rolls
