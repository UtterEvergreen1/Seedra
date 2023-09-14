#include "StaticStructures.hpp"

namespace Structure {
    template<typename Derived>
    char StaticStructure<Derived>::VALID_BIOMES[256];

    template<typename Derived>
    int StaticStructure<Derived>::SALT = 0;
    template<typename Derived>
    int StaticStructure<Derived>::REGION_SIZE = 0;
    template<typename Derived>
    int StaticStructure<Derived>::CHUNK_RANGE = 0;

    template<typename Derived>
    int StaticStructure<Derived>::CHUNK_BOUNDS = 0;

    template<typename Derived>
    void StaticStructure<Derived>::setupDerived(int salt, int regionSize, int spacing, const std::vector<int> &biomeList, WORLDSIZE worldSize) {
        Derived::SALT = salt;
        Derived::REGION_SIZE = regionSize;
        Derived::CHUNK_RANGE = regionSize - spacing;

        Derived::CHUNK_BOUNDS = getChunkWorldBounds(worldSize);

        if (!biomeList.empty() && !VALID_BIOMES[biomeList[0]])
            for (int i : biomeList)
                Derived::VALID_BIOMES[i] = 1;
    }

    template<typename Derived>
    Pos2D StaticStructure<Derived>::getRegionChunkPosition(int64_t worldSeed, int regionX, int regionZ) {
        uint64_t rng;
        setSeed(&rng, (int64_t) regionX * 341873128712ULL +
                          (int64_t) regionZ * 132897987541ULL + worldSeed + Derived::SALT);
        return {
            regionX * Derived::REGION_SIZE + nextInt(&rng, Derived::CHUNK_RANGE),
            regionZ * Derived::REGION_SIZE + nextInt(&rng, Derived::CHUNK_RANGE)
        };
    }

    template<typename Derived>
    inline Pos2D StaticStructure<Derived>::getRegionBlockPosition(int64_t worldSeed, int regionX, int regionZ) {
        return (getRegionChunkPosition(worldSeed, regionX, regionZ) << 4) + 8;
    }

    template<typename Derived>
    std::vector<Pos2D> StaticStructure<Derived>::getAllPositions(Generator* g) {
        Pos2D structPos;
        std::vector<Pos2D> positions;
        int numRegions = CHUNK_BOUNDS / REGION_SIZE;
        for (int regionX = -numRegions - 1; regionX <= numRegions; ++regionX) {
            for (int regionZ = -numRegions - 1; regionZ <= numRegions; ++regionZ) {
                structPos = getRegionChunkPosition(g->seed, regionX, regionZ);
                if (verifyChunkPosition(g, structPos)) {
                    positions.emplace_back((structPos << 4) + 8);
                }
            }
        }
        return positions;
    }

    template<typename Derived>
    bool StaticStructure<Derived>::verifyChunkPosition(Generator* g, int chunkX, int chunkZ) {
        if (chunkX < -CHUNK_BOUNDS || chunkX > CHUNK_BOUNDS || chunkZ < -CHUNK_BOUNDS || chunkZ > CHUNK_BOUNDS)
            return false;

        int id = g->getBiomeAt(1, (chunkX << 4) + 8, (chunkZ << 4) + 8);
        return VALID_BIOMES[id];
    }

    void Feature::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeList;
        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        setupDerived(14357617, useReducedSpacing ? 16 : 32, 8, biomeList, worldSize);
    }

    StructureType Feature::getFeatureType(Generator* g, int blockX, int blockZ) {
        if (blockX < -g->worldCoordinateBounds ||
            blockX > g->worldCoordinateBounds ||
            blockZ < -g->worldCoordinateBounds ||
            blockZ > g->worldCoordinateBounds) {
            return st_NONE;
        }
        switch (g->getBiomeAt(1, blockX, blockZ)) {
        case desert:
        case desert_hills:
            return st_Desert_Pyramid;
        case jungle:
        case jungle_hills:
        case bamboo_jungle:
        case bamboo_jungle_hills:
            return st_Jungle_Pyramid;
        case swamp:
            return st_Swamp_Hut;
        case snowy_tundra:
        case snowy_taiga:
            return st_Igloo;
        default:
            return st_NONE;
        }
    }

    std::vector<FeatureStructure> Feature::getAllFeaturePositions(Generator* g) {
        Pos2D structPos;
        std::vector<FeatureStructure> features;
        int numRegions = CHUNK_BOUNDS / REGION_SIZE;
        for (int regionX = -numRegions - 1; regionX <= numRegions; ++regionX) {
            for (int regionZ = -numRegions - 1; regionZ <= numRegions; ++regionZ) {
                structPos = getRegionBlockPosition(g->seed, regionX, regionZ);
                StructureType structureType = getFeatureType(g, structPos);
                if (structureType != st_NONE)
                    features.emplace_back(structPos, structureType);
            }
        }
        return features;
    }

    template <bool PS4Village>
    void Village<PS4Village>::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeList = {
            plains, desert, savanna, taiga, cold_taiga, ice_plains
        };
        bool useReducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        constexpr int spacing = (PS4Village ? 9 : 8);
        Village<PS4Village>::setupDerived(10387312, useReducedSpacing ? 16 : 32, spacing, biomeList, worldSize);
    }

    void OceanRuin::setup(WORLDSIZE worldSize) {
        std::vector<int> biomeList = {
            ocean, frozen_ocean, warm_ocean, lukewarm_ocean, cold_ocean, deep_ocean, deep_warm_ocean, deep_lukewarm_ocean, deep_cold_ocean, deep_frozen_ocean
        };
        setupDerived(14357617, 8, 2, biomeList, worldSize);
    }
}

template class Structure::StaticStructure<Structure::Feature>;
template class Structure::Village<false>;
template class Structure::Village<true>;
template class Structure::StaticStructure<Structure::Village<false>>;
template class Structure::StaticStructure<Structure::Village<true>>;
template class Structure::StaticStructure<Structure::OceanRuin>;

