#pragma once

#include "LegacyCubiomes/cubiomes/generator.hpp"

namespace Structure {
    class StrongholdStructure {
    public:
        static char VALID_BIOMES[256];

        static void setup();
        static Pos2D getWorldPosition(Generator* g);
        static inline Pos2D getStartCenter(Generator* g) {return getWorldPosition(g) - 4;}
        static inline Pos2D getStartChunk(Generator* g) {return getWorldPosition(g) >> 4;}
        static Pos2D getRawWorldPosition(int64_t worldSeed, bool xboxStronghold);
    };
}
