#pragma once

#include <vector>

#include "LegacyCubiomes/cubiomes/biomeColor.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/Pos2D.hpp"

namespace Placement {
    template<class Derived>
    class StaticStructure {
    public:
        static const uint64_t VALID_BIOMES;
        static const int SALT;
        static int REGION_SIZE;
        static int CHUNK_RANGE;
        static int CHUNK_BOUNDS;
        static bool REDUCED_SPACING;

        static Pos2D getRegionChunkPosition(int64_t worldSeed, int regionX, int regionZ);
        static Pos2D getRegionBlockPosition(int64_t worldSeed, int regionX, int regionZ);

        static std::vector<Pos2D> getAllPositions(Generator* g);

        /// do not use for feature, use getFeatureType instead
        static bool verifyChunkPosition(Generator* g, int chunkX, int chunkZ);

        inline static bool verifyChunkPosition(Generator* g, Pos2D chunkPos) {
            return verifyChunkPosition(g, chunkPos.x, chunkPos.z);
        }

        inline static bool verifyBlockPosition(Generator* g, int blockX, int blockZ) {
            return verifyChunkPosition(g, blockX >> 4, blockZ >> 4);
        }

        inline static bool verifyBlockPosition(Generator* g, Pos2D blockPos) {
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

        FeatureStructurePair(const Pos2D& pos, StructureType type) : pos(pos), type(type) {}

        friend std::ostream& operator<<(std::ostream& out, const FeatureStructurePair& feature) {
            out << "Feature: " << feature.pos << " Type: " << getStructureName(feature.type);
            return out;
        }

#ifdef INCLUDE_QT
        friend QDebug operator<<(QDebug out, const Pos2D &pos) {
            out.nospace() << "(" << pos.x << ", " << pos.z << ")";
            return out.space();
        }
#endif
    };

    class Feature : public StaticStructure<Feature> {
    public:
        static void setWorldSize(WORLDSIZE worldSize);
        static StructureType getFeatureType(Generator* g, int blockX, int blockZ);
        inline static StructureType getFeatureType(Generator* g, const Pos2D& block) {
            return getFeatureType(g, block.x, block.z);
        }
        static std::vector<FeatureStructurePair> getAllFeaturePositions(Generator* g);
    };

    template<bool isPS4Village>
    class Village : public StaticStructure<Village<isPS4Village>> {
    public:
        static void setWorldSize(WORLDSIZE worldSize);
    };

    class OceanRuin : public StaticStructure<OceanRuin> {
    public:
        static void setWorldSize(WORLDSIZE worldSize);
    };

}

