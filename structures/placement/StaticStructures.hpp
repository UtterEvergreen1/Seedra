#pragma once

#include "common/Pos2DTemplate.hpp"
#include "terrain/generator.hpp"
#include <vector>

namespace Placement {
    /**
     * @class StaticStructure
     * @brief Template class for static structure placement logic.
     *
     * This class provides methods for determining valid positions for static structures
     * and verifying their placement in the world.
     *
     * @tparam Derived The derived structure type.
     */
    template<class Derived>
    class StaticStructure {
    public:
        static c_u64 VALID_BIOMES; ///< Bitmask of valid biomes for the structure.
        static c_int SALT; ///< Salt value for structure placement.
        MU static int REGION_SIZE; ///< Size of the region for structure placement.
        MU static int CHUNK_RANGE; ///< Range of chunks to consider for placement.
        MU static int CHUNK_BOUNDS; ///< Bounds for chunk placement.
        MU static bool REDUCED_SPACING; ///< Flag for reduced spacing between structures.

        /**
         * @brief Gets the chunk position of the structure in a region.
         * @param worldSeed The seed of the world.
         * @param regionX The X coordinate of the region.
         * @param regionZ The Z coordinate of the region.
         * @return The chunk position of the structure.
         */
        static Pos2D getRegionChunkPosition(i64 worldSeed, int regionX, int regionZ);

        /**
         * @brief Gets the block position of the structure in a region.
         * @param worldSeed The seed of the world.
         * @param regionX The X coordinate of the region.
         * @param regionZ The Z coordinate of the region.
         * @return The block position of the structure.
         */
        static Pos2D getRegionBlockPosition(i64 worldSeed, int regionX, int regionZ);

        /**
         * @brief Gets all positions of the structure.
         * @param g Pointer to the generator.
         * @return A vector of all structure positions.
         */
        MU static std::vector<Pos2D> getAllPositions(const Generator *g);

        /**
         * @brief Gets all positions of the structure within bounds.
         * @param g Pointer to the generator.
         * @param lowerX Lower X bound.
         * @param lowerZ Lower Z bound.
         * @param upperX Upper X bound.
         * @param upperZ Upper Z bound.
         * @return A vector of all structure positions within bounds.
         */
        static std::vector<Pos2D>
        getAllPositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX, int upperZ);

        /**
         * @brief Verifies if the structure can spawn at a specific chunk position.
         * @param g Pointer to the generator.
         * @param chunkX The X coordinate of the chunk.
         * @param chunkZ The Z coordinate of the chunk.
         * @return True if the structure can spawn, false otherwise.
         */
        static bool verifyChunkPosition(const Generator *g, int chunkX, int chunkZ);

        /**
         * @brief Verifies if the structure can spawn at a specific chunk position.
         * @param g Pointer to the generator.
         * @param chunkPos The chunk position as a 2D coordinate.
         * @return True if the structure can spawn, false otherwise.
         */
        static bool verifyChunkPosition(const Generator *g, const Pos2D chunkPos) {
            return verifyChunkPosition(g, chunkPos.x, chunkPos.z);
        }

        /**
         * @brief Verifies if the structure can spawn at a specific block position.
         * @param g Pointer to the generator.
         * @param blockX The X coordinate of the block.
         * @param blockZ The Z coordinate of the block.
         * @return True if the structure can spawn, false otherwise.
         */
        MU static bool verifyBlockPosition(const Generator *g, c_int blockX, c_int blockZ) {
            return verifyChunkPosition(g, blockX >> 4, blockZ >> 4);
        }

        /**
         * @brief Verifies if the structure can spawn at a specific block position.
         * @param g Pointer to the generator.
         * @param blockPos The block position as a 2D coordinate.
         * @return True if the structure can spawn, false otherwise.
         */
        MU static bool verifyBlockPosition(const Generator *g, const Pos2D blockPos) {
            return verifyChunkPosition(g, blockPos.x >> 4, blockPos.z >> 4);
        }
    };

    /**
     * @class FeatureStructurePair
     * @brief Represents a tuple of position and structure type.
     */
    class FeatureStructurePair {
    public:
        Pos2D pos; ///< The position of the structure.
        StructureType type; ///< The type of the structure.

        /**
         * @brief Constructs a FeatureStructurePair.
         * @param pos The position of the structure.
         * @param type The type of the structure.
         */
        FeatureStructurePair(const Pos2D &pos, const StructureType type) : pos(pos), type(type) {
        }

        /**
         * @brief Outputs the FeatureStructurePair to a stream.
         * @param out The output stream.
         * @param feature The FeatureStructurePair to output.
         * @return The output stream.
         */
        friend std::ostream &operator<<(std::ostream &out, const FeatureStructurePair &feature) {
            out << "Feature: " << feature.pos << " Type: " << getStructureName(feature.type);
            return out;
        }

#ifdef INCLUDE_QT
                /**
                 * @brief Outputs the FeatureStructurePair to a QDebug stream.
                 * @param out The QDebug stream.
                 * @param pos_d The position to output.
                 * @return The QDebug stream.
                 */
                friend QDebug operator<<(QDebug out, const Pos2D &pos_d) {
                    out.nospace() << "(" << pos_d.x << ", " << pos_d.z << ")";
                    return out.space();
                }
#endif
    };

    /**
     * @class Feature
     * @brief Represents a feature structure.
     */
    class Feature : public StaticStructure<Feature> {
    public:
        /**
         * @brief Sets the world size for the feature.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);

        /**
         * @brief Gets the type of the feature at a specific block position.
         * @param g Pointer to the generator.
         * @param blockX The X coordinate of the block.
         * @param blockZ The Z coordinate of the block.
         * @return The type of the feature.
         */
        static StructureType getFeatureType(const Generator *g, int blockX, int blockZ);

        /**
         * @brief Gets the type of the feature at a specific block position.
         * @param g Pointer to the generator.
         * @param block The block position as a 2D coordinate.
         * @return The type of the feature.
         */
        static StructureType getFeatureType(const Generator *g, const Pos2D &block) {
            return getFeatureType(g, block.x, block.z);
        }

        /**
         * @brief Gets all feature positions.
         * @param g Pointer to the generator.
         * @return A vector of all feature positions.
         */
        static std::vector<FeatureStructurePair> getAllFeaturePositions(const Generator *g);

        /**
         * @brief Gets all feature positions separated by type.
         * @param g Pointer to the generator.
         * @return A vector of vectors of feature positions separated by type.
         */
        MU static std::vector<std::vector<Pos2D> > getAllFeaturePositionsSeparated(const Generator *g);

        /**
         * @brief Gets all feature positions within bounds.
         * @param g Pointer to the generator.
         * @param lowerX Lower X bound.
         * @param lowerZ Lower Z bound.
         * @param upperX Upper X bound.
         * @param upperZ Upper Z bound.
         * @return A vector of all feature positions within bounds.
         */
        static std::vector<FeatureStructurePair>
        getAllFeaturePositionsBounded(const Generator *g, int lowerX, int lowerZ, int upperX, int upperZ);
    };

    /**
     * @class Village
     * @brief Represents a Village structure.
     * @tparam isPS4Village Whether the village is a PS4-specific variant.
     */
    template<bool isPS4Village>
    class Village : public StaticStructure<Village<isPS4Village> > {
    public:
        /**
         * @brief Sets the world size for the Village.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    /**
     * @class OceanRuin
     * @brief Represents an Ocean Ruin structure.
     */
    class OceanRuin : public StaticStructure<OceanRuin> {
    public:
        /**
         * @brief Sets the world size for the Ocean Ruin.
         * @param worldSize The world size.
         */
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };
} // namespace Placement
