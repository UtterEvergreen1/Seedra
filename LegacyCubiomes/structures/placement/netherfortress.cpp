#include "netherfortress.hpp"

namespace Placement {


    Pos2D NetherFortress::getWorldPosition(c_i64 worldSeed) {
        RNG rng;
        rng.setSeed(worldSeed);
        rng.next(32);
        c_int pos = rng.nextInt(49);
        return {pos % 7, pos / 7};
    }


}
