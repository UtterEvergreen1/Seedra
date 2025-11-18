#pragma once

#include "terrain/biomes/biome_t.hpp"
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
        static constexpr uint64_t stronghold_biomes = makeBiomeBitmask<
                biome_t::plains, biome_t::desert, biome_t::extreme_hills, biome_t::forest, biome_t::taiga,
                biome_t::hell, biome_t::the_end, biome_t::ice_plains, biome_t::ice_mountains,
                biome_t::mushroom_island, biome_t::desert_hills, biome_t::forest_hills, biome_t::taiga_hills,
                biome_t::extreme_hills_edge, biome_t::jungle, biome_t::jungle_hills, biome_t::jungle_edge,
                biome_t::stone_beach, biome_t::birch_forest, biome_t::birch_forest_hills,
                biome_t::roofed_forest, biome_t::cold_taiga, biome_t::cold_taiga_hills, biome_t::mega_taiga,
                biome_t::mega_taiga_hills, biome_t::extreme_hills_plus_trees, biome_t::savanna,
                biome_t::savanna_plateau, biome_t::mesa, biome_t::mesa_plateau_stone, biome_t::mesa_plateau>();

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
