#include "netherfortress.hpp"

namespace Placement {
    Pos2D NetherFortress::getWorldPosition(const int64_t worldSeed) {
        RNG rng;
        rng.setSeed(worldSeed);
        rng.next(32);
        const int pos = rng.nextInt(49);
        return {pos % 7, pos / 7};
    }
} // namespace Placement
