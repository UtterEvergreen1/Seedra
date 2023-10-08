#include "DynamicStructures.hpp"
namespace Placement {
    // defaults
    template<typename Derived>
    const uint64_t DynamicStructure<Derived>::SECONDARY_VALID_BIOMES = 0;
    template<typename Derived>
    const uint64_t DynamicStructure<Derived>::SECONDARY_VALID_BIOMES_MUTATED = 0;
    template <typename Derived>
    int DynamicStructure<Derived>::REGION_SIZE = 32;
    template <typename Derived>
    int DynamicStructure<Derived>::ATTEMPTS = 60;
    template<typename Derived>
    const bool DynamicStructure<Derived>::HAS_SECOND_BIOME_CHECK = true;
    template<typename Derived>
    int DynamicStructure<Derived>::CHUNK_BOUNDS = 24;
    template<typename Derived>
    bool DynamicStructure<Derived>::REDUCED_SPACING = true;

    template<typename Derived>
    Pos2D DynamicStructure<Derived>::getPosition(Generator* g, int regionX, int regionZ) {
        uint64_t rnds;
        int64_t featureSeed = ((int64_t) regionX * REGION_SIZE) * 341873128712ULL +
                              ((int64_t) regionZ * REGION_SIZE) * 132897987541ULL + g->getWorldSeed() + SALT;
        setSeed(&rnds, featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        int xChunk;
        int zChunk;
        for (int attempts = 0; attempts < ATTEMPTS; attempts++) {
            xChunk = nextInt(&rnds, CHUNK_RANGE);
            zChunk = nextInt(&rnds, CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) { // successfully placed
                xChunk = regionX * REGION_SIZE + xChunk;
                zChunk = regionZ * REGION_SIZE + zChunk;
                //qDebug() << xChunk << zChunk;
                int xPos = (xChunk << 4) + 8;
                int zPos = (zChunk << 4) + 8;
                if (xChunk < CHUNK_BOUNDS && xChunk >= -CHUNK_BOUNDS && zChunk < CHUNK_BOUNDS && zChunk >= -CHUNK_BOUNDS) {
                    if (g->areBiomesViable(xPos, zPos, MAIN_RADIUS, MAIN_VALID_BIOMES)) {
                        if (!HAS_SECOND_BIOME_CHECK ||
                            g->areBiomesViable(xPos, zPos, SECOND_RADIUS, SECONDARY_VALID_BIOMES, SECONDARY_VALID_BIOMES_MUTATED)) {
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
    std::vector<Pos2D> DynamicStructure<Derived>::getAllPositions(Generator* g) {
        Pos2D structPos;
        std::vector<Pos2D> positions;
        int numRegions = CHUNK_BOUNDS / REGION_SIZE;
        for (int regionX = -numRegions - 1; regionX <= numRegions; ++regionX) {
            for (int regionZ = -numRegions - 1; regionZ <= numRegions; ++regionZ) {
                structPos = getPosition(g, regionX, regionZ);
                if(structPos != 0)
                    positions.push_back(structPos);
            }
        }
        return positions;
    }

    template<typename Derived>
    bool DynamicStructure<Derived>::canSpawnAtChunk(int64_t worldSeed, int chunkX, int chunkZ, int regionX, int regionZ) {
        uint64_t rnds;
        int64_t featureSeed = ((int64_t)regionX * REGION_SIZE) * 341873128712ULL +
            ((int64_t)regionZ * REGION_SIZE) * 132897987541ULL + worldSeed + SALT;
        setSeed(&rnds, featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        int xChunk;
        int zChunk;
        for (int attempts = 0; attempts < ATTEMPTS; attempts++) {
            xChunk = nextInt(&rnds, CHUNK_RANGE);
            zChunk = nextInt(&rnds, CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) { // successfully placed
                xChunk = regionX * REGION_SIZE + xChunk;
                zChunk = regionZ * REGION_SIZE + zChunk;
                if (chunkX == xChunk && zChunk == chunkZ)
                    return true;
            }
            else {
                attempts--;
            }
        }
        return false;
    }

    template <>
    const int DynamicStructure<Mansion>::SALT = 10387319;
    template <>
    int DynamicStructure<Mansion>::CHUNK_RANGE = 26;
    template <>
    const int DynamicStructure<Mansion>::MAIN_RADIUS = 4;
    template <>
    const int DynamicStructure<Mansion>::SECOND_RADIUS = 32;
    template <>
    const uint64_t DynamicStructure<Mansion>::MAIN_VALID_BIOMES =
            (1ULL << dark_forest);
    template <>
    const uint64_t DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
    template <>
    const uint64_t DynamicStructure<Mansion>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;


    void Mansion::setWorldSize(WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if(REDUCED_SPACING == reducedSpacing)
            return;
        REGION_SIZE = reducedSpacing ? 32 : 80;
        CHUNK_RANGE = REGION_SIZE - 6;
        ATTEMPTS = reducedSpacing ? 60 : 40;
    }

    template <>
    const int DynamicStructure<Monument>::SALT = 10387313;
    template <>
    int DynamicStructure<Monument>::CHUNK_RANGE = 27;
    template <>
    const int DynamicStructure<Monument>::MAIN_RADIUS = 8;
    template <>
    const int DynamicStructure<Monument>::SECOND_RADIUS = 29;
    template <>
    const uint64_t DynamicStructure<Monument>::MAIN_VALID_BIOMES =
            (1ULL << ocean) |
            (1ULL << river) |
            (1ULL << frozen_river) |
            (1ULL << deep_ocean) |
            (1ULL << warm_ocean) |
            (1ULL << deep_warm_ocean) |
            (1ULL << lukewarm_ocean) |
            (1ULL << deep_lukewarm_ocean) |
            (1ULL << cold_ocean) |
            (1ULL << deep_cold_ocean) |
            (1ULL << frozen_ocean) |
            (1ULL << deep_frozen_ocean);

    template <>
    const uint64_t DynamicStructure<Monument>::SECONDARY_VALID_BIOMES =
            (1ULL << deep_ocean) |
            (1ULL << deep_warm_ocean) |
            (1ULL << deep_lukewarm_ocean) |
            (1ULL << deep_cold_ocean) |
            (1ULL << deep_frozen_ocean);

    void Monument::setWorldSize(WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if(REDUCED_SPACING == reducedSpacing)
            return;
        REGION_SIZE = reducedSpacing ? 32 : 80;
        CHUNK_RANGE = REGION_SIZE - 5;
        ATTEMPTS = reducedSpacing ? 60 : 40;
    }

    template <>
    const int DynamicStructure<Treasure>::SALT = 16842397;
    template <>
    int DynamicStructure<Treasure>::CHUNK_RANGE = 30;
    template <>
    const int DynamicStructure<Treasure>::MAIN_RADIUS = 0;
    template <>
    const int DynamicStructure<Treasure>::SECOND_RADIUS = 16;
    template <>
    const uint64_t DynamicStructure<Treasure>::MAIN_VALID_BIOMES =
            (1ULL << mushroom_island_shore) |
            (1ULL << beach) |
            (1ULL << stone_beach) |
            (1ULL << cold_beach);

    template <>
    const uint64_t DynamicStructure<Treasure>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
    template <>
    const uint64_t DynamicStructure<Treasure>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    void Treasure::setWorldSize(WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if(REDUCED_SPACING == reducedSpacing)
            return;
        REGION_SIZE = reducedSpacing ? 32 : 4;
        CHUNK_RANGE = REGION_SIZE - 2;
        ATTEMPTS = reducedSpacing ? 60 : 4;
    }

    template <>
    const int DynamicStructure<Shipwreck>::SALT = 14357617;
    template <>
    int DynamicStructure<Shipwreck>::CHUNK_RANGE = 27;
    template <>
    const int DynamicStructure<Shipwreck>::MAIN_RADIUS = 10;
    template <>
    const int DynamicStructure<Shipwreck>::SECOND_RADIUS = 0;
    template <>
    const uint64_t DynamicStructure<Shipwreck>::MAIN_VALID_BIOMES =
            (1ULL << ocean) |
            (1ULL << mushroom_island_shore) |
            (1ULL << beach) |
            (1ULL << snowy_beach) |
            (1ULL << deep_ocean) |
            (1ULL << warm_ocean) |
            (1ULL << lukewarm_ocean) |
            (1ULL << deep_lukewarm_ocean) |
            (1ULL << cold_ocean) |
            (1ULL << deep_cold_ocean) |
            (1ULL << frozen_ocean) |
            (1ULL << deep_frozen_ocean);

    template <>
    const bool DynamicStructure<Shipwreck>::HAS_SECOND_BIOME_CHECK = false;

    void Shipwreck::setWorldSize(WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if(REDUCED_SPACING == reducedSpacing)
            return;
        REGION_SIZE = reducedSpacing ? 32 : 10;
        CHUNK_RANGE = REGION_SIZE - 5;
        ATTEMPTS = reducedSpacing ? 60 : 20;
    }

    template <>
    const int DynamicStructure<Outpost>::SALT = 165745296;
    template <>
    int DynamicStructure<Outpost>::CHUNK_RANGE = 26;
    template <>
    const int DynamicStructure<Outpost>::MAIN_RADIUS = 32;
    template <>
    const int DynamicStructure<Outpost>::SECOND_RADIUS = 0;
    template <>
    const uint64_t DynamicStructure<Outpost>::MAIN_VALID_BIOMES =
            (1ULL << plains) |
            (1ULL << desert) |
            (1ULL << taiga) |
            (1ULL << ice_plains) |
            (1ULL << cold_taiga) |
            (1ULL << savanna);

    template <>
    const uint64_t DynamicStructure<Outpost>::SECONDARY_VALID_BIOMES = DEFAULT_SECONDARY_VALID_BIOMES;
    template <>
    const uint64_t DynamicStructure<Outpost>::SECONDARY_VALID_BIOMES_MUTATED = DEFAULT_SECONDARY_VALID_BIOMES_MUTATED;

    void Outpost::setWorldSize(WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if(REDUCED_SPACING == reducedSpacing)
            return;
        REGION_SIZE = reducedSpacing ? 32 : 80;
        CHUNK_RANGE = REGION_SIZE - 6;
        ATTEMPTS = reducedSpacing ? 60 : 40;
    }
}

template class Placement::DynamicStructure<Placement::Mansion>;
template class Placement::DynamicStructure<Placement::Monument>;
template class Placement::DynamicStructure<Placement::Treasure>;
template class Placement::DynamicStructure<Placement::Shipwreck>;
template class Placement::DynamicStructure<Placement::Outpost>;

