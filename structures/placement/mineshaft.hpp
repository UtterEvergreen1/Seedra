#pragma once

#include "terrain/generator.hpp"
#include <vector>

namespace Placement {
    /**
     * @class Mineshaft
     * @brief Represents the placement logic for Mineshaft structures in the world.
     *
     * This class provides methods to retrieve the positions of Mineshafts within a specified range
     * or radius, as well as all Mineshaft positions in the world.
     */
    class Mineshaft {
    public:
        /**
         * @brief Retrieves all Mineshaft positions within a specified rectangular range.
         *
         * This method calculates the positions of Mineshafts within the given bounds
         * using the provided world seed.
         *
         * @param worldSeed The seed of the world.
         * @param xLower The lower X bound of the range.
         * @param zLower The lower Z bound of the range.
         * @param xUpper The upper X bound of the range.
         * @param zUpper The upper Z bound of the range.
         * @return A vector of chunk positions for all Mineshafts within the specified range.
         */
        ND MU static Pos2DVec_t getPositions(i64 worldSeed, int xLower, int zLower, int xUpper, int zUpper);

        /**
         * @brief Retrieves all Mineshaft positions within a specified radius around a point.
         *
         * This method is an overload of `getPositions` that calculates the positions of Mineshafts
         * within a square area centered at the given coordinates, with the specified radius.
         *
         * @param worldSeed The seed of the world.
         * @param x The X coordinate of the center point.
         * @param z The Z coordinate of the center point.
         * @param radius The radius (in chunks) around the center point to check.
         * @return A vector of chunk positions for all Mineshafts within the specified radius.
         */
        ND MU static Pos2DVec_t getPositions(c_i64 worldSeed, c_int x, c_int z, c_int radius) {
            return getPositions(worldSeed, x - radius, z - radius, x + radius, z + radius);
        }

        /**
         * @brief Retrieves all Mineshaft positions in the world.
         *
         * This method calculates the positions of all Mineshafts within the world's coordinate bounds
         * using the provided generator.
         *
         * @param g The generator object containing world seed and coordinate bounds.
         * @return A vector of chunk positions for all Mineshafts in the world.
         */
        ND MU static Pos2DVec_t getAllPositions(const Generator &g) {
            return getPositions(g.getWorldSeed(),
                                -g.getWorldCoordinateBounds() + 1, -g.getWorldCoordinateBounds() + 1,
                                g.getWorldCoordinateBounds() - 1, g.getWorldCoordinateBounds() - 1);
        }
    };
} // namespace Placement
