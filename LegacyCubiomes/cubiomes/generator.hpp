#pragma once

#include "lce/enums.hpp"
#include "LegacyCubiomes/utils/enums.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"

#include "LegacyCubiomes/utils/rng.hpp"
#include "layers.hpp"

struct Layer;
struct LayerStack;
struct Range;
struct SurfaceNoise;

#include <cstdio>
#include <cstring>
#include <memory>


class Generator {
    i64 worldSeed;     // world seed
    LCEVERSION version;    // LCE version, used to generate biomes
    lce::CONSOLE console;       // LCE console, used to generate terrain and stronghold
    lce::BIOMESCALE biomeScale; // biome size for biome generation
    lce::WORLDSIZE worldSize;   // world size for calculating world bounds

    int worldCoordinateBounds; // block positions of the world bounds

    LayerStack layerStack{}; // stack for entries

public:
    ///========================================================================
    /// Biome Generation
    ///========================================================================

    /**
     * Sets up a biome generator for a given LCE version.
     *
     * @param console target console
     * @param version update version
     * @param size the world size for calculating world bounds
     * @param scale the biome size for generating biomes
     */
    Generator(lce::CONSOLE console, LCEVERSION version, lce::WORLDSIZE size, lce::BIOMESCALE scale);

    /**
     * Sets up a biome generator for a given LCE version.
     *
     * @param console target console
     * @param version update version
     * @param scale the biome size for generating biomes
     * @param size the world size for calculating world bounds
     * @param seed the world seed to apply
     */
    Generator(lce::CONSOLE console, LCEVERSION version, i64 seed, lce::WORLDSIZE size, lce::BIOMESCALE scale);

    /// returns the stored world seed
    ND inline i64 getWorldSeed() const { return this->worldSeed; }

    /**
     * Initializes the generator for a given world seed in the overworld.
     *
     * @param seed world seed to apply
     */
    void applyWorldSeed(i64 seed);


    /// Fast way to increment the seed for for-loops.
    void incrementSeed() { applyWorldSeed(getWorldSeed() + 1); }

    /// Fast way to decrement the seed for for-loops.
    void decrementSeed() { applyWorldSeed(getWorldSeed() - 1); }

    /// returns the stored LCE version
    MU ND LCEVERSION getLCEVersion() const { return this->version; }

    /**
     * Change the version of LCE.
     *
     * @param versionIn new LCE version to apply
     */
    MU void changeLCEVersion(LCEVERSION versionIn);

    /// returns the stored LCE console
    ND lce::CONSOLE getConsole() const { return this->console; }

    /**
     * Change the LCE console.
     *
     * @param consoleIn new console to apply
     */
    MU void changeConsole(const lce::CONSOLE consoleIn) { this->console = consoleIn; }

    /// returns the stored biome size
    MU ND lce::BIOMESCALE getBiomeScale() const { return this->biomeScale; }

    /**
     * Change the biome size.
     *
     * @param size new biome size to apply
     */
    MU void changeBiomeSize(lce::BIOMESCALE size);

    /// returns the stored world size
    MU ND lce::WORLDSIZE getWorldSize() const { return this->worldSize; }

    /**
     * Change the world size.
     *
     * @param size new world size to apply
     */
    MU void changeWorldSize(lce::WORLDSIZE size);

    /// returns the stored world size
    MU ND int getWorldCoordinateBounds() const { return this->worldCoordinateBounds; }

    /**
     * Calculates the buffer size (number of ints) required to generate a 2D plane
     * The function allocCache() can be used afterwards to allocate the corresponding int
     * buffer using malloc().
     *
     * @param scale the scale for generating biomes
     * @param sx, sz width and height to generate
     * @return size to malloc()
     */
    ND size_t getMinCacheSize(int scale, int sx, int sz) const;

    /**
     * Allocates the biome cache given the range.
     *
     * @param range
     * @return pointer to the biome cache
     */
    ND int* allocCache(const Range& range) const;

    /**
     * Generates the biomes for a 2D plane scaled range given by 'r'.
     * The required length of the cache can be determined with getMinCacheSize().
     *
     * @param[in,out] cache input: allocated cache
     * output: generated biomes in cache,
     * biome ids can be accessed by indexing as: cache[ z * r.sx + x ]
     * where (x,z) is a relative position inside the 2D plane.
     * @param[in] range the range to generate the biomes in
     * @return zero upon success
     */
    int genBiomes(int* cache, const Range& range) const;

    /**
     * Gets the biome for a specified scaled position. Note that the scale should
     * be either 1 or 4, for block or biome coordinates respectively.
     *
     * @param scale the scale for generating biomes
     * @param x, z coordinates to generate the biome at
     * @return biome id or -1 if failed
     */
    ND int getBiomeAt(int scale, int x, int z) const;

    /**
     * Overload function for that allows for using Pos2D as position in genBiomes.
     *
     * @see int getBiomeAt(int scale, int x, int z) const
     * @param scale the scale for generating biomes
     * @param pos coordinates to generate the biome at
     * @return biome id or -1 if failed
     */
    MU ND int getBiomeAt(c_int scale, const Pos2D pos) const { return getBiomeAt(scale, pos.x, pos.z); }

    /**
     * Generates a biome range (x -> x + w, z -> z + h).
     *
     * @param scale the scale for generating biomes
     * @param x, z top left coordinates to generate
     * @param w, h width and height to generate
     * @return Cache of generated biomes.
     */
    ND int* getBiomeRange(int scale, int x, int z, int w, int h) const;

    /** Generates all biomes, and returns a std::pair of
     *
     * @return std::pair<N, *ids> where N is the NxN size of array size,
     * and ids is the pointer to that data.
     */
    MU ND std::pair<int, int*> generateAllBiomes() const;

    /**
     * Returns the default layer that corresponds to the given scale.
     *
     * @param scale the supported scales are {1, 4, 16, 64, 256}.
     */
    ND Layer* getLayerForScale(int scale) const;

    //========================================================================
    // Checking Biomes & Biome Helper Functions
    //========================================================================

    /**
     * Checks the given id against the valid biomes.
     *
     * @param id biome id to check
     * @param validBiomes u64 value of the valid base biomes
     * @param mutatedValidBiomes u64 value of the valid mutated biomes
     * @return true if the biome id exists in the valid biomes
     */
    static bool id_matches(c_int id, c_u64 validBiomes, c_u64 mutatedValidBiomes = 0) {
        return id < 128 ? (validBiomes & (1ULL << id)) != 0 : (mutatedValidBiomes & (1ULL << (id - 128))) != 0;
    }

    /**
     * Checks the surrounding 'rad' blocks from origin (x, z) for all valid biomes.
     *
     * @param x, z center coordinates to check valid biomes at
     * @param rad block radius to check for valid biomes
     * @param validBiomes u64 value of the valid base biomes
     * @param mutatedValidBiomes u64 value of the valid mutated biomes
     * @return true if all the biomes are valid within the radius
     */
    ND bool areBiomesViable(int x, int z, int rad, u64 validBiomes, u64 mutatedValidBiomes = 0) const;

    /**
     * Overload function for that allows for using Pos2D as position in areBiomesViable.
     *
     * @see bool areBiomesViable(int x, int z, int rad, const char* validBiomes) const
     * @param pos center coordinates to check valid biomes at
     * @param rad block radius to check for valid biomes
     * @param validBiomes u64 value of the valid base biomes
     * @param mutatedValidBiomes u64 value of the valid mutated biomes
     * @return true if all the biomes are valid within the radius
     */
    MU ND bool areBiomesViable(const Pos2D pos, c_int rad, c_u64 validBiomes,
                               c_u64 mutatedValidBiomes = 0) const {
        return areBiomesViable(pos.x, pos.z, rad, validBiomes, mutatedValidBiomes);
    }

    /**
     * Finds a valid biome within 'rad' blocks from origin (x, z) with the rng state 'rng'.
     *
     * @param[in] x center coordinates to check valid biomes at
     * @param[in] z center coordinates to check valid biomes at
     * @param[in] radius block radius to find valid biomes
     * @param[in] validBiomes u64 value of the valid base biomes
     * @param[in] rng pointer to the rng state
     * @param[out] passes returns the total amount of positions picked
     * @return the found position, not found if passes = 0
     */
    Pos2D locateBiome(int x, int z, int radius, u64 validBiomes, RNG& rng, int* passes) const;

    /**
     * Overload function for that allows for using Pos2D as position in locateBiome.
     *
     * @see Pos2D locateBiome(int x, int z, int radius, c_u64& validBiomes, u64* rng, int* passes) const
     * @param[in] pos center coordinates to check valid biomes at
     * @param[in] radius block radius to find valid biomes
     * @param[in] validBiomes u64 value of the valid base biomes
     * @param[in] rng pointer to the rng state
     * @param[out] passes returns the total amount of positions picked
     * @return the found position, not found if passes = 0
     */
    MU Pos2D locateBiome(const Pos2D pos, c_int radius, c_u64 validBiomes, RNG& rng, int* passes) const {
        return locateBiome(pos.x, pos.z, radius, validBiomes, rng, passes);
    }

    /**
     * Generates the approximate terrain height range (x -> x + w, z -> z + h) into 'y'.
     *
     * @param[in,out] y Pointer to the cached y values
     * @param[out] ids if 'ids' != 0, it will store the biome id
     * @param[in] sn SurfaceNoise instance pointer
     * @param[in] x, z top left coordinates to generate
     * @param[in] w, h width and height to generate
     * @return zero on success
     */
    int mapApproxHeight(float* y, int* ids, const SurfaceNoise* sn, int x, int z, int w, int h) const;

    /// u64 value for the valid spawn biomes
    static c_u64 spawn_biomes;

    /**
     * Estimates the spawn by only calling locateBiome.
     *
     * @param rng the current rng state
     * @return found spawn block coordinates, if not found, then (8, 8)
     */
    ND Pos2D estimateSpawn(RNG& rng) const;

    /// Finds the spawn block coordinates (not currently correct in wooded_badlands_plateau or mesa_plateau_stone).
    MU ND Pos2D getSpawnBlock() const;
};
