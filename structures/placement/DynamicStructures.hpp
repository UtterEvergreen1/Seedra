#pragma once

#include "common/Pos2DTemplate.hpp"
#include "terrain/biomes/biome_t.hpp"
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
    static constexpr u64 DEFAULT_SECONDARY_VALID_BIOMES = makeBiomeBitmask<
            biome_t::ocean, biome_t::plains, biome_t::desert, biome_t::forest, biome_t::taiga, biome_t::swampland,
            biome_t::river, biome_t::hell, biome_t::the_end, biome_t::legacy_frozen_ocean,
            biome_t::frozen_river, biome_t::ice_plains, biome_t::ice_mountains, biome_t::mushroom_island,
            biome_t::mushroom_island_shore, biome_t::beach, biome_t::desert_hills, biome_t::forest_hills,
            biome_t::taiga_hills, biome_t::extreme_hills_edge, biome_t::jungle, biome_t::jungle_hills,
            biome_t::jungle_edge, biome_t::deep_ocean, biome_t::cold_beach, biome_t::birch_forest, biome_t::birch_forest_hills,
            biome_t::roofed_forest, biome_t::cold_taiga, biome_t::cold_taiga_hills, biome_t::mega_taiga,
            biome_t::mega_taiga_hills, biome_t::savanna, biome_t::savanna_plateau, biome_t::mesa,
            biome_t::mesa_plateau_stone, biome_t::mesa_plateau, biome_t::warm_ocean, biome_t::deep_warm_ocean,
            biome_t::lukewarm_ocean, biome_t::deep_lukewarm_ocean, biome_t::cold_ocean, biome_t::deep_cold_ocean,
            biome_t::frozen_ocean, biome_t::deep_frozen_ocean>();

    /**
     * @brief Default valid mutated biomes for secondary structures.
     *
     * This constant defines a bitmask of mutated biomes that are considered valid for secondary structures.
     */
    static constexpr u64 DEFAULT_SECONDARY_VALID_BIOMES_MUTATED = makeBiomeBitmask<
        static_cast<biome_t>(static_cast<biome_u>(biome_t::sunflower_plains) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::desert_mutated) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::swampland_mutated) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::mega_spruce_taiga) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::redwood_taiga_hills_mutated) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::mesa_bryce) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::mesa_plateau_stone_mutated) - 128),
        static_cast<biome_t>(static_cast<biome_u>(biome_t::mesa_plateau_mutated) - 128)
    >();


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
        static constexpr u64 MAIN_VALID_BIOMES              = 0; ///< Main valid biomes for the structure.
        static constexpr u64 SECONDARY_VALID_BIOMES         = 0; ///< Secondary valid biomes for the structure.
        static constexpr u64 SECONDARY_VALID_BIOMES_MUTATED = 0; ///< Mutated secondary valid biomes.

        static int CHUNK_BOUNDS; ///< Bounds for chunk placement.
        static int REGION_SIZE; ///< Size of the region for structure placement.
        static int CHUNK_RANGE; ///< Range of chunks to consider for placement.
        static int ATTEMPTS; ///< Maximum number of placement attempts.
        MU static bool REDUCED_SPACING; ///< Flag for reduced spacing between structures.

        static constexpr int  SALT                        = 0; ///< Salt value for structure placement.
        static constexpr int  MAIN_RADIUS                 = 0; ///< Main radius for structure placement.
        static constexpr int  SECOND_RADIUS               = 0; ///< Secondary radius for structure placement.
        static constexpr bool HAS_SECOND_BIOME_CHECK      = true; ///< Flag for secondary biome checks.

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
        MU static bool verifyChunkPosition(const Generator *g, int chunkX, int chunkZ) {
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
                               const std::atomic_bool *terminateFlag = nullptr);

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
        static constexpr int  SALT                   = 10387319;
        static constexpr int  MAIN_RADIUS            = 4;
        static constexpr int  SECOND_RADIUS          = 32;
        static constexpr bool HAS_SECOND_BIOME_CHECK = true;

        static constexpr u64  MAIN_VALID_BIOMES =
                makeBiomeBitmask<biome_t::dark_forest>();
        static constexpr u64  SECONDARY_VALID_BIOMES         = DEFAULT_SECONDARY_VALID_BIOMES;
        static constexpr u64  SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;
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
        static constexpr int  SALT                   = 10387313;
        static constexpr int  MAIN_RADIUS            = 8;
        static constexpr int  SECOND_RADIUS          = 29;
        static constexpr bool HAS_SECOND_BIOME_CHECK = true;

        static constexpr u64  MAIN_VALID_BIOMES =
                makeBiomeBitmask<biome_t::ocean, biome_t::river, biome_t::frozen_river, biome_t::deep_ocean,
                                 biome_t::warm_ocean, biome_t::deep_warm_ocean, biome_t::lukewarm_ocean,
                                 biome_t::deep_lukewarm_ocean, biome_t::cold_ocean, biome_t::deep_cold_ocean,
                                 biome_t::frozen_ocean, biome_t::deep_frozen_ocean>();
        static constexpr u64  SECONDARY_VALID_BIOMES =
                makeBiomeBitmask<biome_t::deep_ocean, biome_t::deep_warm_ocean, biome_t::deep_lukewarm_ocean,
                                 biome_t::deep_cold_ocean, biome_t::deep_frozen_ocean>();

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
        static constexpr int  SALT                   = 16842397;
        static constexpr int  MAIN_RADIUS            = 0;
        static constexpr int  SECOND_RADIUS          = 16;
        static constexpr bool HAS_SECOND_BIOME_CHECK = true;

        static constexpr u64  MAIN_VALID_BIOMES =
                makeBiomeBitmask<biome_t::mushroom_island_shore, biome_t::beach, biome_t::stone_beach, biome_t::cold_beach>();
        static constexpr u64  SECONDARY_VALID_BIOMES         = DEFAULT_SECONDARY_VALID_BIOMES;
        static constexpr u64  SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

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
        static constexpr int  SALT                   = 14357617;
        static constexpr int  MAIN_RADIUS            = 10;
        static constexpr bool HAS_SECOND_BIOME_CHECK = false;

        static constexpr u64  MAIN_VALID_BIOMES =
                makeBiomeBitmask<biome_t::ocean, biome_t::mushroom_island_shore, biome_t::beach, biome_t::snowy_beach,
                                 biome_t::deep_ocean, biome_t::warm_ocean, biome_t::lukewarm_ocean,
                                 biome_t::deep_lukewarm_ocean, biome_t::cold_ocean, biome_t::deep_cold_ocean,
                                 biome_t::frozen_ocean, biome_t::deep_frozen_ocean>();

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
        static constexpr int  SALT                   = 165745296;
        static constexpr int  MAIN_RADIUS            = 32;
        static constexpr bool HAS_SECOND_BIOME_CHECK = false;

        static constexpr u64  MAIN_VALID_BIOMES =
                makeBiomeBitmask<biome_t::plains, biome_t::desert, biome_t::taiga,
                                 biome_t::ice_plains, biome_t::cold_taiga, biome_t::savanna>();

        /**
         * @brief Sets the world size for the Outpost.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };



    // Mansion
    // template<> inline constexpr int  DynamicStructure<Mansion>::SALT = 10387319;
    // template<> inline constexpr int  DynamicStructure<Mansion>::MAIN_RADIUS = 4;
    // template<> inline constexpr int  DynamicStructure<Mansion>::SECOND_RADIUS = 32;
    // template<> inline constexpr bool DynamicStructure<Mansion>::HAS_SECOND_BIOME_CHECK = true;
    // template<> MU inline constexpr u64 DynamicStructure<Mansion>::MAIN_VALID_BIOMES =
    //         makeBiomeBitmask<biome_t::dark_forest>();
    // template<> MU inline constexpr u64 DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES =
    //         DEFAULT_SECONDARY_VALID_BIOMES;
    // template<> MU inline constexpr u64 DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES_MUTATED =
    //         DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    // Monument
    // template<> inline constexpr int  DynamicStructure<Monument>::SALT = 10387313;
    // template<> inline constexpr int  DynamicStructure<Monument>::MAIN_RADIUS = 8;
    // template<> inline constexpr int  DynamicStructure<Monument>::SECOND_RADIUS = 29;
    // template<> inline constexpr bool DynamicStructure<Monument>::HAS_SECOND_BIOME_CHECK = true;
    // template<> MU inline constexpr u64 DynamicStructure<Monument>::MAIN_VALID_BIOMES =
    //         makeBiomeBitmask<biome_t::ocean, biome_t::river, biome_t::frozen_river, biome_t::deep_ocean,
    //                          biome_t::warm_ocean, biome_t::deep_warm_ocean, biome_t::lukewarm_ocean,
    //                          biome_t::deep_lukewarm_ocean, biome_t::cold_ocean, biome_t::deep_cold_ocean,
    //                          biome_t::frozen_ocean, biome_t::deep_frozen_ocean>();
    // template<> MU inline constexpr u64 DynamicStructure<Monument>::SECONDARY_VALID_BIOMES =
    //         makeBiomeBitmask<biome_t::deep_ocean, biome_t::deep_warm_ocean, biome_t::deep_lukewarm_ocean,
    //                          biome_t::deep_cold_ocean, biome_t::deep_frozen_ocean>();

    // // BuriedTreasure
    // template<> inline constexpr int  DynamicStructure<BuriedTreasure>::SALT = 16842397;
    // template<> inline constexpr int  DynamicStructure<BuriedTreasure>::MAIN_RADIUS = 0;
    // template<> inline constexpr int  DynamicStructure<BuriedTreasure>::SECOND_RADIUS = 16;
    // template<> inline constexpr bool DynamicStructure<BuriedTreasure>::HAS_SECOND_BIOME_CHECK = true;
    // template<> MU inline constexpr u64 DynamicStructure<BuriedTreasure>::MAIN_VALID_BIOMES =
    //         makeBiomeBitmask<biome_t::mushroom_island_shore, biome_t::beach, biome_t::stone_beach, biome_t::cold_beach>();
    // template<> MU inline constexpr u64 DynamicStructure<BuriedTreasure>::SECONDARY_VALID_BIOMES =
    //         DEFAULT_SECONDARY_VALID_BIOMES;
    // template<> MU inline constexpr u64 DynamicStructure<BuriedTreasure>::SECONDARY_VALID_BIOMES_MUTATED =
    //         DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    // // Shipwreck
    // template<> inline constexpr int  DynamicStructure<Shipwreck>::SALT = 14357617;
    // template<> inline constexpr int  DynamicStructure<Shipwreck>::MAIN_RADIUS = 10;
    // template<> inline constexpr bool DynamicStructure<Shipwreck>::HAS_SECOND_BIOME_CHECK = false;
    // template<> MU inline constexpr u64 DynamicStructure<Shipwreck>::MAIN_VALID_BIOMES =
    //         makeBiomeBitmask<biome_t::ocean, biome_t::mushroom_island_shore, biome_t::beach, biome_t::snowy_beach,
    //                          biome_t::deep_ocean, biome_t::warm_ocean, biome_t::lukewarm_ocean,
    //                          biome_t::deep_lukewarm_ocean, biome_t::cold_ocean, biome_t::deep_cold_ocean,
    //                          biome_t::frozen_ocean, biome_t::deep_frozen_ocean>();

    // // Outpost
    // template<> inline constexpr int  DynamicStructure<Outpost>::SALT = 165745296;
    // template<> inline constexpr int  DynamicStructure<Outpost>::MAIN_RADIUS = 32;
    // template<> inline constexpr bool DynamicStructure<Outpost>::HAS_SECOND_BIOME_CHECK = false;
    // template<> MU inline constexpr u64 DynamicStructure<Outpost>::MAIN_VALID_BIOMES =
    //         makeBiomeBitmask<biome_t::plains, biome_t::desert, biome_t::taiga,
    //                          biome_t::ice_plains, biome_t::cold_taiga, biome_t::savanna>();


} // namespace Placement
