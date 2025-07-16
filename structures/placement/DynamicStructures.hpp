#pragma once

#include "common/Pos2DTemplate.hpp"
#include "terrain/biomes/biomeID.hpp"
#include "terrain/biomes/layers.hpp"
#include "terrain/generator.hpp"
#include <atomic>
#include <vector>

namespace Placement {
    /**
     * @brief Default valid biomes for secondary structures.
     *
     * This constant defines a bitmask of biomes that are considered valid for secondary structures.
     */
    static constexpr u64 DEFAULT_SECONDARY_VALID_BIOMES =
            1ULL << ocean | 1ULL << plains | 1ULL << desert | 1ULL << forest | 1ULL << taiga | 1ULL << swampland |
            1ULL << river | 1ULL << hell | 1ULL << the_end | 1ULL << legacy_frozen_ocean |
            1ULL << frozen_river | 1ULL << ice_plains | 1ULL << ice_mountains | 1ULL << mushroom_island |
            1ULL << mushroom_island_shore | 1ULL << beach | 1ULL << desert_hills | 1ULL << forest_hills |
            1ULL << taiga_hills | 1ULL << extreme_hills_edge | 1ULL << jungle | 1ULL << jungle_hills |
            1ULL << jungle_edge | 1ULL << deep_ocean | 1ULL << cold_beach | 1ULL << birch_forest | 1ULL << birch_forest_hills |
            1ULL << roofed_forest | 1ULL << cold_taiga | 1ULL << cold_taiga_hills | 1ULL << mega_taiga |
            1ULL << mega_taiga_hills | 1ULL << savanna | 1ULL << savanna_plateau | 1ULL << mesa |
            1ULL << mesa_plateau_stone | 1ULL << mesa_plateau | 1ULL << warm_ocean | 1ULL << deep_warm_ocean |
            1ULL << lukewarm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << cold_ocean | 1ULL << deep_cold_ocean |
            1ULL << frozen_ocean | 1ULL << deep_frozen_ocean;

    /**
     * @brief Default valid mutated biomes for secondary structures.
     *
     * This constant defines a bitmask of mutated biomes that are considered valid for secondary structures.
     */
    static constexpr u64 DEFAULT_SECONDARY_VALID_BIOMES_MUTATED =
            1ULL << (sunflower_plains - 128) | 1ULL << (desert_mutated - 128) |
            1ULL << (swampland_mutated - 128) | 1ULL << (mega_spruce_taiga - 128) |
            1ULL << (redwood_taiga_hills_mutated - 128) | 1ULL << (mesa_bryce - 128) |
            1ULL << (mesa_plateau_stone_mutated - 128) | 1ULL << (mesa_plateau_mutated - 128);

    /**
     * @class DynamicStructure
     * @brief Template class for dynamic structure placement.
     *
     * This class provides methods for determining valid positions for structures
     * and managing biome constraints.
     *
     * @tparam Derived The derived structure type.
     */
    template<typename Derived>
    class DynamicStructure {
    public:
        constexpr static u64 MAIN_VALID_BIOMES = 0; ///< Main valid biomes for the structure.
        constexpr static u64 SECONDARY_VALID_BIOMES = 0; ///< Secondary valid biomes for the structure.
        constexpr static u64 SECONDARY_VALID_BIOMES_MUTATED = 0; ///< Mutated secondary valid biomes.

        static int CHUNK_BOUNDS; ///< Bounds for chunk placement.
        static int REGION_SIZE; ///< Size of the region for structure placement.
        static int CHUNK_RANGE; ///< Range of chunks to consider for placement.
        static int ATTEMPTS; ///< Maximum number of placement attempts.
        static bool REDUCED_SPACING; ///< Flag for reduced spacing between structures.
        constexpr static int SALT = 0; ///< Salt value for structure placement.
        constexpr static int MAIN_RADIUS = 0; ///< Main radius for structure placement.
        constexpr static int SECOND_RADIUS = 0; ///< Secondary radius for structure placement.
        constexpr static bool HAS_SECOND_BIOME_CHECK = true; ///< Flag for secondary biome checks.

        /**
         * @brief Gets the position of the structure in a region.
         * @param g Pointer to the generator.
         * @param regionX The X coordinate of the region.
         * @param regionZ The Z coordinate of the region.
         * @return The position of the structure.
         */
        static Pos2D getPosition(const Generator *g, int regionX, int regionZ);

        static Pos2DVec_t getAllPossiblePositions(int64_t worldSeed, int regionX, int regionZ);

        /**
         * @brief Verifies if the structure can spawn at a specific block position.
         * @param g Pointer to the generator.
         * @param blockX The X coordinate of the block.
         * @param blockZ The Z coordinate of the block.
         * @return True if the structure can spawn, false otherwise.
         */
        static bool verifyBlockPosition(const Generator *g, int blockX, int blockZ);

        /**
         * @brief Verifies if the structure can spawn at a specific chunk position.
         * @param g Pointer to the generator.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         * @return True if the structure can spawn, false otherwise.
         */
        static bool verifyChunkPosition(const Generator *g, int chunkX, int chunkZ) {
            return verifyBlockPosition(g, (chunkX << 4) + 8, (chunkZ << 4) + 8);
        }

        /**
         * @brief Checks if a position is a valid chunk position within the 60 rolls for the structure.
         * @param worldSeed The world seed.
         * @param regionX The X coordinate of the region.
         * @param regionZ The Z coordinate of the region.
         * @param pos The position to check.
         * @return True if the position is valid, false otherwise.
         */
        static bool isPossibleChunkPos(int64_t worldSeed, int regionX, int regionZ, const Pos2D &pos);

        /**
         * @brief Gets all positions of the structure.
         * @param g Pointer to the generator.
         * @param terminateFlag Optional flag to terminate the operation.
         * @return A vector of all structure positions.
         */
        static std::vector<Pos2D> getAllPositions(const Generator *g, std::atomic_bool *terminateFlag = nullptr);

        /**
         * @brief Gets all positions of the structure within bounds.
         * @param g Pointer to the generator.
         * @param lowerX Lower X bound.
         * @param lowerZ Lower Z bound.
         * @param upperX Upper X bound.
         * @param upperZ Upper Z bound.
         * @param terminateFlag Optional flag to terminate the operation.
         * @return A vector of all structure positions within bounds.
         */
        static std::vector<Pos2D>
        getAllPositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX, int upperZ,
                               std::atomic_bool *terminateFlag = nullptr);

        /**
         * @brief Checks if the structure can spawn at a specific chunk.
         * @param worldSeed The world seed.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         * @param regionX The X coordinate of the region.
         * @param regionZ The Z coordinate of the region.
         * @return True if the structure can spawn, false otherwise.
         */
        MU static bool canSpawnAtChunk(i64 worldSeed, int chunkX, int chunkZ, int regionX, int regionZ);
    };

    /**
     * @class Mansion
     * @brief Represents a Mansion structure.
     */
    class Mansion : public DynamicStructure<Mansion> {
    public:
        constexpr static int Placement::DynamicStructure<Mansion>::SALT = 10387319;
        constexpr static int Placement::DynamicStructure<Mansion>::MAIN_RADIUS = 4;
        constexpr static int Placement::DynamicStructure<Mansion>::SECOND_RADIUS = 32;
        constexpr static u64 Placement::DynamicStructure<Mansion>::MAIN_VALID_BIOMES = (1ULL << dark_forest);
        constexpr static u64 Placement::DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
        constexpr static u64 Placement::DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;
        /**
         * @brief Sets the world size for the Mansion.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    /**
     * @class Monument
     * @brief Represents a Monument structure.
     */
    class Monument : public DynamicStructure<Monument> {
    public:
        constexpr static int Placement::DynamicStructure<Monument>::SALT = 10387313;
        constexpr static int Placement::DynamicStructure<Monument>::MAIN_RADIUS = 8;
        constexpr static int Placement::DynamicStructure<Monument>::SECOND_RADIUS = 29;
        constexpr static u64 Placement::DynamicStructure<Monument>::MAIN_VALID_BIOMES =
            1ULL << ocean | 1ULL << river | 1ULL << frozen_river | 1ULL << deep_ocean | 1ULL << warm_ocean |
            1ULL << deep_warm_ocean | 1ULL << lukewarm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << cold_ocean |
            1ULL << deep_cold_ocean | 1ULL << frozen_ocean | 1ULL << deep_frozen_ocean;
        constexpr static u64 Placement::DynamicStructure<Monument>::SECONDARY_VALID_BIOMES =
            1ULL << deep_ocean | 1ULL << deep_warm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << deep_cold_ocean |
            1ULL << deep_frozen_ocean;
        /**
         * @brief Sets the world size for the Monument.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    /**
     * @class BuriedTreasure
     * @brief Represents a Buried Treasure structure.
     */
    class BuriedTreasure : public DynamicStructure<BuriedTreasure> {
    public:
        constexpr static int Placement::DynamicStructure<BuriedTreasure>::SALT = 16842397;
        constexpr static int Placement::DynamicStructure<BuriedTreasure>::MAIN_RADIUS = 0;
        constexpr static int Placement::DynamicStructure<BuriedTreasure>::SECOND_RADIUS = 16;
        constexpr static u64 Placement::DynamicStructure<BuriedTreasure>::MAIN_VALID_BIOMES =
            1ULL << mushroom_island_shore | 1ULL << beach | 1ULL << stone_beach | 1ULL << cold_beach;
        constexpr static u64 Placement::DynamicStructure<BuriedTreasure>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
        constexpr static u64 Placement::DynamicStructure<BuriedTreasure>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;
        /**
         * @brief Sets the world size for the Buried Treasure.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);

        /**
         * @brief Gets all positions of the Buried Treasure.
         * @param g Pointer to the generator.
         * @param terminateFlag Optional flag to terminate the operation.
         * @return A vector of all Buried Treasure positions.
         */
        MU static std::vector<Pos2D> getAllPositions(const Generator *g, std::atomic_bool *terminateFlag = nullptr) {
            std::vector<Pos2D> positions = DynamicStructure::getAllPositions(g, terminateFlag);
            for (Pos2D &pos: positions) {
                pos.z += 1;
            }
            return positions;
        }

        /**
         * @brief Gets all positions of the Buried Treasure within bounds.
         * @param g Pointer to the generator.
         * @param lowerX Lower X bound.
         * @param lowerZ Lower Z bound.
         * @param upperX Upper X bound.
         * @param upperZ Upper Z bound.
         * @param terminateFlag Optional flag to terminate the operation.
         * @return A vector of all Buried Treasure positions within bounds.
         */
        MU static std::vector<Pos2D> getAllPositionsBounded(const Generator *g,
                                                            int lowerX, int lowerZ, int upperX, int upperZ,
                                                            std::atomic_bool *terminateFlag = nullptr) {
            std::vector<Pos2D> positions = DynamicStructure::getAllPositionsBounded(
                g, lowerX, lowerZ, upperX, upperZ, terminateFlag);
            for (Pos2D &pos: positions) {
                pos.z += 1;
            }
            return positions;
        }
    };

    /**
     * @class Shipwreck
     * @brief Represents a Shipwreck structure.
     */
    class Shipwreck : public DynamicStructure<Shipwreck> {
    public:
        constexpr static int Placement::DynamicStructure<Shipwreck>::SALT = 14357617;
        constexpr static int Placement::DynamicStructure<Shipwreck>::MAIN_RADIUS = 10;
        constexpr static bool Placement::DynamicStructure<Shipwreck>::HAS_SECOND_BIOME_CHECK = false;
        constexpr static u64 Placement::DynamicStructure<Shipwreck>::MAIN_VALID_BIOMES =
            1ULL << ocean | 1ULL << mushroom_island_shore | 1ULL << beach | 1ULL << snowy_beach | 1ULL << deep_ocean |
            1ULL << warm_ocean | 1ULL << lukewarm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << cold_ocean |
            1ULL << deep_cold_ocean | 1ULL << frozen_ocean | 1ULL << deep_frozen_ocean;
        /**
         * @brief Sets the world size for the Shipwreck.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    /**
     * @class Outpost
     * @brief Represents an Outpost structure.
     */
    class Outpost : public DynamicStructure<Outpost> {
    public:
        constexpr static int Placement::DynamicStructure<Outpost>::SALT = 165745296;
        constexpr static int Placement::DynamicStructure<Outpost>::MAIN_RADIUS = 32;
        constexpr static bool Placement::DynamicStructure<Outpost>::HAS_SECOND_BIOME_CHECK = false;
        constexpr static u64 Placement::DynamicStructure<Outpost>::MAIN_VALID_BIOMES =
            1ULL << plains | 1ULL << desert | 1ULL << taiga | 1ULL << ice_plains | 1ULL << cold_taiga | 1ULL << savanna;
        /**
         * @brief Sets the world size for the Outpost.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };
} // namespace Placement
