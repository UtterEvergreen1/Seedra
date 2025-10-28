#pragma once

#include "common/__include.hpp"

#include "lce/enums.hpp"
#include "lce/processor.hpp"
#include "terrain/biomes/biome_t.hpp"


class Generator;
struct PerlinNoise;
struct SurfaceNoise;
struct Layer;


extern u64 mcFirstInt_Calls;
extern u64 mcFirstIsZero_Calls;


template<biome_t... BiomeIDs>
constexpr u64 makeBiomeBitmask() {
    return ((1ULL << BiomeIDs) | ...);
}


/**
 * @enum LayerId
 * @brief Enumeration of the layer indices in the layer stack.
 */
enum LayerId {
    L_CONTINENT_4096 = 0, ///< Continent layer at 1:4096 scale.
    L_ZOOM_2048, ///< Zoom layer at 1:2048 scale.
    L_LAND_2048, ///< Land layer at 1:2048 scale.
    L_ZOOM_1024, ///< Zoom layer at 1:1024 scale.
    L_LAND_1024_A, ///< Land layer A at 1:1024 scale.
    L_LAND_1024_B, ///< Land layer B at 1:1024 scale (1.7+).
    L_LAND_1024_C, ///< Land layer C at 1:1024 scale (1.7+).
    L_ISLAND_1024, ///< Island layer at 1:1024 scale (1.7+).
    L_SNOW_1024, ///< Snow layer at 1:1024 scale.
    L_LAND_1024_D, ///< Land layer D at 1:1024 scale (1.7+).
    L_COOL_1024, ///< Cool layer at 1:1024 scale (1.7+).
    L_HEAT_1024, ///< Heat layer at 1:1024 scale (1.7+).
    L_SPECIAL_1024, ///< Special layer at 1:1024 scale (1.7+).
    L_ZOOM_512, ///< Zoom layer at 1:512 scale.
    L_ZOOM_256, ///< Zoom layer at 1:256 scale.
    L_LAND_256, ///< Land layer at 1:256 scale.
    L_MUSHROOM_256, ///< Mushroom layer at 1:256 scale.
    L_DEEP_OCEAN_256, ///< Deep ocean layer at 1:256 scale (1.7+).
    L_BIOME_256, ///< Biome layer at 1:256 scale.
    L_BAMBOO_256, ///< Bamboo layer at 1:256 scale (1.14+).
    L_ZOOM_128, ///< Zoom layer at 1:128 scale.
    L_ZOOM_64, ///< Zoom layer at 1:64 scale.
    L_BIOME_EDGE_64, ///< Biome edge layer at 1:64 scale.
    L_RIVER_INIT_256, ///< River initialization layer at 1:256 scale.
    L_ZOOM_128_HILLS, ///< Zoom layer for hills at 1:128 scale.
    L_ZOOM_64_HILLS, ///< Zoom layer for hills at 1:64 scale.
    L_HILLS_64, ///< Hills layer at 1:64 scale.
    L_SUNFLOWER_64, ///< Sunflower layer at 1:64 scale (1.7+).
    L_ZOOM_32, ///< Zoom layer at 1:32 scale.
    L_LAND_32, ///< Land layer at 1:32 scale.
    L_ZOOM_16, ///< Zoom layer at 1:16 scale.
    L_SHORE_16, ///< Shore layer at 1:16 scale.
    L_SWAMP_RIVER_16, ///< Swamp river layer at 1:16 scale (1.6-).
    L_ZOOM_8, ///< Zoom layer at 1:8 scale.
    L_ZOOM_4, ///< Zoom layer at 1:4 scale.
    L_SMOOTH_4, ///< Smooth layer at 1:4 scale.
    L_ZOOM_128_RIVER, ///< Zoom layer for rivers at 1:128 scale.
    L_ZOOM_64_RIVER, ///< Zoom layer for rivers at 1:64 scale.
    L_ZOOM_32_RIVER, ///< Zoom layer for rivers at 1:32 scale.
    L_ZOOM_16_RIVER, ///< Zoom layer for rivers at 1:16 scale.
    L_ZOOM_8_RIVER, ///< Zoom layer for rivers at 1:8 scale.
    L_ZOOM_4_RIVER, ///< Zoom layer for rivers at 1:4 scale.
    L_RIVER_4, ///< River layer at 1:4 scale.
    L_SMOOTH_4_RIVER, ///< Smooth river layer at 1:4 scale.
    L_RIVER_MIX_4, ///< River mix layer at 1:4 scale.
    L_OCEAN_TEMP_256, ///< Ocean temperature layer at 1:256 scale (1.13+).
    L_ZOOM_128_OCEAN, ///< Zoom layer for oceans at 1:128 scale (1.13+).
    L_ZOOM_64_OCEAN, ///< Zoom layer for oceans at 1:64 scale (1.13+).
    L_ZOOM_32_OCEAN, ///< Zoom layer for oceans at 1:32 scale (1.13+).
    L_ZOOM_16_OCEAN, ///< Zoom layer for oceans at 1:16 scale (1.13+).
    L_ZOOM_8_OCEAN, ///< Zoom layer for oceans at 1:8 scale (1.13+).
    L_ZOOM_4_OCEAN, ///< Zoom layer for oceans at 1:4 scale (1.13+).
    L_OCEAN_MIX_4, ///< Ocean mix layer at 1:4 scale (1.13+).
    L_VORONOI_1, ///< Voronoi layer at 1:1 scale.
    L_A_MUSHROOM_32, ///< Mushroom layer A at 1:32 scale (LCE).
    L_G_MUSHROOM_16, ///< Mushroom layer G at 1:16 scale (LCE).
    L_OCEAN_EDGE_256, ///< Ocean edge layer at 1:256 scale (LCE).
    L_NUM ///< Total number of layers.
};

/**
 * @typedef mapFunc_t
 * @brief Function pointer type for mapping functions used in biome generation layers.
 *
 * This function pointer represents a mapping function that processes a layer
 * and generates an output based on the input parameters.
 *
 * @param layer Pointer to the current layer being processed.
 * @param out Pointer to the output array where the results will be stored.
 * @param x X-coordinate of the area being processed.
 * @param z Z-coordinate of the area being processed.
 * @param width Width of the area being processed.
 * @param height Height of the area being processed.
 * @return An integer status code indicating success or failure.
 */
typedef void(mapFunc_t)(const Layer*, biome_t*, int, int, int, int);

/**
 * @struct Layer
 * @brief Represents a single layer in the biome generation stack.
 */
struct Layer {
    mapFunc_t *getMap; ///< Function pointer for the layer's mapping function.

    MCVERSION mc; ///< Minecraft version associated with the layer.
    i8 zoom; ///< Zoom factor of the layer.
    i8 edge; ///< Maximum border required from the parent layer.
    int scale; ///< Scale of the layer (cell = scale x scale blocks).

    u64 layerSalt; ///< Processed salt or initialization mode.
    u64 startSalt; ///< Salt used to step the PRNG forward (depends on world seed).
    u64 startSeed; ///< Starting seed for chunk seeds (depends on world seed).

    Layer *p1, *p2; ///< Parent layers.
};

/**
 * @struct LayerStack
 * @brief Represents a stack of layers for biome generation.
 */
struct LayerStack {
    Layer layers[L_NUM]; ///< Array of all layers in the stack.
    Layer *entry_1; ///< Entry layer at 1:1 scale [L_VORONOI_1].
    Layer *entry_4; ///< Entry layer at 1:4 scale [L_RIVER_MIX_4|L_OCEAN_MIX_4].
    Layer *entry_16; ///< Entry layer at 1:16 scale [L_SWAMP_RIVER_16|L_SHORE_16].
    Layer *entry_64; ///< Entry layer at 1:64 scale [L_HILLS_64|L_SUNFLOWER_64].
    Layer *entry_256; ///< Entry layer at 1:256 scale [L_BIOME_256|L_BAMBOO_256].
};

/**
 * @typedef EndNoise
 * @brief Alias for PerlinNoise used in End biome generation (1.9+).
 */
typedef PerlinNoise EndNoise;


void setLayerSeed(Layer *layer, u64 worldSeed);


MU void initSurfaceNoiseOld(SurfaceNoise *rnd, u64 *seed, double xzScale, double yScale, double xzFactor,
                            double yFactor);


void initSurfaceNoiseEnd(SurfaceNoise *rnd, u64 seed);


double sampleSurfaceNoise(const Generator *g, const SurfaceNoise *rnd, int x, int y, int z);


void setEndSeed(EndNoise *en, u64 seed);


MU int getSurfaceHeightEnd(const Generator *g, MCVERSION mc, u64 seed, int x, int z);


MU int biomeExists(MCVERSION mc, biome_t id);


MU int isOverworld(MCVERSION mc, biome_t id);


biome_t getMutated(MCVERSION mc, biome_t id);


int getCategory(MCVERSION mc, biome_t id);


int areSimilar(MCVERSION mc, biome_t id1, biome_t id2);

int isMesa(biome_t id);


int isShallowOcean(biome_t id);

int isDeepOcean(biome_t id);

int isOceanic(biome_t id);

int isSnowy(biome_t id);

Layer *setupLayer(Layer *layer, mapFunc_t *map, MCVERSION theMc, i8 zoom, i8 edge, u64 saltBase, Layer *p, Layer *p2);

void setupScale(Layer *layer, int scale);

void genArea(const Layer *layer, biome_t *out, int areaX, int areaZ, int areaWidth, int areaHeight);

void setupLayerStack(LayerStack *layerStack, LCEVERSION lceVersion, lce::BIOMESCALE biomeSize);

void getMaxArea(const Layer *layer, int areaX, int areaZ, int *maxX, int *maxZ, size_t *siz);

size_t getMinLayerCacheSize(const Layer *layer, int sizeX, int sizeZ);

//==============================================================================
// Layers
//==============================================================================

mapFunc_t mapContinent;
mapFunc_t mapZoomFuzzy;
mapFunc_t mapZoom;
mapFunc_t mapLand;
mapFunc_t mapIsland;
mapFunc_t mapSnow;
mapFunc_t mapCoolWarm;
mapFunc_t mapHeatIce;
mapFunc_t mapSpecial;
mapFunc_t mapMushroom;
mapFunc_t mapDeepOcean;
mapFunc_t mapBiome;
mapFunc_t mapBamboo;
mapFunc_t mapRiverInit;
mapFunc_t mapBiomeEdge;
mapFunc_t mapHills;
mapFunc_t mapRiver;
mapFunc_t mapSmooth;
mapFunc_t mapSunflower;
mapFunc_t mapShore;
mapFunc_t mapRiverMix;
mapFunc_t mapOceanTemp;
mapFunc_t mapOceanMix;
mapFunc_t mapGiantMushroom;
mapFunc_t mapOceanEdge;
mapFunc_t mapVoronoi114;