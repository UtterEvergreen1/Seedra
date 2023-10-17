#include "mineshaft.hpp"

#include "LegacyCubiomes/utils/rng.hpp"

namespace Placement {

    std::vector<Pos2D> Mineshaft::getPositions(int64_t worldSeed, int xLower, int zLower, int xUpper, int zUpper) {
        RNG rng;
        Pos2DVec_t positions;
        rng.setSeed(worldSeed);
        uint64_t xModifier = rng.nextLong();
        uint64_t zModifier = rng.nextLong();

        for (int xPos = xLower; xPos <= xUpper; xPos++) {
            uint64_t aix = xPos * xModifier ^ worldSeed;

            for (int zPos = zLower; zPos <= zUpper; zPos++) {
                rng.setSeed(aix ^ zPos * zModifier);
                rng.advance();
                if EXPECT_FALSE(rng.nextDouble() < 0.004) {
                    int distance = xPos;
                    if (-xPos > distance) distance = -xPos;
                    if (+zPos > distance) distance = +zPos;
                    if (-zPos > distance) distance = -zPos;
                    if (distance >= 80 || rng.nextInt(80) < distance) {
                        positions.emplace_back(xPos, zPos);
                    }
                }
            }
        }

        return positions;
    }
}

