#pragma once
#include "LegacyCubiomes/utils/pos2D.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include <map>
namespace Structure {
    static constexpr size_t ARRAY_SIZE = 256;

    template<class Derived>
    class StaticStructure {
    public:
        static char VALID_BIOMES[ARRAY_SIZE];

        static int SALT;
        static int REGION_SIZE;
        static int CHUNK_RANGE;

        static int CHUNK_BOUNDS;

        static void setupDerived(int salt, int regionSize, int spacing, const std::vector<int>& biomeList, WORLDSIZE worldSize);
        static Pos2D getRegionChunkPosition(int64_t worldSeed, int regionX, int regionZ);
        static Pos2D getRegionBlockPosition(int64_t worldSeed, int regionX, int regionZ);

        static std::vector<Pos2D> getAllPositions(int64_t worldSeed);

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

    class Feature : public StaticStructure<Feature> {
    public:
        static void setup(WORLDSIZE worldSize);
        static StructureType getFeatureType(Generator* g, int blockX, int blockZ);
        inline static StructureType getFeatureType(Generator* g, const Pos2D& block) {
            return getFeatureType(g, block.x, block.z);
        }
        static std::map<Pos2D, StructureType, Pos2D::Hasher> getAllFeaturePositions(Generator* g);
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

