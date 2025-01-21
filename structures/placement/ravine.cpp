#include "ravine.hpp"

namespace Placement {

    Pos2DVec_t Ravine::getPositions(c_i64 worldSeed, int xLower, int zLower, int xUpper, int zUpper) {
        RNG rng;
        Pos2DVec_t positions;
        rng.setSeed(worldSeed);
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
                if EXPECT_FALSE (rng.nextInt(50) == 0) {
                    auto tunnelStartX = (double) (xPos * 16 + rng.nextInt(16));
                    MU auto tunnelStartY = (double) (rng.nextInt(rng.nextInt(40) + 8) + 20);
                    auto tunnelStartZ = (double) (zPos * 16 + rng.nextInt(16));
                    positions.emplace_back(tunnelStartX, tunnelStartZ);
                }
            }
        }
        return positions;
    }
} // namespace Placement