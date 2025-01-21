#pragma once

#include "support/__include.hpp"

#include "lce/enums.hpp"
#include "lce/processor.hpp"


class Generator;
struct PerlinNoise;
struct SurfaceNoise;

/// Enumeration of the layer indices in the layer stack.
enum LayerId {
    L_CONTINENT_4096 = 0,
    L_ZOOM_2048,
    L_LAND_2048,
    L_ZOOM_1024,
    L_LAND_1024_A,
    L_LAND_1024_B, // 1.7+
    L_LAND_1024_C, // 1.7+
    L_ISLAND_1024, // 1.7+
    L_SNOW_1024,
    L_LAND_1024_D,  // 1.7+
    L_COOL_1024,    // 1.7+
    L_HEAT_1024,    // 1.7+
    L_SPECIAL_1024, // 1.7+
    L_ZOOM_512,
    //L_LAND_512,   // 1.6-
    L_ZOOM_256,
    L_LAND_256,
    L_MUSHROOM_256,
    L_DEEP_OCEAN_256, // 1.7+
    L_BIOME_256,
    L_BAMBOO_256, // 1.14+
    L_ZOOM_128,
    L_ZOOM_64,
    L_BIOME_EDGE_64,
    L_RIVER_INIT_256,
    L_ZOOM_128_HILLS,
    L_ZOOM_64_HILLS,
    L_HILLS_64,
    L_SUNFLOWER_64, // 1.7+
    L_ZOOM_32,
    L_LAND_32,
    L_ZOOM_16,
    L_SHORE_16,
    L_SWAMP_RIVER_16, // 1.6-
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
    L_OCEAN_TEMP_256, // 1.13+
    L_ZOOM_128_OCEAN, // 1.13+
    L_ZOOM_64_OCEAN,  // 1.13+
    L_ZOOM_32_OCEAN,  // 1.13+
    L_ZOOM_16_OCEAN,  // 1.13+
    L_ZOOM_8_OCEAN,   // 1.13+
    L_ZOOM_4_OCEAN,   // 1.13+
    L_OCEAN_MIX_4,    // 1.13+

    L_VORONOI_1,

    L_A_MUSHROOM_32,  // LCE
    L_G_MUSHROOM_16,  // LCE
    L_OCEAN_EDGE_256, // LCE

    L_NUM
};


struct Layer;
typedef int(mapFunc_t)(const Layer*, int*, int, int, int, int);

struct Layer {
    mapFunc_t* getMap;

    MCVERSION mc; // minecraft version
    i8 zoom;  // zoom factor of layer
    i8 edge;  // maximum border required from parent layer
    int scale;    // scale of this layer (cell = scale x scale blocks)

    u64 layerSalt; // processed salt or initialization mode
    u64 startSalt; // (depends on world seed) used to step PRNG forward
    u64 startSeed; // (depends on world seed) start for chunk seeds

    Layer *p1, *p2; // parent layers
};


// Overworld biome const generator
struct LayerStack {
    Layer layers[L_NUM];
    Layer* entry_1; // entry scale (1:1) [L_VORONOI_1]
    Layer* entry_4; // entry scale (1:4) [L_RIVER_MIX_4|L_OCEAN_MIX_4]
    // unofficial entries for other scales (latest sensible layers):
    Layer* entry_16;  // [L_SWAMP_RIVER_16|L_SHORE_16]
    Layer* entry_64;  // [L_HILLS_64|L_SUNFLOWER_64]
    Layer* entry_256; // [L_BIOME_256|L_BAMBOO_256]
};


// End biome const generator 1.9+
typedef PerlinNoise EndNoise;


//==============================================================================
// Essentials
//==============================================================================

/// Applies the given world seed to the layer and all dependent layers.
void setLayerSeed(Layer* layer, u64 worldSeed);

//==============================================================================
// Noise
//==============================================================================

MU void initSurfaceNoiseOld(SurfaceNoise* rnd, u64* seed, double xzScale, double yScale, double xzFactor,
                         double yFactor);
void initSurfaceNoiseEnd(SurfaceNoise* rnd, u64 seed);
double sampleSurfaceNoise(const Generator* g, const SurfaceNoise* rnd, int x, int y, int z);

int getBiomeDepthAndScale(int id, double* depth, double* scale, int* grass);

//==============================================================================
// End (1.9+) Biome Noise Generation
//==============================================================================

void setEndSeed(EndNoise* en, u64 seed);
MU int getSurfaceHeightEnd(const Generator* g, MCVERSION mc, u64 seed, int x, int z);

//==============================================================================
// BiomeID Helpers
//==============================================================================

MU int biomeExists(MCVERSION mc, int id);
MU int isOverworld(MCVERSION mc, int id);
int getMutated(MCVERSION mc, int id);
int getCategory(MCVERSION mc, int id);
int areSimilar(MCVERSION mc, int id1, int id2);
int isMesa(int id);
int isShallowOcean(int id);
int isDeepOcean(int id);
int isOceanic(int id);
int isSnowy(int id);


//==============================================================================
// Layers
//==============================================================================

//                         old names
mapFunc_t mapContinent; // mapIsland
mapFunc_t mapZoomFuzzy;
mapFunc_t mapZoom;
mapFunc_t mapLand;   // mapAddIsland
mapFunc_t mapIsland; // mapRemoveTooMuchOcean
mapFunc_t mapSnow;   // mapAddSnow
mapFunc_t mapCool;   // mapCoolWarm
mapFunc_t mapHeat;   // mapHeatIce
mapFunc_t mapSpecial;
mapFunc_t mapMushroom; // mapAddMushroomIsland
mapFunc_t mapDeepOcean;
mapFunc_t mapBiome;
mapFunc_t mapBamboo; // mapAddBamboo
mapFunc_t mapNoise;  // mapRiverInit
mapFunc_t mapBiomeEdge;
mapFunc_t mapHills;
mapFunc_t mapRiver;
mapFunc_t mapSmooth;
mapFunc_t mapSunflower; // mapRareBiome
mapFunc_t mapShore;
mapFunc_t mapRiverMix;
mapFunc_t mapOceanTemp;
mapFunc_t mapOceanMix;
/// LCE
mapFunc_t mapGMushroom;
/// LCE
mapFunc_t mapOceanEdge;
/// final layer 1:1
mapFunc_t mapVoronoi114;

// stuff moved from generation
Layer* setupLayer(Layer* l, mapFunc_t* map, MCVERSION theMc, i8 zoom, i8 edge, u64 saltBase, Layer* p, Layer* p2);
void setupScale(Layer* l, int scale);
int genArea(const Layer* layer, int* out, int areaX, int areaZ, int areaWidth, int areaHeight);
void setupLayerStack(LayerStack* layerStack, LCEVERSION lceVersion, lce::BIOMESCALE biomeSize);
void getMaxArea(const Layer* layer, int areaX, int areaZ, int* maxX, int* maxZ, size_t* siz);
size_t getMinLayerCacheSize(const Layer* layer, int sizeX, int sizeZ);
