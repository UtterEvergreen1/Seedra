#pragma once
#include "LegacyCubiomes/utils/pos2D.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/cubiomes/util.hpp"
namespace Structure {
    template<class Derived>
    class StaticStructure {
    public:
        static char VALID_BIOMES[256];

        static int SALT;
        static int REGION_SIZE;
        static int CHUNK_RANGE;

        static int CHUNK_BOUNDS;

        static void setupDerived(int salt, int regionSize, int spacing, const std::vector<int>& biomeList, WORLDSIZE worldSize);
        static Pos2D getRegionChunkPosition(int64_t worldSeed, int regionX, int regionZ);
        static Pos2D getRegionBlockPosition(int64_t worldSeed, int regionX, int regionZ);

        static std::vector<Pos2D> getAllPositions(Generator* g);

        /*do not use for feature, use getFeatureType instead*/
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

    class FeatureStructure {
    public:
        Pos2D pos;
        StructureType type;
        FeatureStructure(const Pos2D& position, StructureType structureType)
            : pos(position), type(structureType) {}

        friend std::ostream& operator<<(std::ostream& out, const FeatureStructure& feature) {
            out << "Feature: " << feature.pos << " Type: " << getStructureName(feature.type);
            return out;
        }

        /*
        friend QDebug operator<<(QDebug out, const Pos2D &pos) {
            out.nospace() << "(" << pos.x << ", " << pos.z << ")";
            return out.space();
        }
        */
    };

    class Feature : public StaticStructure<Feature> {
    public:
        static void setup(WORLDSIZE worldSize);
        static StructureType getFeatureType(Generator* g, int blockX, int blockZ);
        inline static StructureType getFeatureType(Generator* g, const Pos2D& block) {
            return getFeatureType(g, block.x, block.z);
        }
        static std::vector<FeatureStructure> getAllFeaturePositions(Generator* g);
    };

    class OceanRuin : public StaticStructure<OceanRuin> {
    public:
        static void setup(WORLDSIZE worldSize);
    };

    class Village : public StaticStructure<Village> {
    public:
        static void setup(WORLDSIZE worldSize);
    };
}

