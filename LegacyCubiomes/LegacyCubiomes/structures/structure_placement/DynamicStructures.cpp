#include "DynamicStructures.hpp"
namespace Structure {
    template<typename Derived>
    char DynamicStructure<Derived>::MAIN_VALID_BIOMES[Structure::ARRAY_SIZE];
    template<typename Derived>
    char DynamicStructure<Derived>::SECOND_VALID_BIOMES[Structure::ARRAY_SIZE];

    template<typename Derived>
    int DynamicStructure<Derived>::SALT = 0;
    template<typename Derived>
    int DynamicStructure<Derived>::REGION_SIZE = 0;
    template<typename Derived>
    int DynamicStructure<Derived>::CHUNK_RANGE = 0;
    template<typename Derived>
    int DynamicStructure<Derived>::MAX_ATTEMPT = 0;
    template<typename Derived>
    int DynamicStructure<Derived>::MAIN_RADIUS = 0;
    template<typename Derived>
    int DynamicStructure<Derived>::SECOND_RADIUS = 0;

    template<typename Derived>
    bool DynamicStructure<Derived>::HAS_SECOND_BIOME_CHECK = false;

    template<typename Derived>
    int DynamicStructure<Derived>::CHUNK_BOUNDS = 0;

    template<typename Derived>
    void DynamicStructure<Derived>::setupDerived(int salt, int regionSize, int spacing, int attempts, int mainRadius,
                                        int secondRadius, const std::vector<int> &biomeListMain,
                                        const std::vector<int> &biomeListSecond, WORLDSIZE worldSize) {
        Derived::SALT = salt;
        Derived::REGION_SIZE = regionSize;
        Derived::CHUNK_RANGE = regionSize - spacing;
        Derived::MAX_ATTEMPT = attempts;
        Derived::MAIN_RADIUS = mainRadius;
        Derived::SECOND_RADIUS = secondRadius;

        Derived::CHUNK_BOUNDS = getChunkWorldBounds(worldSize) - 3;

        Derived::HAS_SECOND_BIOME_CHECK = biomeListSecond.size() > 0;
        for (unsigned int i = 0; i < biomeListMain.size(); i++)
            Derived::MAIN_VALID_BIOMES[biomeListMain[i]] = 1;

        for (unsigned int i = 0; i < biomeListSecond.size(); i++)
            Derived::SECOND_VALID_BIOMES[biomeListSecond[i]] = 1;
    }

    template<typename Derived>
    Pos2D DynamicStructure<Derived>::getPosition(Generator* g, int regionX, int regionZ) {
        uint64_t rnds;
        int64_t featureSeed = ((int64_t) regionX * REGION_SIZE) * 341873128712ULL +
                              ((int64_t) regionZ * REGION_SIZE) * 132897987541ULL + g->seed + SALT;
        setSeed(&rnds, featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        int xChunk;
        int zChunk;
        for (int attempts = 0; attempts < MAX_ATTEMPT; attempts++) {
            xChunk = nextInt(&rnds, CHUNK_RANGE);
            zChunk = nextInt(&rnds, CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) { // successfully placed
                xChunk = regionX * REGION_SIZE + xChunk;
                zChunk = regionZ * REGION_SIZE + zChunk;
                //qDebug() << xChunk << zChunk;
                int xPos = (xChunk << 4) + 8;
                int zPos = (zChunk << 4) + 8;
                if (xChunk < CHUNK_BOUNDS && xChunk >= -CHUNK_BOUNDS && zChunk < CHUNK_BOUNDS && zChunk >= -CHUNK_BOUNDS) {
                    if (g->areBiomesViable(xPos, zPos, MAIN_RADIUS, MAIN_VALID_BIOMES, 0)) {
                        if (!HAS_SECOND_BIOME_CHECK ||
                            g->areBiomesViable(xPos, zPos, SECOND_RADIUS, SECOND_VALID_BIOMES, 0)) {
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
        int numRegions = CHUNK_BOUNDS / REGION_SIZE;
        std::vector<Pos2D> positions;
        for (int regionX = -numRegions - 1; regionX <= numRegions; ++regionX) {
            for (int regionZ = -numRegions - 1; regionZ <= numRegions; ++regionZ) {
                positions.push_back(getPosition(g, regionX, regionZ));
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
        for (int attempts = 0; attempts < MAX_ATTEMPT; attempts++) {
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

    void Mansion::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeListMain = { dark_forest, roofed_forest_mutated};

        std::vector<int> biomeListSecond = {
            ocean, deep_ocean, warm_ocean, deep_warm_ocean, lukewarm_ocean,
            deep_lukewarm_ocean, cold_ocean, deep_cold_ocean, frozen_ocean,
            deep_frozen_ocean, plains, desert, forest, taiga, swampland, river, hell, the_end, legacy_frozen_ocean,
            frozen_river, ice_plains, ice_mountains,
            mushroom_island, mushroom_island_shore, beach, desert_hills, forest_hills, taiga_hills,
            extreme_hills_edge,
            jungle, jungle_hills, jungle_edge, cold_beach, birch_forest, birch_forest_hills,
            roofed_forest, cold_taiga, cold_taiga_hills, mega_taiga, mega_taiga_hills, savanna, savanna_plateau,
            mesa,
            mesa_plateau_stone, mesa_plateau, the_void, sunflower_plains, desert_mutated, swampland_mutated,
            mega_spruce_taiga, redwood_taiga_hills_mutated, mesa_bryce, mesa_plateau_stone_mutated,
            mesa_plateau_mutated
        };

        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        setupDerived(10387319, useReducedSpacing ? 32 : 80, 6,
                     useReducedSpacing ? 60 : 40, 4, 32, biomeListMain, biomeListSecond, worldSize);
    }

    void Monument::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeListMain =
        {
            ocean, deep_ocean, cold_ocean, deep_cold_ocean,
            frozen_ocean, deep_frozen_ocean, lukewarm_ocean,
            deep_lukewarm_ocean, warm_ocean, deep_warm_ocean,
            river, frozen_river
        };

        std::vector<int> biomeListSecond =
        {
            deep_ocean, deep_cold_ocean, deep_lukewarm_ocean, deep_warm_ocean, deep_frozen_ocean
        };

        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        setupDerived(10387313, useReducedSpacing ? 32 : 80, 5,
                     useReducedSpacing ? 60 : 40, 8, 29, biomeListMain, biomeListSecond, worldSize);
    }

    void Treasure::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeListMain = {
            stone_beach, mushroom_island_shore, beach, cold_beach
        };

        std::vector<int> biomeListSecond = {
            ocean, deep_ocean, warm_ocean, deep_warm_ocean, lukewarm_ocean,
            deep_lukewarm_ocean, cold_ocean, deep_cold_ocean, frozen_ocean,
            deep_frozen_ocean, plains, desert, forest, taiga, swampland, river, hell, the_end, legacy_frozen_ocean,
            frozen_river, ice_plains, ice_mountains,
            mushroom_island, mushroom_island_shore, beach, desert_hills, forest_hills, taiga_hills,
            extreme_hills_edge,
            jungle, jungle_hills, jungle_edge, cold_beach, birch_forest, birch_forest_hills,
            roofed_forest, cold_taiga, cold_taiga_hills, mega_taiga, mega_taiga_hills, savanna, savanna_plateau,
            mesa,
            mesa_plateau_stone, mesa_plateau, the_void, sunflower_plains, desert_mutated, swampland_mutated,
            mega_spruce_taiga, redwood_taiga_hills_mutated, mesa_bryce, mesa_plateau_stone_mutated,
            mesa_plateau_mutated
        };

        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        setupDerived(16842397, useReducedSpacing ? 32 : 4, 2,
                     useReducedSpacing ? 60 : 4, 0, 16, biomeListMain, biomeListSecond, worldSize);
    }

    void Shipwreck::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeListMain = {
            beach, snowy_beach, mushroom_island_shore, ocean, deep_ocean, cold_ocean,
            deep_cold_ocean, lukewarm_ocean, deep_lukewarm_ocean, frozen_ocean,
            deep_frozen_ocean, warm_ocean
        };

        std::vector<int> biomeListSecond;

        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        setupDerived(14357617, useReducedSpacing ? 32 : 10, 5,
                     useReducedSpacing ? 60 : 20, 0, 16, biomeListMain, biomeListSecond, worldSize);
    }

    void Outpost::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeListMain = {
            plains, desert, savanna, taiga, cold_taiga, ice_plains
        };

        std::vector<int> biomeListSecond;

        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        setupDerived(165745296, useReducedSpacing ? 32 : 80, 6,
                     useReducedSpacing ? 60 : 40, 32, 0, biomeListMain, biomeListSecond, worldSize);
    }
}


template class Structure::DynamicStructure<Structure::Mansion>;
template class Structure::DynamicStructure<Structure::Monument>;
template class Structure::DynamicStructure<Structure::Treasure>;
template class Structure::DynamicStructure<Structure::Shipwreck>;
template class Structure::DynamicStructure<Structure::Outpost>;

