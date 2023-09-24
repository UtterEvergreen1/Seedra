#include "NetherFortressStructure.hpp"

namespace Structure {
    Pos2D NetherFortressStructure::getWorldPosition(int64_t worldSeed){
        uint64_t rnd;
        setSeed(&rnd, worldSeed);
        next(&rnd, 32);
        int pos = nextInt(&rnd, 49);
        return { pos % 7, pos / 7 };
    }
}

