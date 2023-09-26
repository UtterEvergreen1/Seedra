#include "MineshaftStructure.hpp"
namespace Structure {
    std::vector<Pos2D> MineshaftStructure::getPositions(int64_t worldSeed, int xLower, int zLower, int xUpper, int zUpper) {
        uint64_t random;
        std::vector<Pos2D> positions;
        setSeed(&random, worldSeed);
        uint64_t xModifier = nextLong(&random);
        uint64_t zModifier = nextLong(&random);

        for (int xPos = xLower; xPos <= xUpper; xPos++) {
            uint64_t aix = xPos * xModifier ^ worldSeed;

            for (int zPos = zLower; zPos <= zUpper; zPos++) {
                setSeed(&random, aix ^ zPos * zModifier);
                random = (random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
                if EXPECT_FALSE(nextDouble(&random) < 0.004) {
                    int distance = xPos;
                    if (-xPos > distance) distance = -xPos;
                    if (+zPos > distance) distance = +zPos;
                    if (-zPos > distance) distance = -zPos;
                    if (distance >= 80 || nextInt(&random, 80) < distance) {
                        positions.emplace_back(xPos, zPos);
                    }
                }
            }
        }

        return positions;
    }
}

