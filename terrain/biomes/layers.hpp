#pragma once

#include "common/__include.hpp"

#include "lce/enums.hpp"
#include "lce/processor.hpp"

class Generator;
struct PerlinNoise;
struct SurfaceNoise;
struct Layer;

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
typedef int(mapFunc_t)(const Layer*, int*, int, int, int, int);

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

/**
 * @brief Applies the given world seed to the layer and all dependent layers.
 * @param layer Pointer to the layer.
 * @param worldSeed The world seed to apply.
 */
void setLayerSeed(Layer *layer, u64 worldSeed);

/**
 * @brief Initializes surface noise for older versions.
 * @param rnd Pointer to the SurfaceNoise structure.
 * @param seed Pointer to the seed.
 * @param xzScale Scale factor for the XZ plane.
 * @param yScale Scale factor for the Y axis.
 * @param xzFactor Factor for the XZ plane.
 * @param yFactor Factor for the Y axis.
 */
MU void initSurfaceNoiseOld(SurfaceNoise *rnd, u64 *seed, double xzScale, double yScale, double xzFactor,
                            double yFactor);

/**
 * @brief Initializes surface noise for the End biome.
 * @param rnd Pointer to the SurfaceNoise structure.
 * @param seed The seed to initialize with.
 */
void initSurfaceNoiseEnd(SurfaceNoise *rnd, u64 seed);

/**
 * @brief Samples surface noise at a specific position.
 * @param g Pointer to the generator.
 * @param rnd Pointer to the SurfaceNoise structure.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @param z Z-coordinate.
 * @return The sampled noise value.
 */
double sampleSurfaceNoise(const Generator *g, const SurfaceNoise *rnd, int x, int y, int z);

/**
 * @brief Retrieves biome depth and scale based on the biome ID.
 * @param id The biome ID.
 * @param depth Pointer to store the depth value.
 * @param scale Pointer to store the scale value.
 * @param grass Pointer to store the grass value.
 * @return Status code indicating success or failure.
 */
int getBiomeDepthAndScale(int id, double *depth, double *scale, int *grass);

/**
 * @brief Sets the seed for End biome noise generation.
 * @param en Pointer to the EndNoise structure.
 * @param seed The seed to set.
 */
void setEndSeed(EndNoise *en, u64 seed);

/**
 * @brief Retrieves the surface height for the End biome.
 * @param g Pointer to the generator.
 * @param mc Minecraft version.
 * @param seed The world seed.
 * @param x X-coordinate.
 * @param z Z-coordinate.
 * @return The surface height.
 */
MU int getSurfaceHeightEnd(const Generator *g, MCVERSION mc, u64 seed, int x, int z);

/**
 * @brief Checks if a biome exists for a given Minecraft version and ID.
 * @param mc Minecraft version.
 * @param id The biome ID.
 * @return 1 if the biome exists, 0 otherwise.
 */
MU int biomeExists(MCVERSION mc, int id);

/**
 * @brief Checks if a biome is part of the Overworld.
 * @param mc Minecraft version.
 * @param id The biome ID.
 * @return 1 if the biome is in the Overworld, 0 otherwise.
 */
MU int isOverworld(MCVERSION mc, int id);

/**
 * @brief Retrieves the mutated variant of a biome.
 * @param mc Minecraft version.
 * @param id The biome ID.
 * @return The ID of the mutated biome.
 */
int getMutated(MCVERSION mc, int id);

/**
 * @brief Retrieves the category of a biome.
 * @param mc Minecraft version.
 * @param id The biome ID.
 * @return The category of the biome.
 */
int getCategory(MCVERSION mc, int id);

/**
 * @brief Checks if two biomes are similar.
 * @param mc Minecraft version.
 * @param id1 The first biome ID.
 * @param id2 The second biome ID.
 * @return 1 if the biomes are similar, 0 otherwise.
 */
int areSimilar(MCVERSION mc, int id1, int id2);

/**
 * @brief Checks if a biome is a mesa biome.
 * @param id The biome ID.
 * @return 1 if the biome is a mesa biome, 0 otherwise.
 */
int isMesa(int id);

/**
 * @brief Checks if a biome is a shallow ocean biome.
 * @param id The biome ID.
 * @return 1 if the biome is a shallow ocean biome, 0 otherwise.
 */
int isShallowOcean(int id);

/**
 * @brief Checks if a biome is a deep ocean biome.
 * @param id The biome ID.
 * @return 1 if the biome is a deep ocean biome, 0 otherwise.
 */
int isDeepOcean(int id);

/**
 * @brief Checks if a biome is oceanic.
 * @param id The biome ID.
 * @return 1 if the biome is oceanic, 0 otherwise.
 */
int isOceanic(int id);

/**
 * @brief Checks if a biome is snowy.
 * @param id The biome ID.
 * @return 1 if the biome is snowy, 0 otherwise.
 */
int isSnowy(int id);

/**
 * @brief Sets up a layer with the specified parameters.
 * @param l Pointer to the layer.
 * @param map Function pointer for the layer's mapping function.
 * @param theMc Minecraft version.
 * @param zoom Zoom factor for the layer.
 * @param edge Maximum border required from the parent layer.
 * @param saltBase Base salt for the layer.
 * @param p Pointer to the first parent layer.
 * @param p2 Pointer to the second parent layer.
 * @return Pointer to the configured layer.
 */
Layer *setupLayer(Layer *l, mapFunc_t *map, MCVERSION theMc, i8 zoom, i8 edge, u64 saltBase, Layer *p, Layer *p2);

/**
 * @brief Sets up the scale for a layer.
 * @param l Pointer to the layer.
 * @param scale The scale to set.
 */
void setupScale(Layer *l, int scale);

/**
 * @brief Generates an area for the specified layer.
 * @param layer Pointer to the layer.
 * @param out Pointer to the output area.
 * @param areaX X-coordinate of the area.
 * @param areaZ Z-coordinate of the area.
 * @param areaWidth Width of the area.
 * @param areaHeight Height of the area.
 * @return Status code indicating success or failure.
 */
int genArea(const Layer *layer, int *out, int areaX, int areaZ, int areaWidth, int areaHeight);

/**
 * @brief Sets up the layer stack for biome generation.
 * @param layerStack Pointer to the LayerStack structure.
 * @param lceVersion LCE version.
 * @param biomeSize Biome size scale.
 */
void setupLayerStack(LayerStack *layerStack, LCEVERSION lceVersion, lce::BIOMESCALE biomeSize);

/**
 * @brief Retrieves the maximum area for a layer.
 * @param layer Pointer to the layer.
 * @param areaX X-coordinate of the area.
 * @param areaZ Z-coordinate of the area.
 * @param maxX Pointer to store the maximum X-coordinate.
 * @param maxZ Pointer to store the maximum Z-coordinate.
 * @param siz Pointer to store the size of the area.
 */
void getMaxArea(const Layer *layer, int areaX, int areaZ, int *maxX, int *maxZ, size_t *siz);

/**
 * @brief Retrieves the minimum cache size for a layer.
 * @param layer Pointer to the layer.
 * @param sizeX Size of the area in the X direction.
 * @param sizeZ Size of the area in the Z direction.
 * @return The minimum cache size.
 */
size_t getMinLayerCacheSize(const Layer *layer, int sizeX, int sizeZ);
//==============================================================================
// Layers
//==============================================================================

/**
 * @var mapContinent
 * @brief Mapping function for the continent layer.
 *        Previously known as mapIsland.
 */
mapFunc_t mapContinent;

/**
 * @var mapZoomFuzzy
 * @brief Mapping function for applying a fuzzy zoom effect.
 */
mapFunc_t mapZoomFuzzy;

/**
 * @var mapZoom
 * @brief Mapping function for applying a standard zoom effect.
 */
mapFunc_t mapZoom;

/**
 * @var mapLand
 * @brief Mapping function for adding land to the layer.
 *        Previously known as mapAddIsland.
 */
mapFunc_t mapLand;

/**
 * @var mapIsland
 * @brief Mapping function for removing excessive ocean areas.
 *        Previously known as mapRemoveTooMuchOcean.
 */
mapFunc_t mapIsland;

/**
 * @var mapSnow
 * @brief Mapping function for adding snow to the layer.
 *        Previously known as mapAddSnow.
 */
mapFunc_t mapSnow;

/**
 * @var mapCool
 * @brief Mapping function for adjusting cool and warm areas.
 *        Previously known as mapCoolWarm.
 */
mapFunc_t mapCool;

/**
 * @var mapHeat
 * @brief Mapping function for adjusting heat and ice areas.
 *        Previously known as mapHeatIce.
 */
mapFunc_t mapHeat;

/**
 * @var mapSpecial
 * @brief Mapping function for adding special features to the layer.
 */
mapFunc_t mapSpecial;

/**
 * @var mapMushroom
 * @brief Mapping function for adding mushroom islands.
 *        Previously known as mapAddMushroomIsland.
 */
mapFunc_t mapMushroom;

/**
 * @var mapDeepOcean
 * @brief Mapping function for adding deep ocean areas.
 */
mapFunc_t mapDeepOcean;

/**
 * @var mapBiome
 * @brief Mapping function for assigning biomes to the layer.
 */
mapFunc_t mapBiome;

/**
 * @var mapBamboo
 * @brief Mapping function for adding bamboo areas.
 *        Previously known as mapAddBamboo.
 */
mapFunc_t mapBamboo;

/**
 * @var mapNoise
 * @brief Mapping function for initializing river noise.
 *        Previously known as mapRiverInit.
 */
mapFunc_t mapNoise;

/**
 * @var mapBiomeEdge
 * @brief Mapping function for defining biome edges.
 */
mapFunc_t mapBiomeEdge;

/**
 * @var mapHills
 * @brief Mapping function for adding hills to the layer.
 */
mapFunc_t mapHills;

/**
 * @var mapRiver
 * @brief Mapping function for generating rivers.
 */
mapFunc_t mapRiver;

/**
 * @var mapSmooth
 * @brief Mapping function for smoothing transitions between areas.
 */
mapFunc_t mapSmooth;

/**
 * @var mapSunflower
 * @brief Mapping function for adding rare biomes like sunflower plains.
 *        Previously known as mapRareBiome.
 */
mapFunc_t mapSunflower;

/**
 * @var mapShore
 * @brief Mapping function for defining shorelines.
 */
mapFunc_t mapShore;

/**
 * @var mapRiverMix
 * @brief Mapping function for mixing river and land layers.
 */
mapFunc_t mapRiverMix;

/**
 * @var mapOceanTemp
 * @brief Mapping function for defining ocean temperature zones.
 */
mapFunc_t mapOceanTemp;

/**
 * @var mapOceanMix
 * @brief Mapping function for mixing ocean layers.
 */
mapFunc_t mapOceanMix;

/**
 * @var mapGMushroom
 * @brief Mapping function for adding giant mushroom areas.
 *        Specific to LCE.
 */
mapFunc_t mapGMushroom;

/**
 * @var mapOceanEdge
 * @brief Mapping function for defining ocean edges.
 *        Specific to LCE.
 */
mapFunc_t mapOceanEdge;

/**
 * @var mapVoronoi114
 * @brief Mapping function for the final Voronoi layer at 1:1 scale.
 */
mapFunc_t mapVoronoi114;