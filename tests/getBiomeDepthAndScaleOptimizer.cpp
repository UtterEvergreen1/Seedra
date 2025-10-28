#include "terrain/biomes/biome_t.hpp"
#include "lce/processor.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>


__declspec(noinline) int getBiomeDepthAndScale(const biome_t id, double* depth, double* scale, int* grass) {
    constexpr int dh = 62; // default height
    double s, d, g;
    switch (id) {
        case ocean:
            s = 0.100;
            d = -1.000;
            g = dh;
            break;
        case plains:
            s = 0.050;
            d = 0.125;
            g = dh;
            break;
        case desert:
            s = 0.050;
            d = 0.125;
            g = 0;
            break;
        case mountains:
            s = 0.500;
            d = 1.000;
            g = dh;
            break;
        case forest:
            s = 0.200;
            d = 0.100;
            g = dh;
            break;
        case taiga:
            s = 0.200;
            d = 0.200;
            g = dh;
            break;
        case swamp:
            s = 0.100;
            d = -0.200;
            g = dh;
            break;
        case river:
            s = 0.000;
            d = -0.500;
            g = 60;
            break;
        case frozen_ocean:
            s = 0.100;
            d = -1.000;
            g = dh;
            break;
        case frozen_river:
            s = 0.000;
            d = -0.500;
            g = 60;
            break;
        case snowy_tundra:
            s = 0.050;
            d = 0.125;
            g = dh;
            break;
        case snowy_mountains:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case mushroom_fields:
            s = 0.300;
            d = 0.200;
            g = 0;
            break;
        case mushroom_field_shore:
            s = 0.025;
            d = 0.000;
            g = 0;
            break;
        case beach:
            s = 0.025;
            d = 0.000;
            g = 64;
            break;
        case desert_hills:
            s = 0.300;
            d = 0.450;
            g = 0;
            break;
        case wooded_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case taiga_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case mountain_edge:
            s = 0.300;
            d = 0.800;
            g = dh;
            break;
        case jungle:
            s = 0.200;
            d = 0.100;
            g = dh;
            break;
        case jungle_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case jungle_edge:
            s = 0.200;
            d = 0.100;
            g = dh;
            break;
        case deep_ocean:
            s = 0.100;
            d = -1.800;
            g = dh;
            break;
        case stone_shore:
            s = 0.800;
            d = 0.100;
            g = 64;
            break;
        case snowy_beach:
            s = 0.025;
            d = 0.000;
            g = 64;
            break;
        case birch_forest:
            s = 0.200;
            d = 0.100;
            g = dh;
            break;
        case birch_forest_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case dark_forest:
            s = 0.200;
            d = 0.100;
            g = dh;
            break;
        case snowy_taiga:
            s = 0.200;
            d = 0.200;
            g = dh;
            break;
        case snowy_taiga_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case giant_tree_taiga:
            s = 0.200;
            d = 0.200;
            g = dh;
            break;
        case giant_tree_taiga_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        case wooded_mountains:
            s = 0.500;
            d = 1.000;
            g = dh;
            break;
        case savanna:
            s = 0.050;
            d = 0.125;
            g = dh;
            break;
        case savanna_plateau:
            s = 0.025;
            d = 1.500;
            g = dh;
            break;
        case badlands:
            s = 0.200;
            d = 0.100;
            g = 0;
            break;
        case wooded_badlands_plateau:
            s = 0.025;
            d = 1.500;
            g = 0;
            break;
        case badlands_plateau:
            s = 0.025;
            d = 1.500;
            g = 0;
            break;
        case warm_ocean:
            s = 0.100;
            d = -1.000;
            g = 0;
            break;
        case lukewarm_ocean:
            s = 0.100;
            d = -1.000;
            g = dh;
            break;
        case cold_ocean:
            s = 0.100;
            d = -1.000;
            g = dh;
            break;
        case deep_warm_ocean:
            s = 0.100;
            d = -1.800;
            g = 0;
            break;
        case deep_lukewarm_ocean:
            s = 0.100;
            d = -1.800;
            g = dh;
            break;
        case deep_cold_ocean:
            s = 0.100;
            d = -1.800;
            g = dh;
            break;
        case deep_frozen_ocean:
            s = 0.100;
            d = -1.800;
            g = dh;
            break;
        case sunflower_plains:
            s = 0.050;
            d = 0.125;
            g = dh;
            break;
        case desert_lakes:
            s = 0.250;
            d = 0.225;
            g = 0;
            break;
        case gravelly_mountains:
            s = 0.500;
            d = 1.000;
            g = dh;
            break;
        case flower_forest:
            s = 0.400;
            d = 0.100;
            g = dh;
            break;
        case taiga_mountains:
            s = 0.400;
            d = 0.300;
            g = dh;
            break;
        case swamp_hills:
            s = 0.300;
            d = -0.100;
            g = dh;
            break;
        case ice_spikes:
            s = 0.450;
            d = 0.425;
            g = 0;
            break;
        case modified_jungle:
            s = 0.400;
            d = 0.200;
            g = dh;
            break;
        case modified_jungle_edge:
            s = 0.400;
            d = 0.200;
            g = dh;
            break;
        case tall_birch_forest:
            s = 0.400;
            d = 0.200;
            g = dh;
            break;
        case tall_birch_hills:
            s = 0.500;
            d = 0.550;
            g = dh;
            break;
        case dark_forest_hills:
            s = 0.400;
            d = 0.200;
            g = dh;
            break;
        case snowy_taiga_mountains:
            s = 0.400;
            d = 0.300;
            g = dh;
            break;
        case giant_spruce_taiga:
            s = 0.200;
            d = 0.200;
            g = dh;
            break;
        case giant_spruce_taiga_hills:
            s = 0.200;
            d = 0.200;
            g = dh;
            break;
        case modified_gravelly_mountains:
            s = 0.500;
            d = 1.000;
            g = dh;
            break;
        case shattered_savanna:
            s = 1.225;
            d = 0.3625;
            g = dh;
            break;
        case shattered_savanna_plateau:
            s = 1.212;
            d = 1.050;
            g = dh;
            break;
        case eroded_badlands:
            s = 0.200;
            d = 0.100;
            g = 0;
            break;
        case modified_wooded_badlands_plateau:
            s = 0.300;
            d = 0.450;
            g = 0;
            break;
        case modified_badlands_plateau:
            s = 0.300;
            d = 0.450;
            g = 0;
            break;
        case bamboo_jungle:
            s = 0.200;
            d = 0.100;
            g = dh;
            break;
        case bamboo_jungle_hills:
            s = 0.300;
            d = 0.450;
            g = dh;
            break;
        default:
            printf("getBiomeDepthAndScale_OLD(): unhandled case %d\n", id);
            return 0;
    }
    if (scale) *scale = s;
    if (depth) *depth = d;
    if (grass) *grass = g;
    return 1;
}

template<typename T>
struct Row {
    T depth;
    T scale;
    uint8_t grass;
    uint8_t valid;
};

template<typename T>
static consteval std::array<Row<T>,256> makeRows() {
    std::array<Row<T>,256> t{};
    auto set = [&](biome_t b, T depth, T scale, uint8_t grass){
        t[(uint16_t)b] = Row{depth, scale, grass, 1};
    };

    // Values copied verbatim from the switch (order is depth, scale)
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
    set(shattered_savanna,                0.3625,1.225, 62);
    set(shattered_savanna_plateau,        1.050, 1.212, 62);
    set(eroded_badlands,                  0.100, 0.200,  0);
    set(modified_wooded_badlands_plateau, 0.450, 0.300,  0);
    set(modified_badlands_plateau,        0.450, 0.300,  0);
    set(bamboo_jungle,                    0.100, 0.200, 62);
    set(bamboo_jungle_hills,              0.450, 0.300, 62);

    return t;
}

static constexpr auto kRowsF = makeRows<float>();
static constexpr auto kRowsD = makeRows<double>();


__declspec(noinline)
int getBiomeDepthAndScale_FLOAT(biome_t id, double* depth, double* scale, int* grass) {
    const Row r = kRowsF[(uint16_t)id];
    if (!r.valid) {
        std::printf("getBiomeDepthAndScale_PACKED(): unhandled case %d\n", (int)id);
        return 0;
    }
    if (scale) *scale = r.scale;   // no multiply
    if (depth) *depth = r.depth;   // no multiply
    if (grass) *grass = r.grass;   // no decode
    return 1;
}


__declspec(noinline)
int getBiomeDepthAndScale_DOUBLE(biome_t id, double* depth, double* scale, int* grass) {
    const Row r = kRowsD[(uint16_t)id];
    if (!r.valid) {
        std::printf("getBiomeDepthAndScale_PACKED(): unhandled case %d\n", (int)id);
        return 0;
    }
    if (scale) *scale = r.scale;   // no multiply
    if (depth) *depth = r.depth;   // no multiply
    if (grass) *grass = r.grass;   // no decode
    return 1;
}




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

// Safe path: keeps the validity check (for debug/tests).
template<bool GetDepth, bool GetScale, bool GetGrass, bool GetInvDPlus2>
FORCEINLINE int getBiomeDepthAndScale_SAFE(biome_t id, double* depth, double* scale, int* grass, double* invDPlus2) {
    const auto u = static_cast<uint16_t>(id);
    if EXPECT_FALSE(!kT.valid[u]) return 0;
    if constexpr (GetDepth)     { *depth     = kT.depth[u]; }
    if constexpr (GetScale)     { *scale     = kT.scale[u]; }
    if constexpr (GetGrass)     { *grass     = kT.grass[u]; }
    if constexpr (GetInvDPlus2) { *invDPlus2 = kT.inv_d_plus_2[u]; }
    return 1;
}





FORCEINLINE int getBiomeDepthAndScale_FAST(biome_t id, double* depth, double* scale, int* grass) {
    const auto u = static_cast<uint16_t>(id);
    if (depth) *depth = kT.depth[u];
    if (scale) *scale = kT.scale[u];
    if (grass) *grass = kT.grass[u];
    return 1; // trust caller: valid IDs only in terrain gen
}





#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <iostream>
struct MuteStdout {
    int old_fd{-1}, null_fd{-1};
    MuteStdout() {
        fflush(stdout);
        old_fd  = _dup(_fileno(stdout));
        null_fd = _open("NUL", _O_WRONLY);
        _dup2(null_fd, _fileno(stdout));
    }
    ~MuteStdout() {
        fflush(stdout);
        if (old_fd != -1) _dup2(old_fd, _fileno(stdout)), _close(old_fd);
        if (null_fd != -1) _close(null_fd);
    }
};
#else
#include <unistd.h>
#include <fcntl.h>
struct MuteStdout {
    int old_fd{-1}, null_fd{-1};
    MuteStdout() {
        fflush(stdout);
        old_fd  = dup(fileno(stdout));
        null_fd = open("/dev/null", O_WRONLY);
        dup2(null_fd, fileno(stdout));
    }
    ~MuteStdout() {
        fflush(stdout);
        if (old_fd != -1) dup2(old_fd, fileno(stdout)), close(old_fd);
        if (null_fd != -1) close(null_fd);
    }
};
#endif

static inline bool same_bits(double a, double b) {
    return std::memcmp(&a, &b, sizeof(double)) == 0;
}


int main() {
    int mismatches = 0;

    std::puts(" id |                OLD(depth,scale,grass)                |                NEW(depth,scale,grass)                | match");
    std::puts("----+-------------------------------------------------------+-------------------------------------------------------+------");

    for (int id = 0; id < 256; ++id) {
        double d1=0, s1=0, d2=0, s2=0;
        int g1=0, g2=0;
        int ok1=0, ok2=0;

        if (kRowsD[id].valid) {
            ok1 = getBiomeDepthAndScale((biome_t)id, &d1, &s1, &g1);
            ok2 = getBiomeDepthAndScale_SAFE<true, true, true, false>((biome_t)id, &d2, &s2, &g2, nullptr);

            bool match = (ok1==ok2) && same_bits(d1,d2) && same_bits(s1,s2) && (g1==g2);
            if (!match) ++mismatches;

            std::printf("%3d | (% .17g, % .17g, %3d) ok=%d | (% .17g, % .17g, %3d) ok=%d | %s\n",
                        id, d1, s1, g1, ok1, d2, s2, g2, ok2, match ? "OK" : "<- DIFF");
        } else {
            // call both while silencing their internal printf, and ensure both return 0
            {
                MuteStdout _mute;
                ok1 = getBiomeDepthAndScale((biome_t)id, &d1, &s1, &g1);
                ok2 = getBiomeDepthAndScale_SAFE<true, true, true, false>((biome_t)id, &d2, &s2, &g2, nullptr);
            }
            bool match = (ok1==0 && ok2==0);
            if (!match) ++mismatches;

            std::printf("%3d | <unhandled>                                ok=%d | <unhandled>                                ok=%d | %s\n",
                        id, ok1, ok2, match ? "OK" : "<- DIFF");
        }
    }

    std::printf("\nexactness: %s (%d mismatches)\n\n", mismatches ? "FAIL" : "OK", mismatches);

    // ---- bench on valid IDs only ----
    std::vector<uint16_t> valid_ids;
    valid_ids.reserve(256);
    for (int id = 0; id < 256; ++id)
        if (kRowsD[id].valid) valid_ids.push_back((uint16_t)id);

    if (valid_ids.empty()) {
        std::puts("No valid ids in table — check your enum/table wiring.");
        return 0;
    }

    constexpr size_t N = 100'000'000; // heavy; drop if needed
    std::vector<uint16_t> seq(N);
    std::mt19937 rng(12345);
    std::uniform_int_distribution<size_t> pick(0, valid_ids.size()-1);
    for (size_t i = 0; i < N; ++i) seq[i] = valid_ids[pick(rng)];

    volatile double sink = 0.0;
    auto bench = [&](auto fn, const char* name){
        auto t0 = std::chrono::steady_clock::now();
        for (size_t i = 0; i < N; ++i) {
            double d, s; int g;
            fn((biome_t)seq[i], &d, &s, &g);
            sink += d + s + g;
        }
        auto t1 = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::printf("%s: %.2f ms  (sink=%f)\n", name, ms, sink);
    };

    bench(getBiomeDepthAndScale,        "switch impl");
    bench(getBiomeDepthAndScale_DOUBLE, "packed constexpr table");

    int x;
    std::cin >> x;
    return mismatches ? 1 : 0;
}
