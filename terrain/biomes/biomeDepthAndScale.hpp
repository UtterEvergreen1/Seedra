#pragma once

#include "terrain/biomes/biome_t.hpp"
#include "lce/processor.hpp" // used for FORCEINLINE and EXPECT_FALSE
#include <array>
#include <cstdint>


struct Tables {
    alignas(64) std::array<double,256> depth{};
    alignas(64) std::array<double,256> scale{};
    alignas(64) std::array<uint8_t,256> grass{};
    alignas(64) std::array<uint8_t,256> valid{};
    // Helpful for generateHeightmap’s weight = kernel / (d+2)
    alignas(64) std::array<double,256> inv_d_plus_2{};
};

static consteval Tables makeTables() {
    Tables T{};
    auto set = [&](biome_t b, double d, double s, uint8_t g){
        const auto u = static_cast<uint16_t>(b);
        T.depth[u] = d;
        T.scale[u] = s;
        T.grass[u] = g;
        T.valid[u] = 1;
    };

    // (Same literals, depth first, then scale)
    set(ocean,                           -1.000, 0.100, 62);
    set(plains,                           0.125, 0.050, 62);
    set(desert,                           0.125, 0.050,  0);
    set(mountains,                        1.000, 0.500, 62);
    set(forest,                           0.100, 0.200, 62);
    set(taiga,                            0.200, 0.200, 62);
    set(swamp,                           -0.200, 0.100, 62);
    set(river,                           -0.500, 0.000, 60);
    set(frozen_ocean,                    -1.000, 0.100, 62);
    set(frozen_river,                    -0.500, 0.000, 60);
    set(snowy_tundra,                     0.125, 0.050, 62);
    set(snowy_mountains,                  0.450, 0.300, 62);
    set(mushroom_fields,                  0.200, 0.300,  0);
    set(mushroom_field_shore,             0.000, 0.025,  0);
    set(beach,                            0.000, 0.025, 64);
    set(desert_hills,                     0.450, 0.300,  0);
    set(wooded_hills,                     0.450, 0.300, 62);
    set(taiga_hills,                      0.450, 0.300, 62);
    set(mountain_edge,                    0.800, 0.300, 62);
    set(jungle,                           0.100, 0.200, 62);
    set(jungle_hills,                     0.450, 0.300, 62);
    set(jungle_edge,                      0.100, 0.200, 62);
    set(deep_ocean,                      -1.800, 0.100, 62);
    set(stone_shore,                      0.100, 0.800, 64);
    set(snowy_beach,                      0.000, 0.025, 64);
    set(birch_forest,                     0.100, 0.200, 62);
    set(birch_forest_hills,               0.450, 0.300, 62);
    set(dark_forest,                      0.100, 0.200, 62);
    set(snowy_taiga,                      0.200, 0.200, 62);
    set(snowy_taiga_hills,                0.450, 0.300, 62);
    set(giant_tree_taiga,                 0.200, 0.200, 62);
    set(giant_tree_taiga_hills,           0.450, 0.300, 62);
    set(wooded_mountains,                 1.000, 0.500, 62);
    set(savanna,                          0.125, 0.050, 62);
    set(savanna_plateau,                  1.500, 0.025, 62);
    set(badlands,                         0.100, 0.200,  0);
    set(wooded_badlands_plateau,          1.500, 0.025,  0);
    set(badlands_plateau,                 1.500, 0.025,  0);
    set(warm_ocean,                      -1.000, 0.100,  0);
    set(lukewarm_ocean,                  -1.000, 0.100, 62);
    set(cold_ocean,                      -1.000, 0.100, 62);
    set(deep_warm_ocean,                 -1.800, 0.100,  0);
    set(deep_lukewarm_ocean,             -1.800, 0.100, 62);
    set(deep_cold_ocean,                 -1.800, 0.100, 62);
    set(deep_frozen_ocean,               -1.800, 0.100, 62);
    set(sunflower_plains,                 0.125, 0.050, 62);
    set(desert_lakes,                     0.225, 0.250,  0);
    set(gravelly_mountains,               1.000, 0.500, 62);
    set(flower_forest,                    0.100, 0.400, 62);
    set(taiga_mountains,                  0.300, 0.400, 62);
    set(swamp_hills,                     -0.100, 0.300, 62);
    set(ice_spikes,                       0.425, 0.450,  0);
    set(modified_jungle,                  0.200, 0.400, 62);
    set(modified_jungle_edge,             0.200, 0.400, 62);
    set(tall_birch_forest,                0.200, 0.400, 62);
    set(tall_birch_hills,                 0.550, 0.500, 62);
    set(dark_forest_hills,                0.200, 0.400, 62);
    set(snowy_taiga_mountains,            0.300, 0.400, 62);
    set(giant_spruce_taiga,               0.200, 0.200, 62);
    set(giant_spruce_taiga_hills,         0.200, 0.200, 62);
    set(modified_gravelly_mountains,      1.000, 0.500, 62);
    set(shattered_savanna,                0.3625, 1.225, 62);
    set(shattered_savanna_plateau,        1.050,  1.212, 62);
    set(eroded_badlands,                  0.100, 0.200,  0);
    set(modified_wooded_badlands_plateau, 0.450, 0.300,  0);
    set(modified_badlands_plateau,        0.450, 0.300,  0);
    set(bamboo_jungle,                    0.100, 0.200, 62);
    set(bamboo_jungle_hills,              0.450, 0.300, 62);

    // Precompute 1/(d+2) for every valid biome (helps the 25-tap kernel)
    for (int u = 0; u < 256; ++u) {
        if (T.valid[u])
            T.inv_d_plus_2[u] = 1.0 / (T.depth[u] + 2.0);
    }
    return T;
}

static constexpr Tables kT = makeTables();

/**
 * @brief Retrieves biome depth and scale based on the biome ID. Constexprs are used in place of checking if pointers are valid.
 * @param id The biome ID.
 * @param depth Pointer to store the depth value.
 * @param scale Pointer to store the scale value.
 * @param grass Pointer to store the grass value.
 * @param invDPlus2 Pointer to store the invDPlus2 value (used by ``ChunkGeneratorOverWorld::generateHeightmap``)
 * @return Status code indicating success or failure.
 */
template<bool GetDepth, bool GetScale, bool GetGrass, bool GetInvDPlus2>
FORCEINLINE int getBiomeDepthAndScale(
    biome_t id,
    double* depth,
    double* scale,
    int* grass,
    double* invDPlus2
) {
    const auto u = static_cast<uint16_t>(id);
    if EXPECT_FALSE(!kT.valid[u]) return 0;
    if constexpr (GetDepth)     { *depth     = kT.depth[u]; }
    if constexpr (GetScale)     { *scale     = kT.scale[u]; }
    if constexpr (GetGrass)     { *grass     = kT.grass[u]; }
    if constexpr (GetInvDPlus2) { *invDPlus2 = kT.inv_d_plus_2[u]; }
    return 1;
}