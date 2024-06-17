#include "mineshaft.hpp"

#include "LegacyCubiomes/utils/rng.hpp"

namespace Placement {

    Pos2DVec_t Mineshaft::getPositions(c_i64 worldSeed, c_int xLower, c_int zLower, c_int xUpper, c_int zUpper) {
        RNG rng;
        Pos2DVec_t positions;
        rng.setSeed(worldSeed);
        c_u64 xModifier = rng.nextLong();
        c_u64 zModifier = rng.nextLong();

        for (int xPos = xLower; xPos <= xUpper; xPos++) {
            c_u64 aix = xPos * xModifier ^ worldSeed;

            for (int zPos = zLower; zPos <= zUpper; zPos++) {
                rng.setSeed(aix ^ zPos * zModifier);
                rng.advance();
                if EXPECT_FALSE (rng.nextDouble() < 0.004) {
                    int distance = xPos;
                    if (-xPos > distance) distance = -xPos;
                    if (+zPos > distance) distance = +zPos;
                    if (-zPos > distance) distance = -zPos;
                    if (distance >= 80 || rng.nextInt(80) < distance) { positions.emplace_back(xPos, zPos); }
                }
            }
        }

        return positions;
    }
} // namespace Placement
