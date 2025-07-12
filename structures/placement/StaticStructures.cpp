#include "StaticStructures.hpp"

#include "common/rng.hpp"
#include "terrain/biomes/biomeID.hpp"


namespace Placement {

    // #######################################################
    //              StaticStructure<Derived>
    // #######################################################

    // defaults
    template<typename Derived>
    c_u64 StaticStructure<Derived>::VALID_BIOMES = 0;
    template<typename Derived>
    int StaticStructure<Derived>::REGION_SIZE = 16;
    template<typename Derived>
    int StaticStructure<Derived>::CHUNK_RANGE = 8;
    template<typename Derived>
    int StaticStructure<Derived>::CHUNK_BOUNDS = 27;
    template<typename Derived>
    bool StaticStructure<Derived>::REDUCED_SPACING = true;

    template<typename Derived>
    Pos2D StaticStructure<Derived>::getRegionChunkPosition(c_i64 worldSeed, c_int regionX, c_int regionZ) {
        RNG rng;
        rng.setSeed((i64) regionX * 341873128712ULL + (i64) regionZ * 132897987541ULL + worldSeed +
                    Derived::SALT);
        return {regionX * REGION_SIZE + rng.nextInt(CHUNK_RANGE), regionZ * REGION_SIZE + rng.nextInt(CHUNK_RANGE)};
    }

    template<typename Derived>
    Pos2D StaticStructure<Derived>::getRegionBlockPosition(c_i64 worldSeed, c_int regionX, c_int regionZ) {
        return (getRegionChunkPosition(worldSeed, regionX, regionZ) << 4) + 8;
    }

    template<typename Derived>
    std::vector<Pos2D> StaticStructure<Derived>::getAllPositions(const Generator *g) {
        return getAllPositionsBounded(g, -g->getWorldCoordinateBounds(), -g->getWorldCoordinateBounds(),
                                      g->getWorldCoordinateBounds(), g->getWorldCoordinateBounds());
    }

    template<typename Derived>
    std::vector<Pos2D>
    StaticStructure<Derived>::getAllPositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX,
                                                     int upperZ) {
        std::vector<Pos2D> positions;
        c_int lowerXRegion = std::floor((float)(lowerX >> 4) / REGION_SIZE);
        c_int lowerZRegion = std::floor((float)(lowerZ >> 4) / REGION_SIZE);
        c_int upperXRegion = std::floor((float)(upperX >> 4) / REGION_SIZE);
        c_int upperZRegion = std::floor((float)(upperZ >> 4) / REGION_SIZE);
        for (int regionX = lowerXRegion; regionX <= upperXRegion; ++regionX) {
            for (int regionZ = lowerZRegion; regionZ <= upperZRegion; ++regionZ) {
                if (const Pos2D structPos = getRegionBlockPosition(g->getWorldSeed(), regionX, regionZ);
                        verifyChunkPosition(g, structPos.toChunkPos()) && structPos.insideBounds(lowerX, lowerZ, upperX, upperZ))
                    positions.push_back(structPos);
            }
        }
        return positions;
    }

    template<typename Derived>
    bool StaticStructure<Derived>::verifyChunkPosition(const Generator *g, c_int chunkX, c_int chunkZ) {
        if (chunkX < -CHUNK_BOUNDS || chunkX > CHUNK_BOUNDS || chunkZ < -CHUNK_BOUNDS || chunkZ > CHUNK_BOUNDS)
            return false;

        return Generator::id_matches(g->getBiomeIdAt(1, (chunkX << 4) + 8, (chunkZ << 4) + 8), VALID_BIOMES);
    }


    // #######################################################
    //              StaticStructure<Feature>
    // #######################################################


    template<>
    MU c_int StaticStructure<Feature>::SALT = 14357617;

    void Feature::setWorldSize(const lce::WORLDSIZE worldSize) {
        CHUNK_BOUNDS = getChunkWorldBounds(worldSize);
        // prevent from setting the same values
        c_bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (REDUCED_SPACING == reducedSpacing) return;
        REDUCED_SPACING = reducedSpacing;
        REGION_SIZE = reducedSpacing ? 16 : 32;
        CHUNK_RANGE = REGION_SIZE - 8;
    }

    StructureType Feature::getFeatureType(const Generator *g, c_int blockX, c_int blockZ) {
        if (blockX < -g->getWorldCoordinateBounds() || blockX > g->getWorldCoordinateBounds() ||
            blockZ < -g->getWorldCoordinateBounds() || blockZ > g->getWorldCoordinateBounds()) {
            return StructureType::NONE;
        }
        switch (g->getBiomeIdAt(1, blockX, blockZ)) {
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
    std::vector<FeatureStructurePair> Feature::getAllFeaturePositions(const Generator *g) {
        return getAllFeaturePositionsBounded(g, -g->getWorldCoordinateBounds(), -g->getWorldCoordinateBounds(),
                                             g->getWorldCoordinateBounds(), g->getWorldCoordinateBounds());
    }


    MU std::vector<std::vector<Pos2D>> Feature::getAllFeaturePositionsSeparated(const Generator *g) {
        auto features = getAllFeaturePositions(g);
        std::vector<std::vector<Pos2D>> separatedFeatures((size_t) StructureType::FEATURE_NUM);
        for (const auto &feature : features) {
            separatedFeatures[(size_t) feature.type].push_back(feature.pos);
        }
        return separatedFeatures;
    }

    std::vector<FeatureStructurePair>
    Feature::getAllFeaturePositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX, int upperZ) {
        std::vector<FeatureStructurePair> features;
        c_int numXRegions = ((upperX - lowerX) >> 4) / REGION_SIZE - 1;
        c_int numZRegions = ((upperZ - lowerZ) >> 4) / REGION_SIZE - 1;
        for (int regionX = -numXRegions; regionX < numXRegions; ++regionX) {
            for (int regionZ = -numZRegions; regionZ < numZRegions; ++regionZ) {
                Pos2D structPos = getRegionBlockPosition(g->getWorldSeed(), regionX, regionZ);
                if (StructureType structureType = getFeatureType(g, structPos); structureType != StructureType::NONE &&
                                                                                structPos.insideBounds(lowerX, lowerZ,
                                                                                                       upperX, upperZ))
                    features.emplace_back(structPos, structureType);
            }
        }
        return features;
    }

    // #######################################################
    //             StaticStructure<Village<bool>>
    // #######################################################


    template<>
    MU c_int StaticStructure<Village<false>>::SALT = 10387312;
    template<>
    c_u64 StaticStructure<Village<false>>::VALID_BIOMES =
            1ULL << plains | 1ULL << desert | 1ULL << taiga | 1ULL << ice_plains | 1ULL << cold_taiga | 1ULL << savanna;

    template<>
    MU c_int StaticStructure<Village<true>>::SALT = 10387312;
    template<>
    MU int StaticStructure<Village<true>>::CHUNK_RANGE = 7;
    template<>
    c_u64 StaticStructure<Village<true>>::VALID_BIOMES =
            1ULL << plains | 1ULL << desert | 1ULL << taiga | 1ULL << ice_plains | 1ULL << cold_taiga | 1ULL << savanna;

    template<bool PS4Village>
    void Village<PS4Village>::setWorldSize(const lce::WORLDSIZE worldSize) {
        Village::CHUNK_BOUNDS = getChunkWorldBounds(worldSize);
        // prevent from setting the same values
        bool reducedSpacing = worldSize < lce::WORLDSIZE::MEDIUM;
        if (Village::REDUCED_SPACING == reducedSpacing) return;
        Village::REDUCED_SPACING = reducedSpacing;
        Village::REGION_SIZE = reducedSpacing ? 16 : 32;
        Village::CHUNK_RANGE = Village::REGION_SIZE - 8;
    }

    // #######################################################
    //             StaticStructure<OceanRuin>
    // #######################################################

    template<>
    MU c_int StaticStructure<OceanRuin>::SALT = 14357617;
    template<>
    MU int StaticStructure<OceanRuin>::REGION_SIZE = 8;
    template<>
    MU int StaticStructure<OceanRuin>::CHUNK_RANGE = 6;
    template<>
    c_u64 StaticStructure<OceanRuin>::VALID_BIOMES =
            1ULL << ocean | 1ULL << deep_ocean | 1ULL << warm_ocean | 1ULL << deep_warm_ocean |
            1ULL << lukewarm_ocean | 1ULL << deep_lukewarm_ocean | 1ULL << cold_ocean |
            1ULL << deep_cold_ocean | 1ULL << frozen_ocean | 1ULL << deep_frozen_ocean;

    void OceanRuin::setWorldSize(const lce::WORLDSIZE worldSize) { CHUNK_BOUNDS = getChunkWorldBounds(worldSize); }


} // namespace Placement

template
class Placement::StaticStructure<Placement::Feature>;

template
class Placement::Village<false>;

template
class Placement::Village<true>;

template
class Placement::StaticStructure<Placement::Village<false>>;

template
class Placement::StaticStructure<Placement::Village<true>>;

template
class Placement::StaticStructure<Placement::OceanRuin>;
