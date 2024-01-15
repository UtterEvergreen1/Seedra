#include "StaticStructures.hpp"

#include "LegacyCubiomes/cubiomes/biomeID.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

namespace Placement {

    // #######################################################
    //              StaticStructure<Derived>
    // #######################################################

    // defaults
    template<typename Derived>
    const uint64_t StaticStructure<Derived>::VALID_BIOMES = 0;
    template<typename Derived>
    int StaticStructure<Derived>::REGION_SIZE = 16;
    template<typename Derived>
    int StaticStructure<Derived>::CHUNK_RANGE = 8;
    template<typename Derived>
    int StaticStructure<Derived>::CHUNK_BOUNDS = 27;
    template<typename Derived>
    bool StaticStructure<Derived>::REDUCED_SPACING = true;

    template<typename Derived>
    Pos2D StaticStructure<Derived>::getRegionChunkPosition(int64_t worldSeed, int regionX, int regionZ) {
        RNG rng;
        rng.setSeed((int64_t) regionX * 341873128712ULL + (int64_t) regionZ * 132897987541ULL + worldSeed +
                    Derived::SALT);
        return {regionX * REGION_SIZE + rng.nextInt(CHUNK_RANGE),
                regionZ * REGION_SIZE + rng.nextInt(CHUNK_RANGE)};
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
                structPos = getRegionChunkPosition(g->getWorldSeed(), regionX, regionZ);
                if (verifyChunkPosition(g, structPos)) { positions.emplace_back((structPos << 4) + 8); }
            }
        }
        return positions;
    }

    template<typename Derived>
    bool StaticStructure<Derived>::verifyChunkPosition(Generator* g, int chunkX, int chunkZ) {
        if (chunkX < -CHUNK_BOUNDS || chunkX > CHUNK_BOUNDS || chunkZ < -CHUNK_BOUNDS || chunkZ > CHUNK_BOUNDS)
            return false;

        return Generator::id_matches(g->getBiomeAt(1, (chunkX << 4) + 8, (chunkZ << 4) + 8), VALID_BIOMES);
    }


    // #######################################################
    //              StaticStructure<Feature>
    // #######################################################


    template<>
    const int StaticStructure<Feature>::SALT = 14357617;

    void Feature::setWorldSize(WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize);
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REGION_SIZE = reducedSpacing ? 16 : 32;
        CHUNK_RANGE = REGION_SIZE - 8;
    }

    StructureType Feature::getFeatureType(Generator* g, int blockX, int blockZ) {
        if (blockX < -g->getWorldCoordinateBounds() || blockX > g->getWorldCoordinateBounds() ||
            blockZ < -g->getWorldCoordinateBounds() || blockZ > g->getWorldCoordinateBounds()) {
            return StructureType::NONE;
        }
        switch (g->getBiomeAt(1, blockX, blockZ)) {
            case desert:
            case desert_hills:
                return StructureType::DesertPyramid;
            case jungle:
            case jungle_hills:
            case bamboo_jungle:
            case bamboo_jungle_hills:
                return StructureType::JunglePyramid;
            case swamp:
                return StructureType::SwampHut;
            case snowy_tundra:
            case snowy_taiga:
                return StructureType::Igloo;
            default:
                return StructureType::NONE;
        }
    }

    /**
     * Returns a list of locations that a feature CAN spawn in.
     * This calls generator.biomeAt() !!!
     *
     * @param g the generator
     * @return a vector of position + type.
     */
    std::vector<FeatureStructurePair> Feature::getAllFeaturePositions(Generator* g) {
        Pos2D structPos;
        std::vector<FeatureStructurePair> features;
        int numRegions = CHUNK_BOUNDS / REGION_SIZE;
        for (int regionX = -numRegions - 1; regionX <= numRegions; ++regionX) {
            for (int regionZ = -numRegions - 1; regionZ <= numRegions; ++regionZ) {
                structPos = getRegionBlockPosition(g->getWorldSeed(), regionX, regionZ);
                StructureType structureType = getFeatureType(g, structPos);
                if (structureType != StructureType::NONE) features.emplace_back(structPos, structureType);
            }
        }
        return features;
    }

    // #######################################################
    //             StaticStructure<Village<bool>>
    // #######################################################


    template<>
    const int StaticStructure<Village<false>>::SALT = 10387312;
    template<>
    const uint64_t StaticStructure<Village<false>>::VALID_BIOMES =
            (1ULL << plains) | (1ULL << desert) | (1ULL << taiga) | (1ULL << ice_plains) | (1ULL << cold_taiga) |
            (1ULL << savanna);

    template<>
    const int StaticStructure<Village<true>>::SALT = 10387312;
    template<>
    int StaticStructure<Village<true>>::CHUNK_RANGE = 7;
    template<>
    const uint64_t StaticStructure<Village<true>>::VALID_BIOMES =
            (1ULL << plains) | (1ULL << desert) | (1ULL << taiga) | (1ULL << ice_plains) | (1ULL << cold_taiga) |
            (1ULL << savanna);

    template<bool PS4Village>
    void Village<PS4Village>::setWorldSize(WORLDSIZE worldSize) {
        Village<PS4Village>::CHUNK_BOUNDS = getChunkWorldBounds(worldSize);
        // prevent from setting the same values
        bool reducedSpacing = worldSize < WORLDSIZE::MEDIUM;
        if (Village<PS4Village>::REDUCED_SPACING == reducedSpacing) return;
        Village<PS4Village>::REGION_SIZE = reducedSpacing ? 16 : 32;
        Village<PS4Village>::CHUNK_RANGE = Village<PS4Village>::REGION_SIZE - 8;
    }

    // #######################################################
    //             StaticStructure<OceanRuin>
    // #######################################################

    template<>
    const int StaticStructure<OceanRuin>::SALT = 14357617;
    template<>
    int StaticStructure<OceanRuin>::REGION_SIZE = 8;
    template<>
    int StaticStructure<OceanRuin>::CHUNK_RANGE = 6;
    template<>
    const uint64_t StaticStructure<OceanRuin>::VALID_BIOMES =
            (1ULL << ocean) | (1ULL << deep_ocean) | (1ULL << warm_ocean) | (1ULL << deep_warm_ocean) |
            (1ULL << lukewarm_ocean) | (1ULL << deep_lukewarm_ocean) | (1ULL << cold_ocean) |
            (1ULL << deep_cold_ocean) | (1ULL << frozen_ocean) | (1ULL << deep_frozen_ocean);
    void OceanRuin::setWorldSize(WORLDSIZE worldSize) { CHUNK_BOUNDS = getChunkWorldBounds(worldSize); }


} // namespace Placement

template class Placement::StaticStructure<Placement::Feature>;

template class Placement::Village<false>;
template class Placement::Village<true>;
template class Placement::StaticStructure<Placement::Village<false>>;
template class Placement::StaticStructure<Placement::Village<true>>;

template class Placement::StaticStructure<Placement::OceanRuin>;
