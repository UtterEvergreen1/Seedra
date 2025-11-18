#include "mineshaft.hpp"

#include "common/rng.hpp"

namespace Placement {

    Pos2DVec_t Mineshaft::getPositions(c_i64 worldSeed, int xLower, int zLower, int xUpper, int zUpper) {
        RNG rng;
        Pos2DVec_t positions;
        rng.setSeed(static_cast<u64>(worldSeed));
        xLower >>= 4;
        zLower >>= 4;
        xUpper >>= 4;
        zUpper >>= 4;
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
                    if (distance >= 80 || rng.nextInt(80) < distance) {
                        positions.emplace_back((xPos << 4) + 8, (zPos << 4) + 8);
                    }
                }
            }
        }
        return positions;
    }
} // namespace Placement
