#include "netherfortress.hpp"

namespace Placement {
    Pos2D NetherFortress::getWorldPosition(int64_t worldSeed) {
        RNG rng;
        rng.setSeed(worldSeed);
        rng.next(32);
        int pos = rng.nextInt(49);
        return { pos % 7, pos / 7 };
    }
}

