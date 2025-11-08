#include "DynamicStructures.hpp"

#include <unordered_set>


namespace Placement {

    // defaults
    template<typename Derived>
    int DynamicStructure<Derived>::REGION_SIZE = 32;
    template<typename Derived>
    int DynamicStructure<Derived>::CHUNK_RANGE = 16;
    template<typename Derived>
    MU int DynamicStructure<Derived>::ATTEMPTS = 60;
    template<typename Derived>
    MU int DynamicStructure<Derived>::CHUNK_BOUNDS = 24;
    template<typename Derived>
    bool DynamicStructure<Derived>::REDUCED_SPACING = true;

    template<typename Derived>
    Pos2D DynamicStructure<Derived>::getPosition(const Generator *g, c_int regionX, c_int regionZ) {
        RNG rnds;
        c_i64 featureSeed = static_cast<i64>(regionX * Derived::REGION_SIZE) * 341873128712ULL +
                            static_cast<i64>(regionZ * Derived::REGION_SIZE) * 132897987541ULL + g->getWorldSeed() + Derived::SALT;
        rnds.setSeed(featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        for (int attempts = 0; attempts < Derived::ATTEMPTS; attempts++) {
            int xChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            int zChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) {
                // successfully placed
                xChunk = regionX * Derived::REGION_SIZE + xChunk;
                zChunk = regionZ * Derived::REGION_SIZE + zChunk;
                int xPos = (xChunk << 4) + 8;
                int zPos = (zChunk << 4) + 8;
                if (xChunk < Derived::CHUNK_BOUNDS && xChunk >= -Derived::CHUNK_BOUNDS
                    && zChunk < Derived::CHUNK_BOUNDS && zChunk >= -Derived::CHUNK_BOUNDS) {

                    if (Derived::verifyBlockPosition(g, xPos, zPos)) {
                        return {xPos, zPos};
                    }
                }
            } else {
                attempts--;
            }
        }
        return {0, 0};
    }

    template<typename Derived>
    Pos2DVec_t DynamicStructure<Derived>::
    getAllPossibleChunks(int64_t worldSeed, int regionX, int regionZ) {
        RNG rnds;
        c_i64 featureSeed = static_cast<i64>(regionX * Derived::REGION_SIZE) * 341873128712ULL +
                            static_cast<i64>(regionZ * Derived::REGION_SIZE) * 132897987541ULL + worldSeed + Derived::SALT;
        rnds.setSeed(featureSeed);
        Pos2DVec_t positions;
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        positions.reserve(Derived::ATTEMPTS);
        attempted.reserve(Derived::ATTEMPTS);
        for (int attempts = 0; attempts < Derived::ATTEMPTS; attempts++) {
            int xChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            int zChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) {
                // successfully placed
                xChunk = regionX * Derived::REGION_SIZE + xChunk;
                zChunk = regionZ * Derived::REGION_SIZE + zChunk;
                if (xChunk < Derived::CHUNK_BOUNDS && xChunk >= -Derived::CHUNK_BOUNDS &&
                    zChunk < Derived::CHUNK_BOUNDS && zChunk >= -Derived::CHUNK_BOUNDS) {
                    positions.emplace_back(xChunk, zChunk);
                }
            } else {
                attempts--;
            }
        }
        return positions;
    }

    template<typename Derived>
    bool DynamicStructure<Derived>::verifyBlockPosition(const Generator *g, int blockX, int blockZ) {
        if (g->areBiomesViable(blockX, blockZ, Derived::MAIN_RADIUS, Derived::MAIN_VALID_BIOMES)) {
            if constexpr (!Derived::HAS_SECOND_BIOME_CHECK) { return true; }
            if (g->areBiomesViable(blockX, blockZ, Derived::SECOND_RADIUS, Derived::SECONDARY_VALID_BIOMES,
                                   Derived::SECONDARY_VALID_BIOMES_MUTATED)) {
                return true;
            }
        }
        return false;
    }

    template<typename Derived>
    bool DynamicStructure<Derived>::isPossibleChunkPos(int64_t worldSeed, int regionX, int regionZ, const Pos2D &pos) {
        RNG rnds;
        c_i64 featureSeed = static_cast<i64>(regionX * Derived::REGION_SIZE) * 341873128712ULL +
                            static_cast<i64>(regionZ * Derived::REGION_SIZE) * 132897987541ULL + worldSeed + Derived::SALT;
        rnds.setSeed(featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        for (int attempts = 0; attempts < Derived::ATTEMPTS; attempts++) {
            int xChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            int zChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) {
                // successfully placed
                xChunk = regionX * Derived::REGION_SIZE + xChunk;
                zChunk = regionZ * Derived::REGION_SIZE + zChunk;
                if (xChunk < Derived::CHUNK_BOUNDS && xChunk >= -Derived::CHUNK_BOUNDS &&
                    zChunk < Derived::CHUNK_BOUNDS && zChunk >= -Derived::CHUNK_BOUNDS &&
                    xChunk == pos.x && zChunk == pos.z) {
                    return true;
                }
            } else {
                attempts--;
            }
        }
        return false;
    }


    template<typename Derived>
    std::vector<Pos2D> DynamicStructure<Derived>::getAllPositions(const Generator *g, std::atomic_bool *terminateFlag) {
        return getAllPositionsBounded(g, -g->getWorldCoordinateBounds(), -g->getWorldCoordinateBounds(),
                                      g->getWorldCoordinateBounds(), g->getWorldCoordinateBounds(), terminateFlag);
    }

    template<typename Derived>
    std::vector<Pos2D>
    DynamicStructure<Derived>::getAllPositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX,
                                                      int upperZ, const std::atomic_bool *terminateFlag) {
        std::vector<Pos2D> positions;
        c_int lowerXRegion = static_cast<const int>(std::floor(static_cast<float>(lowerX >> 4) / static_cast<float>(REGION_SIZE)));
        c_int lowerZRegion = static_cast<const int>(std::floor(static_cast<float>(lowerZ >> 4) / static_cast<float>(REGION_SIZE)));
        c_int upperXRegion = static_cast<const int>(std::floor(static_cast<float>(upperX >> 4) / static_cast<float>(REGION_SIZE)));
        c_int upperZRegion = static_cast<const int>(std::floor(static_cast<float>(upperZ >> 4) / static_cast<float>(REGION_SIZE)));
        for (int regionX = lowerXRegion; regionX <= upperXRegion; ++regionX) {
            for (int regionZ = lowerZRegion; regionZ <= upperZRegion; ++regionZ) {
                if (terminateFlag != nullptr && terminateFlag->load()) return positions;
                Pos2D structPos = getPosition(g, regionX, regionZ);
                if (structPos != 0 && structPos.insideBounds(lowerX, lowerZ, upperX, upperZ))
                    positions.push_back(structPos);
            }
        }
        return positions;
    }

    template<typename Derived>
    MU bool DynamicStructure<Derived>::canSpawnAtChunk(c_i64 worldSeed, c_int chunkX, c_int chunkZ, c_int regionX,
                                                       c_int regionZ) {
        RNG rnds;
        c_i64 featureSeed = static_cast<i64>(regionX * Derived::REGION_SIZE) * 341873128712ULL +
                            static_cast<i64>(regionZ * Derived::REGION_SIZE) * 132897987541ULL + worldSeed + Derived::SALT;
        rnds.setSeed(featureSeed);
        std::unordered_set<Pos2D, Pos2D::Hasher> attempted;
        for (int attempts = 0; attempts < Derived::ATTEMPTS; attempts++) {
            int xChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            int zChunk = rnds.nextInt(Derived::CHUNK_RANGE);
            if (attempted.emplace(xChunk, zChunk).second) {
                // successfully placed
                xChunk = regionX * Derived::REGION_SIZE + xChunk;
                zChunk = regionZ * Derived::REGION_SIZE + zChunk;
                if (chunkX == xChunk && zChunk == chunkZ) return true;
            } else {
                attempts--;
            }
        }
        return false;
    }

    template<>
    int DynamicStructure<Mansion>::CHUNK_RANGE = 26;
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
    int DynamicStructure<Monument>::CHUNK_RANGE = 27;
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
    int DynamicStructure<BuriedTreasure>::CHUNK_RANGE = 30;
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
    int DynamicStructure<Shipwreck>::CHUNK_RANGE = 27;
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
    int DynamicStructure<Outpost>::CHUNK_RANGE = 26;
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
