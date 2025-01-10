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
        ND MU static Pos2DVec_t getPositions(i64 worldSeed, int xLower, int zLower, int xUpper, int zUpper);

        /**
         * Overload function for getPositions to use a radius
         *
         * @param worldSeed the world seed
         * @param x, z the position to get the mineshafts from
         * @param radius the number of chunks to check around the target position
         * @return a vector of chunk positions for all the mineshafts within the radius
         */
        ND MU static Pos2DVec_t getPositions(c_i64 worldSeed, c_int x, c_int z, c_int radius) {
            return getPositions(worldSeed, x - radius, z - radius, x + radius, z + radius);
        }

        ND MU static Pos2DVec_t getAllPositions(const Generator& g) {
            return getPositions(g.getWorldSeed(),
                                -g.getWorldCoordinateBounds() + 1, -g.getWorldCoordinateBounds() + 1,
                                g.getWorldCoordinateBounds() - 1, g.getWorldCoordinateBounds() - 1);
        }
    };
} // namespace Placement
