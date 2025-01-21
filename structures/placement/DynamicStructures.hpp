#pragma once

#include "common/Pos2DTemplate.hpp"
#include "terrain/biomes/biomeID.hpp"
#include "terrain/biomes/layers.hpp"
#include "terrain/generator.hpp"
#include <atomic>
#include <vector>


namespace Placement {
    static constexpr u64 DEFAULT_SECONDARY_VALID_BIOMES =
            1ULL << plains | 1ULL << desert | 1ULL << forest | 1ULL << taiga | 1ULL << swampland |
            1ULL << river | 1ULL << hell | 1ULL << the_end | 1ULL << legacy_frozen_ocean |
            1ULL << frozen_river | 1ULL << ice_plains | 1ULL << ice_mountains | 1ULL << mushroom_island |
            1ULL << mushroom_island_shore | 1ULL << beach | 1ULL << desert_hills | 1ULL << forest_hills |
            1ULL << taiga_hills | 1ULL << extreme_hills_edge | 1ULL << jungle | 1ULL << jungle_hills |
            1ULL << jungle_edge | 1ULL << cold_beach | 1ULL << birch_forest | 1ULL << birch_forest_hills |
            1ULL << roofed_forest | 1ULL << cold_taiga | 1ULL << cold_taiga_hills | 1ULL << mega_taiga |
            1ULL << mega_taiga_hills | 1ULL << savanna | 1ULL << savanna_plateau | 1ULL << mesa |
            1ULL << mesa_plateau_stone | 1ULL << mesa_plateau;

    static constexpr u64 DEFAULT_SECONDARY_VALID_BIOMES_MUTATED =
            1ULL << (sunflower_plains - 128) | 1ULL << (desert_mutated - 128) |
            1ULL << (swampland_mutated - 128) | 1ULL << (mega_spruce_taiga - 128) |
            1ULL << (redwood_taiga_hills_mutated - 128) | 1ULL << (mesa_bryce - 128) |
            1ULL << (mesa_plateau_stone_mutated - 128) | 1ULL << (mesa_plateau_mutated - 128);

    template<typename Derived>
    class DynamicStructure {
    public:
        static c_u64 MAIN_VALID_BIOMES;
        MU static c_u64 SECONDARY_VALID_BIOMES;
        MU static c_u64 SECONDARY_VALID_BIOMES_MUTATED;

        static c_int SALT;
        MU static int REGION_SIZE;
        static int CHUNK_RANGE;
        MU static int ATTEMPTS;
        static c_int MAIN_RADIUS;
        static c_int SECOND_RADIUS;
        MU static c_bool HAS_SECOND_BIOME_CHECK;
        MU static bool HAS_MAX_ATTEMPTS;
        MU static int CHUNK_BOUNDS;
        MU static bool REDUCED_SPACING;

        static Pos2D getPosition(const Generator* g, int regionX, int regionZ);
        static std::vector<Pos2D> getAllPositions(const Generator* g, std::atomic_bool* terminateFlag = nullptr);

        static std::vector<Pos2D>
        getAllPositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX, int upperZ, std::atomic_bool* terminateFlag = nullptr);
        MU static bool canSpawnAtChunk(i64 worldSeed, int chunkX, int chunkZ, int regionX, int regionZ);
    };

    class Mansion : public DynamicStructure<Mansion> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    class Monument : public DynamicStructure<Monument> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    class BuriedTreasure : public DynamicStructure<BuriedTreasure> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
        MU static std::vector<Pos2D> getAllPositions(const Generator* g, std::atomic_bool* terminateFlag = nullptr) {
            std::vector<Pos2D> positions = DynamicStructure::getAllPositions(g, terminateFlag);
            for (Pos2D& pos : positions) {
                pos.z += 1;
            }
            return positions;
        }

        MU static std::vector<Pos2D> getAllPositionsBounded(const Generator *g,
            int lowerX, int lowerZ, int upperX, int upperZ, std::atomic_bool* terminateFlag = nullptr) {
            std::vector<Pos2D> positions = DynamicStructure::getAllPositionsBounded(g, lowerX, lowerZ, upperX, upperZ, terminateFlag);
            for (Pos2D& pos : positions) {
                pos.z += 1;
            }
            return positions;
        }
    };

    class Shipwreck : public DynamicStructure<Shipwreck> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    class Outpost : public DynamicStructure<Outpost> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };
} // namespace Placement
