#pragma once

#include "LegacyCubiomes/cubiomes/biomeID.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"


namespace Placement {
    class Stronghold {
    public:
        static constexpr uint64_t stronghold_biomes =
                1ULL << plains | 1ULL << desert | 1ULL << extreme_hills | 1ULL << forest | 1ULL << taiga |
                1ULL << hell | 1ULL << the_end | 1ULL << ice_plains | 1ULL << ice_mountains |
                1ULL << mushroom_island | 1ULL << desert_hills | 1ULL << forest_hills | 1ULL << taiga_hills |
                1ULL << extreme_hills_edge | 1ULL << jungle | 1ULL << jungle_hills | 1ULL << jungle_edge |
                1ULL << stone_beach | 1ULL << birch_forest | 1ULL << birch_forest_hills |
                1ULL << roofed_forest | 1ULL << cold_taiga | 1ULL << cold_taiga_hills | 1ULL << mega_taiga |
                1ULL << mega_taiga_hills | 1ULL << extreme_hills_plus_trees | 1ULL << savanna |
                1ULL << savanna_plateau | 1ULL << mesa | 1ULL << mesa_plateau_stone | 1ULL << mesa_plateau;

        ND static Pos2D getWorldPosition(const Generator& g);
        MU ND static Pos2D getStartCenter(const Generator& g) { return getWorldPosition(g) - 4; }
        MU ND static Pos2D getStartChunk(const Generator& g) { return getWorldPosition(g) >> 4; }
        MU ND static Pos2D getRawWorldPosition(int64_t worldSeed, bool xboxStronghold);
    };
} // namespace Placement
