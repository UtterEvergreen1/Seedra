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
        static c_u64 MAIN_VALID_BIOMES; ///< Main valid biomes for the structure.
        MU static c_u64 SECONDARY_VALID_BIOMES; ///< Secondary valid biomes for the structure.
        MU static c_u64 SECONDARY_VALID_BIOMES_MUTATED; ///< Mutated secondary valid biomes.

        static c_int SALT; ///< Salt value for structure placement.
        MU static int REGION_SIZE; ///< Size of the region for structure placement.
        static int CHUNK_RANGE; ///< Range of chunks to consider for placement.
        MU static int ATTEMPTS; ///< Maximum number of placement attempts.
        static c_int MAIN_RADIUS; ///< Main radius for structure placement.
        static c_int SECOND_RADIUS; ///< Secondary radius for structure placement.
        MU static c_bool HAS_SECOND_BIOME_CHECK; ///< Flag for secondary biome checks.
        MU static bool HAS_MAX_ATTEMPTS; ///< Flag for maximum attempts.
        MU static int CHUNK_BOUNDS; ///< Bounds for chunk placement.
        MU static bool REDUCED_SPACING; ///< Flag for reduced spacing between structures.

        /**
         * @brief Gets the position of the structure in a region.
         * @param g Pointer to the generator.
         * @param regionX The X coordinate of the region.
         * @param regionZ The Z coordinate of the region.
         * @return The position of the structure.
         */
        static Pos2D getPosition(const Generator *g, int regionX, int regionZ);

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
        /**
         * @brief Sets the world size for the Outpost.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };
} // namespace Placement
