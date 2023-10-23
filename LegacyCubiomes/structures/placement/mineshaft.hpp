#pragma once

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include <vector>


namespace Placement {

    class Mineshaft {
    public:
        /**
         * Gets all mineshafts within a range
         *
         * @param worldSeed the world seed
         * @param xLower, zLower, xUpper, zUpper the range to get the mineshafts from
         * @return a vector of chunk positions for all the mineshafts within the radius
         */
        ND MU static Pos2DVec_t getPositions(int64_t worldSeed, int xLower, int zLower, int xUpper, int zUpper);

        /**
         * Overload function for getPositions to use a radius
         *
         * @param worldSeed the world seed
         * @param x, z the position to get the mineshafts from
         * @param radius the number of chunks to check around the target position
         * @return a vector of chunk positions for all the mineshafts within the radius
         */
        ND MU inline static Pos2DVec_t getPositions(int64_t worldSeed, int x, int z, int radius) {
            return getPositions(worldSeed, x - radius, z - radius, x + radius, z + radius);
        }
    };
} // namespace Placement
