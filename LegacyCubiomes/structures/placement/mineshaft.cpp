#include "mineshaft.hpp"

#include "LegacyCubiomes/utils/rng.hpp"

namespace Placement {

    std::vector<Pos2D> Mineshaft::getPositions(const int64_t worldSeed, const int xLower, const int zLower,
                                               const int xUpper, const int zUpper) {
        RNG rng;
        Pos2DVec_t positions;
        rng.setSeed(worldSeed);
        const uint64_t xModifier = rng.nextLong();
        const uint64_t zModifier = rng.nextLong();

        for (int xPos = xLower; xPos <= xUpper; xPos++) {
            const uint64_t aix = xPos * xModifier ^ worldSeed;

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
