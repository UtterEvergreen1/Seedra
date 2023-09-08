#pragma once
#include "LegacyCubiomes/cubiomes/generator.hpp"
namespace Structure {
    static constexpr size_t ARRAY_SIZE = 256;

    class StrongholdStructure {
    public:
        static char VALID_BIOMES[ARRAY_SIZE];

        static void setup();
        static Pos2D getWorldPosition(Generator* g);
        static Pos2D getRawWorldPosition(int64_t worldSeed, bool xboxStronghold);
    };
}
