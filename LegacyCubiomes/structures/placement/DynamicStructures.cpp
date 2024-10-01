#include "DynamicStructures.hpp"

#include "LegacyCubiomes/utils/rng.hpp"


#include <unordered_set>

namespace Placement {

    // defaults
    template<typename Derived>
    c_u64 DynamicStructure<Derived>::SECONDARY_VALID_BIOMES = 0;
    template<typename Derived>
    c_u64 DynamicStructure<Derived>::SECONDARY_VALID_BIOMES_MUTATED = 0;
    template<typename Derived>
    int DynamicStructure<Derived>::REGION_SIZE = 32;
    template<typename Derived>
    int DynamicStructure<Derived>::ATTEMPTS = 60;
    template<typename Derived>
    c_bool DynamicStructure<Derived>::HAS_SECOND_BIOME_CHECK = true;
    template<typename Derived>
    int DynamicStructure<Derived>::CHUNK_BOUNDS = 24;
    template<typename Derived>
    bool DynamicStructure<Derived>::REDUCED_SPACING = true;

    template<typename Derived>
    Pos2D DynamicStructure<Derived>::getPosition(const Generator *g, c_int regionX, c_int regionZ) {
        RNG rnds;
        c_i64 featureSeed = static_cast<i64>(regionX * REGION_SIZE) * 341873128712ULL +
                            static_cast<i64>(regionZ * REGION_SIZE) * 132897987541ULL + g->getWorldSeed() + SALT;
        rnds.setSeed(featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        for (int attempts = 0; attempts < ATTEMPTS; attempts++) {
            int xChunk = rnds.nextInt(CHUNK_RANGE);
            int zChunk = rnds.nextInt(CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) { // successfully placed
                xChunk = regionX * REGION_SIZE + xChunk;
                zChunk = regionZ * REGION_SIZE + zChunk;
                // qDebug() << xChunk << zChunk;
                int xPos = (xChunk << 4) + 8;
                int zPos = (zChunk << 4) + 8;
                if (xChunk < CHUNK_BOUNDS && xChunk >= -CHUNK_BOUNDS && zChunk < CHUNK_BOUNDS &&
                    zChunk >= -CHUNK_BOUNDS) {
                    if (g->areBiomesViable(xPos, zPos, MAIN_RADIUS, MAIN_VALID_BIOMES)) {
                        if (!HAS_SECOND_BIOME_CHECK) { return {xPos, zPos}; }
                        if (g->areBiomesViable(xPos, zPos, SECOND_RADIUS, SECONDARY_VALID_BIOMES,
                                               SECONDARY_VALID_BIOMES_MUTATED)) {
                            return {xPos, zPos};
                        }
                    }
                }
            } else {
                attempts--;
            }
        }
        return {0, 0};
    }

    template<typename Derived>
    std::vector<Pos2D> DynamicStructure<Derived>::getAllPositions(const Generator *g, std::atomic_bool* terminateFlag) {
        return getAllPositionsBounded(g, -g->getWorldCoordinateBounds(), -g->getWorldCoordinateBounds(),
                                      g->getWorldCoordinateBounds(), g->getWorldCoordinateBounds(), terminateFlag);
    }

    template<typename Derived>
    std::vector<Pos2D>
    DynamicStructure<Derived>::getAllPositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX,
                                                      int upperZ, std::atomic_bool* terminateFlag) {
        std::vector<Pos2D> positions;
        c_int numXRegions = (upperX - lowerX) / REGION_SIZE;
        c_int numZRegions = (upperZ - lowerZ) / REGION_SIZE;
        for (int regionX = -numXRegions - 1; regionX <= numXRegions; ++regionX) {
            for (int regionZ = -numZRegions - 1; regionZ <= numZRegions; ++regionZ) {
                if (terminateFlag && terminateFlag->load()) return positions;
                if (Pos2D structPos = getPosition(g, regionX, regionZ); structPos != 0 &&
                                                                        structPos.insideBounds(lowerX, lowerZ, upperX,
                                                                                               upperZ))
                    positions.push_back(structPos);
            }
        }
        return positions;
    }

    template<typename Derived>
    bool DynamicStructure<Derived>::canSpawnAtChunk(c_i64 worldSeed, c_int chunkX, c_int chunkZ, c_int regionX,
                                                    c_int regionZ) {
        RNG rnds;
        c_i64 featureSeed = static_cast<i64>(regionX * REGION_SIZE) * 341873128712ULL +
                            static_cast<i64>(regionZ * REGION_SIZE) * 132897987541ULL + worldSeed + SALT;
        rnds.setSeed(featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        for (int attempts = 0; attempts < ATTEMPTS; attempts++) {
            int xChunk = rnds.nextInt(CHUNK_RANGE);
            int zChunk = rnds.nextInt(CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) { // successfully placed
                xChunk = regionX * REGION_SIZE + xChunk;
                zChunk = regionZ * REGION_SIZE + zChunk;
                if (chunkX == xChunk && zChunk == chunkZ) return true;
            } else {
                attempts--;
            }
        }
        return false;
    }


    template<>
    c_int DynamicStructure<Mansion>::SALT = 10387319;
    template<>
    int DynamicStructure<Mansion>::CHUNK_RANGE = 26;
    template<>
    c_int DynamicStructure<Mansion>::MAIN_RADIUS = 4;
    template<>
    c_int DynamicStructure<Mansion>::SECOND_RADIUS = 32;
    template<>
    c_bool DynamicStructure<Mansion>::HAS_SECOND_BIOME_CHECK = false;
    template<>
    c_u64 DynamicStructure<Mansion>::MAIN_VALID_BIOMES = (1ULL << dark_forest);
    template<>
    c_u64 DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
    template<>
    c_u64 DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    void Mansion::setWorldSize(const lce::WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        c_bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REDUCED_SPACING = reducedSpacing;
        REGION_SIZE = reducedSpacing ? 32 : 80;
        CHUNK_RANGE = REGION_SIZE - 6;
        ATTEMPTS = reducedSpacing ? 60 : 40;
    }


    template<>
    c_int DynamicStructure<Monument>::SALT = 10387313;
    template<>
    int DynamicStructure<Monument>::CHUNK_RANGE = 27;
    template<>
    c_int DynamicStructure<Monument>::MAIN_RADIUS = 8;
    template<>
    c_int DynamicStructure<Monument>::SECOND_RADIUS = 29;
    template<>
    c_bool DynamicStructure<Monument>::HAS_SECOND_BIOME_CHECK = true;
    template<>
    c_u64 DynamicStructure<Monument>::MAIN_VALID_BIOMES =
            1ULL << ocean | 1ULL << river | 1ULL << frozen_river | 1ULL << deep_ocean | 1ULL << warm_ocean |
            1ULL << deep_warm_ocean | 1ULL << lukewarm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << cold_ocean |
            1ULL << deep_cold_ocean | 1ULL << frozen_ocean | 1ULL << deep_frozen_ocean;
    template<>
    c_u64 DynamicStructure<Monument>::SECONDARY_VALID_BIOMES =
            1ULL << deep_ocean | 1ULL << deep_warm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << deep_cold_ocean |
            1ULL << deep_frozen_ocean;

    void Monument::setWorldSize(const lce::WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        c_bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REDUCED_SPACING = reducedSpacing;
        REGION_SIZE = reducedSpacing ? 32 : 80;
        CHUNK_RANGE = REGION_SIZE - 5;
        ATTEMPTS = reducedSpacing ? 60 : 40;
    }


    template<>
    c_int DynamicStructure<BuriedTreasure>::SALT = 16842397;
    template<>
    int DynamicStructure<BuriedTreasure>::CHUNK_RANGE = 30;
    template<>
    c_int DynamicStructure<BuriedTreasure>::MAIN_RADIUS = 0;
    template<>
    c_int DynamicStructure<BuriedTreasure>::SECOND_RADIUS = 16;
    template<>
    c_bool DynamicStructure<BuriedTreasure>::HAS_SECOND_BIOME_CHECK = false;
    template<>
    c_u64 DynamicStructure<BuriedTreasure>::MAIN_VALID_BIOMES =
            1ULL << mushroom_island_shore | 1ULL << beach | 1ULL << stone_beach | 1ULL << cold_beach;
    template<>
    c_u64 DynamicStructure<BuriedTreasure>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
    template<>
    c_u64 DynamicStructure<BuriedTreasure>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    void BuriedTreasure::setWorldSize(const lce::WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        c_bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REDUCED_SPACING = reducedSpacing;
        REGION_SIZE = reducedSpacing ? 32 : 4;
        CHUNK_RANGE = REGION_SIZE - 2;
        ATTEMPTS = reducedSpacing ? 60 : 4;
    }


    template<>
    c_int DynamicStructure<Shipwreck>::SALT = 14357617;
    template<>
    int DynamicStructure<Shipwreck>::CHUNK_RANGE = 27;
    template<>
    c_int DynamicStructure<Shipwreck>::MAIN_RADIUS = 10;
    template<>
    c_int DynamicStructure<Shipwreck>::SECOND_RADIUS = 0;
    template<>
    c_bool DynamicStructure<Shipwreck>::HAS_SECOND_BIOME_CHECK = false;
    template<>
    c_u64 DynamicStructure<Shipwreck>::MAIN_VALID_BIOMES =
            1ULL << ocean | 1ULL << mushroom_island_shore | 1ULL << beach | 1ULL << snowy_beach | 1ULL << deep_ocean |
            1ULL << warm_ocean | 1ULL << lukewarm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << cold_ocean |
            1ULL << deep_cold_ocean | 1ULL << frozen_ocean | 1ULL << deep_frozen_ocean;

    void Shipwreck::setWorldSize(const lce::WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        c_bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REDUCED_SPACING = reducedSpacing;
        REGION_SIZE = reducedSpacing ? 32 : 10;
        CHUNK_RANGE = REGION_SIZE - 5;
        ATTEMPTS = reducedSpacing ? 60 : 20;
    }


    template<>
    c_int DynamicStructure<Outpost>::SALT = 165745296;
    template<>
    int DynamicStructure<Outpost>::CHUNK_RANGE = 26;
    template<>
    c_int DynamicStructure<Outpost>::MAIN_RADIUS = 32;
    template<>
    c_int DynamicStructure<Outpost>::SECOND_RADIUS = 0;
    template<>
    c_bool DynamicStructure<Outpost>::HAS_SECOND_BIOME_CHECK = false;
    template<>
    c_u64 DynamicStructure<Outpost>::MAIN_VALID_BIOMES =
            1ULL << plains | 1ULL << desert | 1ULL << taiga | 1ULL << ice_plains | 1ULL << cold_taiga | 1ULL << savanna;
    template<>
    c_u64 DynamicStructure<Outpost>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
    template<>
    c_u64 DynamicStructure<Outpost>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    void Outpost::setWorldSize(const lce::WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        c_bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REDUCED_SPACING = reducedSpacing;
        REGION_SIZE = reducedSpacing ? 32 : 80;
        CHUNK_RANGE = REGION_SIZE - 6;
        ATTEMPTS = reducedSpacing ? 60 : 40;
    }
} // namespace Placement


template
class Placement::DynamicStructure<Placement::Mansion>;

template
class Placement::DynamicStructure<Placement::Monument>;

template
class Placement::DynamicStructure<Placement::BuriedTreasure>;

template
class Placement::DynamicStructure<Placement::Shipwreck>;

template
class Placement::DynamicStructure<Placement::Outpost>;
