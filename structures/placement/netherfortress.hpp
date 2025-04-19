#pragma once
#include "common/Pos2DTemplate.hpp"
#include "common/rng.hpp"

namespace Placement {
    /**
     * @class NetherFortress
     * @brief Handles the placement logic for Nether Fortress structures in the world.
     *
     * This class provides methods to determine the world position of Nether Fortresses
     * based on the world seed.
     */
    class MU NetherFortress {
    public:
        /**
         * @brief Calculates the world position of a Nether Fortress.
         *
         * This method uses the provided world seed to determine the position of a Nether Fortress
         * in the world.
         *
         * @param worldSeed The seed of the world.
         * @return The position of the Nether Fortress as a 2D coordinate.
         */
        MU ND static Pos2D getWorldPosition(i64 worldSeed);
    };
}