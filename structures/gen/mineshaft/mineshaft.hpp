#pragma once

#include "components/GenerationStep.hpp"
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

    static constexpr u32 MINESHAFT_ARRAY_SIZE = 1024;

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
        void buildComponent(RNG& rng, StructureComponent p);
    };

} // namespace gen

namespace build {
    namespace mineshaft {

        /**
         * @brief Gets biome-specific plank block state for the mineshaft.
         *
         * @param structureType The type of the structure.
         * @return The block state of the biome-specific plank.
         */
        MU ND extern lce::BlockState getBiomeSpecificPlank(c_int structureType);

        /**
         * @brief Gets biome-specific fence block state for the mineshaft.
         *
         * @param structureType The type of the structure.
         * @return The block state of the biome-specific fence.
         */
        MU ND extern lce::BlockState getBiomeSpecificFence(c_int structureType);

        /**
         * @brief Performs a specific operation on the world and bounding box.
         *
         * @param world The world to modify.
         * @param chunkBB The bounding box of the chunk.
         * @param minX The minimum X coordinate.
         * @param maxX The maximum X coordinate.
         * @param y The Y coordinate.
         * @param z The Z coordinate.
         * @param piece The structure component.
         * @return True if the operation was successful, false otherwise.
         */
        extern bool func_189918_a(World& world, const BoundingBox& chunkBB, c_int minX, c_int maxX,
                                  c_int y, c_int z, const StructureComponent& piece);

        /**
         * @class Room
         * @brief Represents a room component in the mineshaft.
         */
        class MU Room final {
        public:
            MU Room() = delete; ///< Deleted default constructor.

            /**
             * @brief Adds the room component parts to the world.
             *
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                    World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
        };

        /**
         * @class Corridor
         * @brief Represents a corridor component in the mineshaft.
         */
        class MU Corridor final {
        public:
            MU Corridor() = delete; ///< Deleted default constructor.

            /**
             * @brief Adds the corridor component parts to the world.
             *
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                    World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);

            /**
             * @brief Performs a specific operation on the corridor.
             *
             * @param world The world to modify.
             * @param chunkBB The bounding box of the chunk.
             * @param minX The minimum X coordinate.
             * @param minY The minimum Y coordinate.
             * @param minZ The minimum Z coordinate.
             * @param maxY The maximum Y coordinate.
             * @param maxX The maximum X coordinate.
             * @param rng The random number generator.
             * @param piece The structure component.
             */
            static void func_189921_a(World& world, const BoundingBox& chunkBB,
                                      c_int minX, c_int minY, c_int minZ, c_int maxY, c_int maxX,
                                      RNG& rng, const StructureComponent& piece);

            /**
             * @brief Places cobwebs randomly in the corridor.
             *
             * @param world The world to modify.
             * @param chunkBB The bounding box of the chunk.
             * @param rng The random number generator.
             * @param chance The chance of placing a cobweb.
             * @param x The X coordinate.
             * @param y The Y coordinate.
             * @param z The Z coordinate.
             * @param piece The structure component.
             */
            static void placeCobwebsRandomly(World& world, const BoundingBox& chunkBB, RNG& rng,
                                             c_float chance, c_int x, c_int y, c_int z, const StructureComponent& piece);
        };

        /**
         * @class Crossing
         * @brief Represents a crossing component in the mineshaft.
         */
        class MU Crossing final {
        public:
            MU Crossing() = delete; ///< Deleted default constructor.

            /**
             * @brief Adds the crossing component parts to the world.
             *
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                    World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);

            /**
             * @brief Places a plank pillar in the crossing.
             *
             * @param world The world to modify.
             * @param chunkBB The bounding box of the chunk.
             * @param x The X coordinate.
             * @param minY The minimum Y coordinate.
             * @param z The Z coordinate.
             * @param maxY The maximum Y coordinate.
             * @param piece The structure component.
             */
            static void placePlankPillar(World& world, const BoundingBox& chunkBB,
                                         c_int x, c_int minY, c_int z, c_int maxY, const StructureComponent& piece);
        };

        /**
         * @class Stairs
         * @brief Represents a stairs component in the mineshaft.
         */
        class MU Stairs final {
        public:
            MU Stairs() = delete; ///< Deleted default constructor.

            /**
             * @brief Adds the stairs component parts to the world.
             *
             * @param worldIn The world to modify.
             * @param rng The random number generator.
             * @param chunkBB The bounding box of the chunk.
             * @param piece The structure component.
             * @return True if the parts were added successfully, false otherwise.
             */
            static bool addComponentParts(
                    World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
        };

        /**
         * @brief Adds component parts to the world.
         *
         * @param worldIn The world to modify.
         * @param rng The random number generator.
         * @param chunkBB The bounding box of the chunk.
         * @param piece The structure component.
         * @return True if the parts were added successfully, false otherwise.
         */
        MU extern bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    }
} // namespace build::mineshaft

namespace rolls {

    /**
     * @class Mineshaft
     * @brief Handles the generation of mineshaft-related structures and features.
     */
    class MU Mineshaft {
    public:
        std::vector<std::pair<Pos3D, i64>> mineshaftChests{}; ///< List of mineshaft chests and their positions.

        /**
         * @brief Generates the mineshaft structure in the world.
         *
         * @param worldIn The world to modify.
         * @param chunk The chunk primer.
         * @param mg The mineshaft generator.
         * @param rng The random number generator.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         */
        void generateStructure(World& worldIn, const ChunkPrimer* chunk, const gen::Mineshaft* mg,
                               RNG& rng, int chunkX, int chunkZ);

        /**
         * @brief Generates all chests for the mineshaft.
         *
         * @param worldIn The world to modify.
         * @param mg The mineshaft generator.
         * @param g The generator.
         * @param generateFullChunk Whether to generate the full chunk.
         */
        MU void generateAllChests(World& worldIn, const gen::Mineshaft* mg, const Generator& g, bool generateFullChunk);

        /**
         * @brief Generates a chest at a specific position.
         *
         * @param chunk The chunk primer.
         * @param chunkBB The bounding box of the chunk.
         * @param piece The structure component.
         * @param rng The random number generator.
         * @param x The X coordinate.
         * @param y The Y coordinate.
         * @param z The Z coordinate.
         */
        void generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                           const StructureComponent *piece, RNG& rng, int x, int y, int z);

    private:
        /**
         * @brief Places a cobweb at a specific position.
         *
         * @param chunk The chunk primer.
         * @param chunkBB The bounding box of the chunk.
         * @param piece The structure component.
         * @param rng The random number generator.
         * @param x The X coordinate.
         * @param z The Z coordinate.
         */
        static void placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                const StructureComponent& piece, RNG& rng, int x, int z);
    };

} // namespace rolls