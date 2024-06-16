#pragma once

#include <vector>
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"


namespace Placement {
    template<class Derived>
    class StaticStructure {
    public:
        static c_u64 VALID_BIOMES;
        static c_int SALT;
        static int REGION_SIZE;
        static int CHUNK_RANGE;
        static int CHUNK_BOUNDS;
        static bool REDUCED_SPACING;

        static Pos2D getRegionChunkPosition(i64 worldSeed, int regionX, int regionZ);
        static Pos2D getRegionBlockPosition(i64 worldSeed, int regionX, int regionZ);

        static std::vector<Pos2D> getAllPositions(const Generator* g);

        /// do not use for feature, use getFeatureType instead
        static bool verifyChunkPosition(const Generator* g, int chunkX, int chunkZ);

        static bool verifyChunkPosition(const Generator* g, const Pos2D chunkPos) {
            return verifyChunkPosition(g, chunkPos.x, chunkPos.z);
        }

        static bool verifyBlockPosition(const Generator* g, c_int blockX, c_int blockZ) {
            return verifyChunkPosition(g, blockX >> 4, blockZ >> 4);
        }

        static bool verifyBlockPosition(const Generator* g, const Pos2D blockPos) {
            return verifyChunkPosition(g, blockPos.x >> 4, blockPos.z >> 4);
        }
    };

    /**
     * Represents a tuple of:
     * Position, and structure Type
     */
    class FeatureStructurePair {
    public:
        Pos2D pos;
        StructureType type;

        FeatureStructurePair(const Pos2D& pos, const StructureType type) : pos(pos), type(type) {}

        friend std::ostream& operator<<(std::ostream& out, const FeatureStructurePair& feature) {
            out << "Feature: " << feature.pos << " Type: " << getStructureName(feature.type);
            return out;
        }

#ifdef INCLUDE_QT
        friend QDebug operator<<(QDebug out, const Pos2D& pos) {
            out.nospace() << "(" << pos.x << ", " << pos.z << ")";
            return out.space();
        }
#endif
    };

    class Feature : public StaticStructure<Feature> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
        static StructureType getFeatureType(const Generator* g, int blockX, int blockZ);
        static StructureType getFeatureType(const Generator* g, const Pos2D& block) {
            return getFeatureType(g, block.x, block.z);
        }
        static std::vector<FeatureStructurePair> getAllFeaturePositions(const Generator* g);
    };

    template<bool isPS4Village>
    class Village : public StaticStructure<Village<isPS4Village>> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

    class OceanRuin : public StaticStructure<OceanRuin> {
    public:
        static void setWorldSize(lce::WORLDSIZE worldSize);
    };

} // namespace Placement
