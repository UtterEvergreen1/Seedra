#pragma once

#include "structures/gen/Structure.hpp"

#include "components/GenerationStep.hpp"

#include "common/rng.hpp"
#include "structures/gen/FeaturePiece.hpp"

#include <map>
#include <vector>

class StructureComponent;
class Generator;
class World;

namespace gen {
    static constexpr u32 VILLAGE_ARRAY_SIZE = 256;

    /**
     * @class Village
     * @brief Represents a village structure in the world generation system.
     *
     * This class handles the generation and management of village structures, including
     * piece weights, road generation, and biome-specific configurations.
     */
    class Village : public Structure<VILLAGE_ARRAY_SIZE> {
        /**
         * @struct PieceWeight
         * @brief Defines the weight and placement constraints for a specific piece type.
         */
        struct PieceWeight {
            MU PieceType pieceType; ///< The type of the piece.
            MU int weight; ///< The weight of the piece for random selection.
            MU int PlaceCountMin; ///< Minimum number of times this piece can be placed.
            MU int PlaceCountMax; ///< Maximum number of times this piece can be placed.
        };

        static c_int VILLAGE_SIZE; ///< The size of the village.
        static const PieceWeight PIECE_WEIGHTS[9]; ///< Array of piece weights for the village.

    private:
        /**
         * @struct FinalPieceWeight
         * @brief Tracks the placement state of a specific piece type during generation.
         */
        struct FinalPieceWeight {
            PieceType pieceType; ///< The type of the piece.
            int weight; ///< The weight of the piece for random selection.
            int maxPlaceCount; ///< Maximum number of times this piece can be placed.
            int amountPlaced; ///< Number of times this piece has been placed.

            /**
             * @brief Constructs a FinalPieceWeight object.
             * @param pieceType The type of the piece.
             * @param weight The weight of the piece.
             * @param maxPlaceCount The maximum placement count.
             * @param amountPlaced The initial placement count.
             */
            FinalPieceWeight(const PieceType pieceType, c_int weight, c_int maxPlaceCount, c_int amountPlaced)
                : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {
            }
        };

        std::vector<FinalPieceWeight> currentVillagePW; ///< Current piece weights for the village.

        int pendingRoadArray[VILLAGE_ARRAY_SIZE]{}; ///< Array of pending road pieces.
        int pendingRoadArraySize{}; ///< Size of the pending road array.

        const Generator *g; ///< Pointer to the generator.
        RNG rng; ///< Random number generator for structure generation.

        PieceType previousPiece{}; ///< The previously placed piece type.

        int numInvalidPieces = 1; ///< Number of invalid pieces (e.g., roads and wells).
        int myBlackSmithPieceIndex = -1; ///< Index of the blacksmith piece, if present.

        bool useBiomes; ///< Flag indicating whether to check biomes for village generation.
        bool isZombieInfested{}; ///< Flag indicating if the village is a zombie village.

    public:
        MU static std::string PIECE_TYPE_NAMES[13]; ///< Names of the different piece types.

        StructureVariant myVariant{}; ///< The variant of the village.

        GenerationStep generationStep = GS_Village_Full; ///< Current generation step.

        /**
         * @brief Deleted default constructor.
         */
        Village() = delete;

        /**
         * @brief Constructs a Village object with the specified generator.
         * @param generator Pointer to the generator.
         * @param useBiomes Flag indicating whether to check biomes for village generation.
         */
        Village(const Generator *generator, bool useBiomes = true);

        /**
         * @brief Destructor for the Village class.
         */
        ~Village();

        /**
         * @brief Resets the village to its initial state.
         */
        void reset();

        /**
         * @brief Generates the village structure at the specified chunk coordinates.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         */
        void generate(int chunkX, int chunkZ);

        /**
         * @brief Generates the village structure at the specified chunk position.
         * @param chunk The position of the chunk as a 2D coordinate.
         */
        void generate(Pos2D chunk);

        /**
         * @brief Retrieves the blacksmith piece of the village, if present.
         * @return A pointer to the blacksmith piece, or nullptr if not present.
         */
        MU ND const StructureComponent *getBlackSmithPiece();

        /**
         * @brief Checks if the village has more than two valid components.
         * @return True if the village has more than two components, false otherwise.
         */
        MU ND bool hasMoreThanTwoComponents() const { return pieceArraySize - numInvalidPieces > 2; }

        /**
         * @brief Checks if the village is a zombie village.
         * @return True if the village is a zombie village, false otherwise.
         */
        MU ND bool isZombieVillage() const { return isZombieInfested; }

        /**
         * @brief Gets the name of a piece type.
         * @param pieceType The type of the piece.
         * @return The name of the piece type as a string.
         */
        MU static std::string getPieceName(PieceType pieceType);

    private:
        /**
         * @brief Sets the biome type for the village.
         */
        MU void setVillageBiomeType();

        /**
         * @brief Sets up the pieces for the village.
         */
        void setupPieces();

        /**
         * @brief Updates the piece weight for the village.
         * @return The updated piece weight.
         */
        ND int updatePieceWeight() const;

        /**
         * @brief Creates a bounding box for a piece.
         * @param pieceType The type of the piece.
         * @param pos The position of the piece.
         * @param direction The orientation of the piece.
         * @return The bounding box of the piece.
         */
        static BoundingBox createPieceBB(PieceType pieceType, Pos3D pos, EnumFacing direction);

        /**
         * @brief Builds the starting component of the village.
         * @param piece The starting piece.
         */
        void buildComponentStart(const StructureComponent &piece);

        /**
         * @brief Builds a component of the village.
         * @param scIn The structure component to build.
         */
        void buildComponent(const StructureComponent &scIn);

        /**
         * @brief Creates a bounding box for a road.
         * @param pos The position of the road.
         * @param facing The orientation of the road.
         * @return The bounding box of the road.
         */
        BoundingBox road(Pos3D pos, EnumFacing facing);

        /**
         * @brief Performs additional random number generator rolls for a piece.
         * @param p The structure component.
         */
        void additionalRngRolls(StructureComponent &p);

        /**
         * @brief Generates a component at the specified position and orientation.
         * @param pos The position of the component.
         * @param facing The orientation of the component.
         * @param depth The depth of the component in the structure hierarchy.
         * @return The generated structure component.
         */
        StructureComponent generateComponent(Pos3D pos, EnumFacing facing, i8 depth);

        /**
         * @brief Generates and adds a road piece to the village.
         * @param pos The position of the road piece.
         * @param facing The orientation of the road piece.
         */
        void genAndAddRoadPiece(Pos3D pos, EnumFacing facing);

        /**
         * @brief Generates and adds a component to the village.
         * @param pos The position of the component.
         * @param facing The orientation of the component.
         * @param depth The depth of the component in the structure hierarchy.
         * @return The generated structure component.
         */
        StructureComponent genAndAddComponent(Pos3D pos, EnumFacing facing, i8 depth);

        /**
         * @brief Adds a piece to the village.
         * @param piece The structure component to add.
         */
        void addPiece(StructureComponent &piece);

        /**
         * @brief Checks if a bounding box collides with existing pieces.
         * @param boundingBox The bounding box to check.
         * @return True if a collision is detected, false otherwise.
         */
        ND bool hasCollisionPiece(const BoundingBox &boundingBox) const;
    };
} // namespace gen


namespace build::village {
    MU extern bool addComponentParts(
        World &worldIn, RNG &rng, BoundingBox &chunkBB, StructureComponent &piece);
} // namespace build::village
