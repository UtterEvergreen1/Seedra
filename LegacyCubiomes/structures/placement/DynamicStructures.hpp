#pragma once

#include <vector>
#include "LegacyCubiomes/cubiomes/biomeID.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"


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
        static c_u64 SECONDARY_VALID_BIOMES;
        static c_u64 SECONDARY_VALID_BIOMES_MUTATED;

        static c_int SALT;
        static int REGION_SIZE;
        static int CHUNK_RANGE;
        static int ATTEMPTS;
        static c_int MAIN_RADIUS;
        static c_int SECOND_RADIUS;
        static c_bool HAS_SECOND_BIOME_CHECK;
        static bool HAS_MAX_ATTEMPTS;
        static int CHUNK_BOUNDS;
        static bool REDUCED_SPACING;

        static Pos2D getPosition(const Generator* g, int regionX, int regionZ);
        static std::vector<Pos2D> getAllPositions(const Generator* g);
        static bool canSpawnAtChunk(i64 worldSeed, int chunkX, int chunkZ, int regionX, int regionZ);
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
