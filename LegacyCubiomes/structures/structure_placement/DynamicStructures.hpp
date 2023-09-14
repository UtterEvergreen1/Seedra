#pragma once

#include <vector>
#include <iterator>
#include <unordered_set>

#include "LegacyCubiomes/utils/pos2D.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"

namespace Structure {
    template <typename Derived>
    class DynamicStructure {
    public:
        static char MAIN_VALID_BIOMES[256];
        static char SECOND_VALID_BIOMES[256];
        
        static int SALT;
        static int REGION_SIZE;
        static int CHUNK_RANGE;
        static int MAX_ATTEMPT;
        static int MAIN_RADIUS;
        static int SECOND_RADIUS;

        static bool HAS_SECOND_BIOME_CHECK;

        static bool HAS_MAX_ATTEMPTS;

        static int CHUNK_BOUNDS;

        static void setupDerived(int salt, int regionSize, int spacing, int attempts,
                                 int mainRadius, int secondRadius,
                                 const std::vector<int>& biomeListMain,
                                 const std::vector<int>& biomeListSecond, WORLDSIZE worldSize);
        static Pos2D getPosition(Generator* g, int regionX, int regionZ);
        static std::vector<Pos2D> getAllPositions(Generator* g);
        static bool canSpawnAtChunk(int64_t worldSeed, int chunkX, int chunkZ, int regionX, int regionZ);
    };

    class Mansion : public DynamicStructure<Mansion> {
    public:
        static void setup(WORLDSIZE worldSize);
    };
    class Monument : public DynamicStructure<Monument> {
    public:
        static void setup(WORLDSIZE worldSize);
    };
    class Treasure : public DynamicStructure<Treasure> {
    public:
        static void setup(WORLDSIZE worldSize);
    };
    class Shipwreck : public DynamicStructure<Shipwreck> {
    public:
        static void setup(WORLDSIZE worldSize);
    };
    class Outpost : public DynamicStructure<Outpost> {
    public:
        static void setup(WORLDSIZE worldSize);
    };
}

