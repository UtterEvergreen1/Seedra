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
protected:
    static c_u64 SPAWN_BIOMES; /// u64 value for the valid spawn biomes

    i64 worldSeed{}; // world seed
    LCEVERSION version; // LCE version, used to generate biomes
    lce::CONSOLE console; // LCE console, used to generate terrain and stronghold
    lce::BIOMESCALE biomeScale; // biome size for biome generation
    lce::WORLDSIZE worldSize; // world size for calculating world bounds
    i32 worldCoordinateBounds; // block positions of the world bounds
    LayerStack layerStack{}; // stack for entries

public:
    ///========================================================================
    /// Biome Generation
    ///========================================================================

    Generator(lce::CONSOLE console, LCEVERSION version, lce::WORLDSIZE size, lce::BIOMESCALE scale);

    Generator(lce::CONSOLE console, LCEVERSION version, i64 seed, lce::WORLDSIZE size, lce::BIOMESCALE scale);

    Generator(lce::CONSOLE console, LCEVERSION version, const std::string& seed, lce::WORLDSIZE size, lce::BIOMESCALE scale);

    /// returns the stored world seed
    ND i64 getWorldSeed() const { return this->worldSeed; }

    void applyWorldSeed(i64 seed);

    void applyWorldSeed(const std::string& seed);

    /// Fast way to increment the seed for for-loops.
    MU void incrementSeed() { applyWorldSeed(getWorldSeed() + 1); }

    /// Fast way to decrement the seed for for-loops.
    MU void decrementSeed() { applyWorldSeed(getWorldSeed() - 1); }

    /// returns the stored LCE version
    MU ND LCEVERSION getLCEVersion() const { return this->version; }


    MU void changeLCEVersion(LCEVERSION versionIn);

    /// returns the stored LCE console
    ND lce::CONSOLE getConsole() const { return this->console; }

    /// Change the LCE console.
    MU void changeConsole(const lce::CONSOLE consoleIn) { this->console = consoleIn; }

    /// returns the stored biome size
    MU ND lce::BIOMESCALE getBiomeScale() const { return this->biomeScale; }

    MU void changeBiomeSize(lce::BIOMESCALE size);

    /// returns the stored world size
    MU ND lce::WORLDSIZE getWorldSize() const { return this->worldSize; }

    MU void changeWorldSize(lce::WORLDSIZE size);

    /// returns the stored world size
    MU ND i32 getWorldCoordinateBounds() const { return this->worldCoordinateBounds; }

    ND size_t getMinCacheSize(i32 scale, i32 sx, i32 sz) const;

    ND i32* allocCache(const Range& range) const;

    i32 genBiomes(i32* cache, const Range& range) const;

    ND i32 getBiomeAt(i32 scale, i32 x, i32 z) const;

    MU ND i32 getBiomeAt(c_i32 scale, Pos2D pos) const;

    ND i32* getBiomeRange(i32 scale, i32 x, i32 z, i32 w, i32 h) const;

    MU ND std::pair<i32, i32*> generateAllBiomes() const;

    ND Layer* getLayerForScale(i32 scale) const;

    //========================================================================
    // Checking Biomes & Biome Helper Functions
    //========================================================================

    ND bool areBiomesViable(i32 x, i32 z, i32 rad, u64 validBiomes, u64 mutatedValidBiomes = 0) const;

    MU ND bool areBiomesViable(Pos2D pos, i32 rad, c_u64 validBiomes, u64 mutatedValidBiomes = 0) const;

    Pos2D locateBiome(i32 x, i32 z, i32 radius, u64 validBiomes, RNG& rng, i32* passes) const;

    MU Pos2D locateBiome(Pos2D pos, i32 radius, u64 validBiomes, RNG& rng, i32* passes) const;

    i32 mapApproxHeight(float* y, i32* ids, const SurfaceNoise* sn, i32 x, i32 z, i32 w, i32 h) const;

    ND Pos2D estimateSpawn(RNG& rng) const;

    MU ND Pos2D getSpawnBlock() const;

    static bool id_matches(i32 id, u64 validBiomes, u64 mutatedValidBiomes = 0);

    static i32 stringToSeed(const std::string& theString);
};
