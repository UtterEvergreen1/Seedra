#pragma once

#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"

#include <map>
#include <vector>

#include "lce/blocks/block.hpp"
#include "lce/enums.hpp"

class World;
class Generator;
class ChunkPrimer;

namespace gen {

    static constexpr u32 MINESHAFT_ARRAY_SIZE = 512;

    /**
     * @class Mineshaft
     * @brief Represents a mineshaft structure in the world generation system.
     *
     * This class handles the generation and management of mineshaft structures, including
     * collision detection, piece generation, and component building.
     */
    class Mineshaft : public Structure<MINESHAFT_ARRAY_SIZE> {
    public:
        StructureVariant myVariant = StructureVariant::SV_Mineshaft_Normal; ///< The variant of the mineshaft.

        MU static std::string PIECE_TYPE_NAMES[5]; ///< Names of the different piece types in the mineshaft.

        Mineshaft() = default; ///< Default constructor for the Mineshaft class.

        ~Mineshaft(); ///< Destructor for the Mineshaft class.

        /**
         * @brief Generates the mineshaft structure.
         *
         * @param console The console for logging or debugging.
         * @param worldSeed The seed of the world.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         */
        void generate(lce::CONSOLE console, i64 worldSeed, int chunkX, int chunkZ);

        /**
         * @brief Generates the mineshaft structure.
         *
         * @param console The console for logging or debugging.
         * @param worldSeed The seed of the world.
         * @param chunkPos The position of the chunk as a 2D coordinate.
         */
        void generate(lce::CONSOLE console, i64 worldSeed, Pos2D chunkPos);

        /**
         * @brief Resets the mineshaft structure to its initial state.
         */
        MU void reset();

        /**
         * @brief Gets the name of a piece type.
         *
         * @param pieceType The type of the piece.
         * @return The name of the piece type as a string.
         */
        MU static std::string getPieceName(PieceType pieceType);

    private:
        /**
         * @brief Checks if a bounding box collides with existing components.
         *
         * @param bbIn The bounding box to check.
         * @return True if a collision is detected, false otherwise.
         */
        bool collides(const BoundingBox& bbIn);

        /**
         * @brief Finds a structure component that collides with the given bounding box.
         *
         * @param bbIn The bounding box to check.
         * @return A pointer to the colliding structure component, or nullptr if none is found.
         */
        StructureComponent* findCollisionPiece(const BoundingBox& bbIn);

        /**
         * @brief Generates and adds a new piece to the mineshaft.
         *
         * @param rng The random number generator.
         * @param pos The position of the piece.
         * @param facing The orientation of the piece.
         * @param depth The depth of the piece in the structure hierarchy.
         */
        void genAndAddPiece(RNG& rng, Pos3D pos, EnumFacing facing, int depth);

        /**
         * @brief Builds a structure component.
         *
         * @param rng The random number generator.
         * @param p The structure component to build.
         */
        void buildComponent(RNG& rng, const StructureComponent& p);
    };

} // namespace gen


namespace build::mineshaft {
    MU extern bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& chunkBB, StructureComponent& piece);
} // namespace build::mineshaft

namespace rolls {

    /**
     * @class Mineshaft
     * @brief Handles the generation of mineshaft-related structures and features.
     */
    class MU Mineshaft {
    public:
        /// List of mineshaft chests and their positions.
        std::vector<std::pair<Pos3D, i64>> mineshaftChests{};

        void generateStructure(World& worldIn, const ChunkPrimer* chunk, const gen::Mineshaft* mg,
                               RNG& rng, int chunkX, int chunkZ);

        MU void generateAllChests(World& worldIn, const gen::Mineshaft* mg, const Generator& g,
                                  bool generateFullChunk);

        void generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                           const StructureComponent *piece, RNG& rng, int x, int y, int z);

    private:
        static void placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                const StructureComponent& piece, RNG& rng, int x, int z);
    };

} // namespace rolls