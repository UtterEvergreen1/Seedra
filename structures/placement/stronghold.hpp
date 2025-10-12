#pragma once

#include "terrain/biomes/biomeID.hpp"
#include "terrain/generator.hpp"

namespace Placement {
    /**
     * @class Stronghold
     * @brief Handles the placement logic for Stronghold structures in the world.
     *
     * This class provides methods to determine the position of Strongholds based on the world seed
     * and biome constraints. It also includes functionality to adjust placement behavior based on
     * the world size.
     */
    class Stronghold {
    public:
        /**
         * @brief Bitmask of biomes valid for Stronghold placement.
         *
         * This constant defines the biomes where Strongholds can be placed. Each biome is represented
         * as a bit in the bitmask.
         */
        static constexpr uint64_t stronghold_biomes =
                1ULL << plains | 1ULL << desert | 1ULL << extreme_hills | 1ULL << forest | 1ULL << taiga |
                1ULL << hell | 1ULL << the_end | 1ULL << ice_plains | 1ULL << ice_mountains |
                1ULL << mushroom_island | 1ULL << desert_hills | 1ULL << forest_hills | 1ULL << taiga_hills |
                1ULL << extreme_hills_edge | 1ULL << jungle | 1ULL << jungle_hills | 1ULL << jungle_edge |
                1ULL << stone_beach | 1ULL << birch_forest | 1ULL << birch_forest_hills |
                1ULL << roofed_forest | 1ULL << cold_taiga | 1ULL << cold_taiga_hills | 1ULL << mega_taiga |
                1ULL << mega_taiga_hills | 1ULL << extreme_hills_plus_trees | 1ULL << savanna |
                1ULL << savanna_plateau | 1ULL << mesa | 1ULL << mesa_plateau_stone | 1ULL << mesa_plateau;

        /**
         * @brief Flag indicating whether to use far Stronghold placement logic.
         *
         * This flag is set based on the world size. Larger worlds may require different placement logic.
         */
        static bool useFarStronghold;

        /**
         * @brief Sets the world size and adjusts Stronghold placement logic accordingly.
         *
         * If the world size is medium or larger, the `useFarStronghold` flag is set to true.
         *
         * @param worldSize The size of the world.
         */
        static void setWorldSize(const lce::WORLDSIZE worldSize) { useFarStronghold = (worldSize >= lce::WORLDSIZE::MEDIUM); }

        /**
         * @brief Calculates the world position of the Stronghold.
         *
         * This method determines the position of the Stronghold in the world using the provided generator.
         *
         * @param g The generator object containing world seed and other parameters.
         * @return The position of the Stronghold as a 2D coordinate.
         */
        ND static std::vector<Pos2D> getWorldPositions(const Generator& g);
    };
} // namespace Placement