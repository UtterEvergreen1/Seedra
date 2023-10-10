#pragma once

#include "LegacyCubiomes/utils/enums.hpp"
#include "processor.hpp"

class Generator;
struct PerlinNoise;
struct SurfaceNoise;


enum class StructureType {
    DesertPyramid,
    JungleTemple, JunglePyramid = JungleTemple,
    SwampHut,
    Igloo,
    Village,
    OceanRuin,
    Mansion,
    Monument,
    Treasure,
    Shipwreck,
    Outpost,
    Mineshaft,
    Fortress,
    EndCity,
    EndGateway,
    NONE
};

enum BiomeID {
    none = -1,
    // 0
    ocean = 0,
    plains,
    desert,
    extreme_hills,                  mountains = extreme_hills,
    forest,
    taiga,
    swampland,                      swamp = swampland,
    river,
    hell,                           nether_wastes = hell,
    the_end,
    // 10
    legacy_frozen_ocean,            frozenOcean = legacy_frozen_ocean,
    frozen_river,
    ice_plains,                     snowy_tundra = ice_plains,
    ice_mountains,                  snowy_mountains = ice_mountains,
    mushroom_island,                mushroom_fields = mushroom_island,
    mushroom_island_shore,          mushroom_field_shore = mushroom_island_shore,
    beach,
    desert_hills,
    forest_hills,                   wooded_hills = forest_hills,
    taiga_hills,
    // 20
    extreme_hills_edge,             mountain_edge = extreme_hills_edge,
    jungle,
    jungle_hills,
    jungle_edge,
    deep_ocean,
    stone_beach,                    stone_shore = stone_beach,
    cold_beach,                     snowy_beach = cold_beach,
    birch_forest,
    birch_forest_hills,
    roofed_forest,                  dark_forest = roofed_forest,
    // 30
    cold_taiga,                     snowy_taiga = cold_taiga,
    cold_taiga_hills,               snowy_taiga_hills = cold_taiga_hills,
    mega_taiga,                     giant_tree_taiga = mega_taiga,
    mega_taiga_hills,               giant_tree_taiga_hills = mega_taiga_hills,
    extreme_hills_plus_trees,       wooded_mountains = extreme_hills_plus_trees,
    savanna,
    savanna_plateau,
    mesa,                           badlands = mesa,
    mesa_plateau_stone,             wooded_badlands_plateau = mesa_plateau_stone,
    mesa_plateau,                   badlands_plateau = mesa_plateau,
    // 40  --  1.13
    warm_ocean,
    deep_warm_ocean,
    lukewarm_ocean,
    deep_lukewarm_ocean,
    cold_ocean,
    deep_cold_ocean,
    frozen_ocean,
    deep_frozen_ocean,
    small_end_islands,
    end_midlands,
    end_highlands,
    end_barrens,
    // 50
    // BIOME_NUM,

    the_void = 127,

    // mutated variants
    sunflower_plains                = plains+128,
    desert_mutated                  = desert+128,                        desert_lakes = desert_mutated,
    extreme_hills_mutated           = extreme_hills+128,                 gravelly_mountains = extreme_hills_mutated,
    flower_forest                   = forest+128,
    taiga_mutated                   = taiga+128,                         taiga_mountains = taiga_mutated,
    swampland_mutated               = swampland+128,                     swamp_hills = swampland_mutated,
    ice_plains_spikes               = ice_plains+128,                    ice_spikes = ice_plains_spikes,
    jungle_mutated                  = jungle+128,                        modified_jungle = jungle_mutated,
    jungle_edge_mutated             = jungle_edge+128,                   modified_jungle_edge = jungle_edge_mutated,
    birch_forest_mutated            = birch_forest+128,                  tall_birch_forest = birch_forest_mutated,
    birch_forest_hills_mutated      = birch_forest_hills+128,            tall_birch_hills = birch_forest_hills_mutated,
    roofed_forest_mutated           = roofed_forest+128,                 dark_forest_hills = roofed_forest_mutated,
    cold_taiga_mutated              = cold_taiga+128,                    snowy_taiga_mountains = cold_taiga_mutated,
    mega_spruce_taiga               = mega_taiga+128,                    giant_spruce_taiga = mega_spruce_taiga,
    redwood_taiga_hills_mutated     = mega_taiga_hills+128,              giant_spruce_taiga_hills = redwood_taiga_hills_mutated,
    extreme_hills_plus_trees_mutated= extreme_hills_plus_trees+128,      modified_gravelly_mountains = extreme_hills_plus_trees_mutated,
    savanna_mutated                 = savanna+128,                       shattered_savanna = savanna_mutated,
    savanna_plateau_mutated         = savanna_plateau+128,               shattered_savanna_plateau = savanna_plateau_mutated,
    mesa_bryce                      = mesa+128,                          eroded_badlands = mesa_bryce,
    mesa_plateau_stone_mutated      = mesa_plateau_stone+128,            modified_wooded_badlands_plateau = mesa_plateau_stone_mutated,
    mesa_plateau_mutated            = mesa_plateau+128,                  modified_badlands_plateau = mesa_plateau_mutated,
    // 1.14
    bamboo_jungle                   = 168,
    bamboo_jungle_hills             = 169
};


enum BiomeTempCategory {
    Oceanic, Warm, Lush, Cold, Freezing, Special
};


/* Enumeration of the layer indices in the layer stack. */
enum LayerId {
    L_CONTINENT_4096 = 0,
    L_ZOOM_2048,
    L_LAND_2048,
    L_ZOOM_1024,
    L_LAND_1024_A,
    L_LAND_1024_B,                                                  // 1.7+
    L_LAND_1024_C,                                                  // 1.7+
    L_ISLAND_1024,                                                  // 1.7+
    L_SNOW_1024,
    L_LAND_1024_D,                                                  // 1.7+
    L_COOL_1024,                                                    // 1.7+
    L_HEAT_1024,                                                    // 1.7+
    L_SPECIAL_1024,                                                 // 1.7+
    L_ZOOM_512,
    //L_LAND_512,                                                     // 1.6-
    L_ZOOM_256,
    L_LAND_256,
    L_MUSHROOM_256,
    L_DEEP_OCEAN_256,                                               // 1.7+
    L_BIOME_256,
    L_BAMBOO_256,                                                   // 1.14+
    L_ZOOM_128,
    L_ZOOM_64,
    L_BIOME_EDGE_64,
    L_RIVER_INIT_256,
    L_ZOOM_128_HILLS,
    L_ZOOM_64_HILLS,
    L_HILLS_64,
    L_SUNFLOWER_64,                                                 // 1.7+
    L_ZOOM_32,
    L_LAND_32,
    L_ZOOM_16,
    L_SHORE_16,
    L_SWAMP_RIVER_16,                                               // 1.6-
    L_ZOOM_8,
    L_ZOOM_4,
    L_SMOOTH_4,
    L_ZOOM_128_RIVER,
    L_ZOOM_64_RIVER,
    L_ZOOM_32_RIVER,
    L_ZOOM_16_RIVER,
    L_ZOOM_8_RIVER,
    L_ZOOM_4_RIVER,
    L_RIVER_4,
    L_SMOOTH_4_RIVER,
    L_RIVER_MIX_4,
    L_OCEAN_TEMP_256,                                               // 1.13+
    L_ZOOM_128_OCEAN,                                               // 1.13+
    L_ZOOM_64_OCEAN,                                                // 1.13+
    L_ZOOM_32_OCEAN,                                                // 1.13+
    L_ZOOM_16_OCEAN,                                                // 1.13+
    L_ZOOM_8_OCEAN,                                                 // 1.13+
    L_ZOOM_4_OCEAN,                                                 // 1.13+
    L_OCEAN_MIX_4,                                                  // 1.13+

    L_VORONOI_1,

    // LCE
    L_A_MUSHROOM_32,
    L_G_MUSHROOM_16,
    L_OCEAN_EDGE_256,

    L_NUM
};


struct Range {
    // Cuboidal range, given by a position, size and scaling in the horizontal
    // axes, used to define a generation range. The parameters for the vertical
    // control can be left at zero when dealing with versions without 3D volume
    // support. The vertical scaling is equal to 1:1 iff scale == 1, and 1:4
    // (default biome scale) in all other cases!
    //
    // @scale:  Horizontal scale factor, should be one of 1, 4, 16, 64, or 256
    //          additionally a value of zero bypasses scaling and expects a
    //          manual generation entry layer.
    // @x,z:    Horizontal position, i.e. coordinates of north-west corner.
    // @sx,sz:  Horizontal size (width and height for 2D), should be positive.
    //
    // Volumes generated with a range are generally indexed as:
    //  out [ i_z*sx + i_x ]
    // where i_x, i_z are indecies in their respective directions.
    //
    // EXAMPLES
    // Area at normal biome scale (1:4):
    //  Range r_2d = {4, x,z, sx,sz};
    // (C99 syntax allows commission of the trailing zero-initialization.)
    //
    // Area at block scale (1:1) at sea level:
    //  Range r_surf = {1, x,z, sx,sz, 63};
    // (Block level scale uses voronoi sampling with 1:1 vertical scaling.)
    //
    // Area at chunk scale (1:16) near sea level:
    //  Range r_surf16 = {16, x,z, sx,sz, 15};
    // (Note that the vertical scaling is always 1:4 for non-voronoi scales.)
    //
    // Volume at scale (1:4):
    //  Range r_vol = {4, x,z, sx,sz, y,sy};

    int scale;
    int x, z, sx, sz;
};



struct Layer;
typedef int (mapfunc_t)(const struct Layer *, int *, int, int, int, int);

struct Layer
{
    mapfunc_t *getMap;

    int8_t mc;          // minecraft version
    int8_t zoom;        // zoom factor of layer
    int8_t edge;        // maximum border required from parent layer
    int scale;          // scale of this layer (cell = scale x scale blocks)

    uint64_t layerSalt; // processed salt or initialization mode
    uint64_t startSalt; // (depends on world seed) used to step PRNG forward
    uint64_t startSeed; // (depends on world seed) start for chunk seeds

    Layer *p, *p2;      // parent layers
};

// Overworld biome const generator
struct LayerStack
{
    Layer layers[L_NUM];
    Layer *entry_1;     // entry scale (1:1) [L_VORONOI_1]
    Layer *entry_4;     // entry scale (1:4) [L_RIVER_MIX_4|L_OCEAN_MIX_4]
    // unofficial entries for other scales (latest sensible layers):
    Layer *entry_16;    // [L_SWAMP_RIVER_16|L_SHORE_16]
    Layer *entry_64;    // [L_HILLS_64|L_SUNFLOWER_64]
    Layer *entry_256;   // [L_BIOME_256|L_BAMBOO_256]
};

// End biome const generator 1.9+
typedef PerlinNoise EndNoise;



//==============================================================================
// Essentials
//==============================================================================

/* Applies the given world seed to the layer and all dependent layers. */
void setLayerSeed(Layer *layer, uint64_t worldSeed);


//==============================================================================
// Noise
//==============================================================================

void initSurfaceNoiseOld(SurfaceNoise *rnd, uint64_t *seed,
                      double xzScale, double yScale, double xzFactor, double yFactor);
void initSurfaceNoiseEnd(SurfaceNoise *rnd, uint64_t seed);
double sampleSurfaceNoise(const Generator* g, const SurfaceNoise *rnd, int x, int y, int z);

int getBiomeDepthAndScale(int id, double *depth, double *scale, int *grass);

//==============================================================================
// End (1.9+) Biome Noise Generation
//==============================================================================
void setEndSeed(EndNoise *en, uint64_t seed);
MU int getSurfaceHeightEnd(const Generator* g, int mc, uint64_t seed, int x, int z);


//==============================================================================
// BiomeID Helpers
//==============================================================================

MU int biomeExists(int mc, int id);
MU int isOverworld(int mc, int id);
int getMutated(int mc, int id);
int getCategory(int mc, int id);
int areSimilar(int mc, int id1, int id2);
int isMesa(int id);
int isShallowOcean(int id);
int isDeepOcean(int id);
int isOceanic(int id);
int isSnowy(int id);


//==============================================================================
// Layers
//==============================================================================

//                             old names
mapfunc_t mapContinent;     // mapIsland
mapfunc_t mapZoomFuzzy;
mapfunc_t mapZoom;
mapfunc_t mapLand;          // mapAddIsland
mapfunc_t mapIsland;        // mapRemoveTooMuchOcean
mapfunc_t mapSnow;          // mapAddSnow
mapfunc_t mapCool;          // mapCoolWarm
mapfunc_t mapHeat;          // mapHeatIce
mapfunc_t mapSpecial;
mapfunc_t mapMushroom;      // mapAddMushroomIsland
mapfunc_t mapDeepOcean;
mapfunc_t mapBiome;
mapfunc_t mapBamboo;        // mapAddBamboo
mapfunc_t mapNoise;         // mapRiverInit
mapfunc_t mapBiomeEdge;
mapfunc_t mapHills;
mapfunc_t mapRiver;
mapfunc_t mapSmooth;
mapfunc_t mapSunflower;     // mapRareBiome
mapfunc_t mapShore;
mapfunc_t mapRiverMix;
mapfunc_t mapOceanTemp;
mapfunc_t mapOceanMix;

// stuff moved from generation
Layer *setupLayer(Layer *l, mapfunc_t *map, int theMc, int8_t zoom, int8_t edge, uint64_t saltbase, Layer *p, Layer *p2);
void setupScale(Layer *l, int scale);
int genArea(const Layer *layer, int *out, int areaX, int areaZ, int areaWidth, int areaHeight);
void setupLayerStack(LayerStack *layerStack, LCEVERSION lceVersion, BIOMESCALE biomeSize);
void getMaxArea(const Layer *layer, int areaX, int areaZ, int *maxX, int *maxZ, size_t *siz);
size_t getMinLayerCacheSize(const Layer *layer, int sizeX, int sizeZ);

// LCE
mapfunc_t mapGMushroom;
mapfunc_t mapOceanEdge;

// final layer 1:1
mapfunc_t mapVoronoi114;

