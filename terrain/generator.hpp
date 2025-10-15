#pragma once

#include "WorldConfig.hpp"
#include "biomes/biomeID.hpp"
#include "terrain/biomes/layers.hpp"

#include "common/Pos2DTemplate.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/rng.hpp"
#include "components/BoundingBox.hpp"

class Biome;
struct Layer;
struct LayerStack;
struct Range;
struct SurfaceNoise;

class BiomeCache {
    int scale;
    BoundingBox box;
    int *biomes;
    bool generated;

public:
    BiomeCache(int scale, BoundingBox box)
        : scale(scale), box(box), biomes(nullptr), generated(false) {
    }

    ~BiomeCache() {
        delete biomes;
    }

    ND int getScale() const { return scale; }

    ND const BoundingBox &getBox() const { return box; }

    ND int *getBiomes() const { return biomes; }

    ND bool isGenerated() const { return generated; }

    void setBiomes(int *biomes) {
        delete this->biomes;
        this->generated = true;
        this->biomes = biomes;
    }
};

/**
 * @class Generator
 * @brief Handles biome and terrain generation for a world.
 *
 * This class provides methods for generating biomes, managing seeds, and configuring
 * world generation parameters such as size, scale, and console type.
 */
class Generator {
    /**
     * @brief Valid spawn biomes represented as a 64-bit value.
     */
    static constexpr uint64_t SPAWN_BIOMES = (1ULL << forest) | (1ULL << plains) | (1ULL << taiga) | (
                                                 1ULL << taiga_hills) |
                                             (1ULL << wooded_hills) | (1ULL << jungle) | (1ULL << jungle_hills);

    /**
     * @brief The world config containing settings for world generation.
     */
    WorldConfig config;

    /**
     * @brief The stack of layers used for biome generation.
     */
    LayerStack layerStack{};

    /**
     * @brief An array of biome caches used for caching world biomes at certain scales.
     */
    std::vector<BiomeCache> biomeCaches;

public:
    ///========================================================================
    /// Biome Generation
    ///========================================================================

    /**
     * @brief Constructs a Generator with the specified console, version, world size, and biome scale.
     * @param console The LCE console type.
     * @param version The LCE version.
     * @param size The world size.
     * @param scale The biome scale.
     * @param worldGen The world generator enum.
     */
    Generator(lce::CONSOLE console, LCEVERSION version, lce::WORLDSIZE size, lce::BIOMESCALE scale,
              WORLDGENERATOR worldGen = WORLDGENERATOR::DEFAULT);

    /**
     * @brief Constructs a Generator with the specified console, version, seed, world size, and biome scale.
     * @param console The LCE console type.
     * @param version The LCE version.
     * @param seed The world seed.
     * @param size The world size.
     * @param scale The biome scale.
     * @param worldGen The world generator enum.
     */
    Generator(lce::CONSOLE console, LCEVERSION version, i64 seed, lce::WORLDSIZE size, lce::BIOMESCALE scale,
              WORLDGENERATOR worldGen = WORLDGENERATOR::DEFAULT);

    /**
     * @brief Constructs a Generator with the specified console, version, seed string, world size, and biome scale.
     * @param console The LCE console type.
     * @param version The LCE version.
     * @param seed The world seed as a string.
     * @param size The world size.
     * @param scale The biome scale.
     * @param worldGen The world generator enum.
     */
    Generator(lce::CONSOLE console, LCEVERSION version, const std::string &seed, lce::WORLDSIZE size,
              lce::BIOMESCALE scale, WORLDGENERATOR worldGen = WORLDGENERATOR::DEFAULT);

    /**
     * @brief Copy constructor for the Generator class.
     * @param other The Generator instance to copy from.
     */
    Generator(const Generator &other);

    /**
     * @brief Retrieves the stored world seed.
     * @return The world seed.
     */
    ND i64 getWorldSeed() const { return this->config.getWorldSeed(); }

    /**
     * @brief Applies a new world seed.
     * @param seed The new world seed.
     */
    void applyWorldSeed(i64 seed);

    /**
     * @brief Applies a new world seed from a string.
     * @param seed The new world seed as a string.
     */
    void applyWorldSeed(const std::string &seed);

    /**
     * @brief Generates a biome cache for the specified scale.
     * @param scale The scale of the cache to generate.
     */
    void generateCache(int scale);

    /**
     * @brief Generates biome caches for all scales up to the specified maximum scale.
     * Works differently then just calling generateCache for each scale
     * as it reuses the upper layers to generate the lower ones and avoids regenerating the upper layers
     * @param maxScale The maximum scale to generate caches for.
     */
    void generateCachesUpTo(int maxScale);

    /**
     * @brief Generates all biome caches for all scales. (1-256)
     * This method generates caches for all scales defined in the layer stack.
     */
    void generateAllCaches();

    /**
     * @brief Reloads the biome caches.
     * This method clears and regenerates all biome caches. Internally used when the world seed or settings change.
     */
    void reloadCache();

    /**
     * @brief Retrieves the location of the biome cache for the specified scale and coordinates.
     * @param scale The scale of the cache to retrieve.
     * @param x The X-coordinate of the cache.
     * @param z The Z-coordinate of the cache.
     * @return Pointer to the biome cache at the specified scale and coordinates.
     */
    ND int *getCacheAtBlock(int scale, int x, int z) const;

    /**
     * @brief Increments the world seed by 1.
     */
    void incrementSeed() { applyWorldSeed(getWorldSeed() + 1); }

    /**
     * @brief Decrements the world seed by 1.
     */
    void decrementSeed() { applyWorldSeed(getWorldSeed() - 1); }

    /**
     * @brief Retrieves the stored LCE version.
     * @return The LCE version.
     */
    ND LCEVERSION getLCEVersion() const { return this->config.getLCEVersion(); }

    /**
     * @brief Changes the LCE version.
     * @param versionIn The new LCE version.
     */
    void changeLCEVersion(LCEVERSION versionIn);

    /**
     * @brief Retrieves the stored LCE console type.
     * @return The LCE console type.
     */
    ND lce::CONSOLE getConsole() const { return this->config.getConsole(); }

    /**
     * @brief Changes the LCE console type.
     * @param consoleIn The new LCE console type.
     */
    void changeConsole(const lce::CONSOLE consoleIn) { this->config.setConsole(consoleIn); }

    /**
     * @brief Retrieves the stored biome scale.
     * @return The biome scale.
     */
    ND lce::BIOMESCALE getBiomeScale() const { return this->config.getBiomeScale(); }

    /**
     * @brief Changes the biome scale.
     * @param size The new biome scale.
     */
    void changeBiomeSize(lce::BIOMESCALE size);

    /**
     * @brief Retrieves the stored world size.
     * @return The world size.
     */
    ND lce::WORLDSIZE getWorldSize() const { return this->config.getWorldSize(); }

    /**
     * @brief Changes the world size.
     * @param size The new world size.
     */
    void changeWorldSize(lce::WORLDSIZE size);

    /**
     * @brief Retrieves the world generator enum.
     * @return The world generator enum.
     */
    ND WORLDGENERATOR getWorldGenerator() const { return this->config.getWorldGenerator(); }

    /**
     * @brief Sets the world generator enum.
     * @param gen The new world generator enum.
     */
    void setWorldGenerator(const WORLDGENERATOR gen) { this->config.setWorldGenerator(gen); }

    /**
     * @brief Sets the fixed biome for superflat worlds.
     * @param b The fixed biome ID.
     */
    void setFixedBiome(const BiomeID b) { this->config.setFixedBiome(b); }

    /**
     * @brief Retrieves the fixed biome for superflat worlds.
     * @return The fixed biome ID.
     */
    BiomeID getFixedBiome() const { return this->config.getFixedBiome(); }

    /**
     * @brief Sets the number of strongholds to generate in the world.
     * @param count The number of strongholds.
     */
    void setStrongholdCount(const int count) { this->config.setStrongholdCount(count); }

    /**
     * @brief Retrieves the number of strongholds to generate in the world.
     * @return The number of strongholds.
     */
    int getStrongholdCount() const { return this->config.getStrongholdCount(); }

    /**
     * @brief Retrieves the world coordinate bounds.
     * @return The world coordinate bounds.
     */
    ND i32 getWorldCoordinateBounds() const { return this->config.getWorldCoordinateBounds(); }

    /**
     * @brief Retrieves the world chunk bounds.
     * @return The world chunk bounds.
     */
    ND i32 getWorldChunkBounds() const { return this->config.getWorldChunkBounds(); }

    /**
     * @brief Calculates the minimum cache size for a given scale and dimensions.
     * @param scale The scale of the cache.
     * @param sx The width of the cache.
     * @param sz The height of the cache.
     * @return The minimum cache size.
     */
    ND size_t getMinCacheSize(i32 scale, i32 sx, i32 sz) const;

    /**
     * @brief Allocates a cache for biome generation.
     * @param range The range of the cache.
     * @return A pointer to the allocated cache.
     */
    ND i32 *allocCache(const Range &range) const;

    /**
     * @brief Generates biomes for a given range and stores them in the cache.
     * @param cache The cache to store the generated biomes.
     * @param range The range of biomes to generate.
     * @return The number of biomes generated.
     */
    i32 genBiomes(i32 *cache, const Range &range) const;

    /**
     * @brief Retrieves the biome at a specific scale and coordinates.
     * @param scale The scale of the biome.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The biome ID.
     */
    ND i32 getBiomeIdAt(i32 scale, i32 x, i32 z) const;

    /**
     * @brief Retrieves the biome at a specific scale and 2D position.
     * @param scale The scale of the biome.
     * @param pos The 2D position.
     * @return The biome ID.
     */
    ND i32 getBiomeIdAt(c_i32 scale, Pos2D pos) const;

    /**
     * @brief Retrieves the biome at a specific scale and 3D position.
     * @param scale The scale of the biome.
     * @param pos The 3D position.
     * @return The biome ID.
     */
    ND i32 getBiomeIdAt(c_i32 scale, Pos3D pos) const;

    /**
     * @brief Retrieves the biome at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return Pointer to the Biome.
     */
    Biome *getBiomeAt(int scale, int x, int z) const;

    /**
     * @brief Retrieves a range of biomes at a specific scale and coordinates.
     * @param scale The scale of the biomes.
     * @param x The starting X-coordinate.
     * @param z The starting Z-coordinate.
     * @param w The width of the range.
     * @param h The height of the range.
     * @return A pointer to the range of biomes.
     */
    ND i32 *getBiomeRange(i32 scale, i32 x, i32 z, i32 w, i32 h) const;

    /**
     * @brief Retrieves the world biomes at a specific scale.
     * @param scale The scale of the world biomes.
     * @return A pointer to the world biomes if it exists in the cache.
     */
    int *getWorldBiomes(int scale = 1) const;

    /**
     * @brief Retrieves the layer for a specific scale.
     * @param scale The scale of the layer.
     * @return A pointer to the layer.
     */
    ND Layer *getLayerForScale(i32 scale) const;

    //========================================================================
    // Checking Biomes & Biome Helper Functions
    //========================================================================

    /**
     * @brief Checks if biomes in a given range are viable.
     * @param x The X-coordinate of the range.
     * @param z The Z-coordinate of the range.
     * @param rad The radius of the range.
     * @param validBiomes The valid biomes as a 64-bit value.
     * @param mutatedValidBiomes The mutated valid biomes as a 64-bit value (optional).
     * @return True if the biomes are viable, false otherwise.
     */
    ND bool areBiomesViable(i32 x, i32 z, i32 rad, u64 validBiomes, u64 mutatedValidBiomes = 0) const;

    /**
     * @brief Checks if biomes in a given range are viable using a 2D position.
     * @param pos The 2D position of the range.
     * @param rad The radius of the range.
     * @param validBiomes The valid biomes as a 64-bit value.
     * @param mutatedValidBiomes The mutated valid biomes as a 64-bit value (optional).
     * @return True if the biomes are viable, false otherwise.
     */
    ND bool areBiomesViable(Pos2D pos, i32 rad, c_u64 validBiomes, u64 mutatedValidBiomes = 0) const;

    /**
     * @brief Locates a biome within a given range.
     * @param x The X-coordinate of the range.
     * @param z The Z-coordinate of the range.
     * @param radius The radius of the range.
     * @param validBiomes The valid biomes as a 64-bit value.
     * @param rng The random number generator.
     * @param passes The number of passes to perform.
     * @return The position of the located biome.
     */
    Pos2D locateBiome(i32 x, i32 z, i32 radius, u64 validBiomes, RNG &rng, i32 *passes) const;

    /**
     * @brief Locates a biome within a given range using a 2D position.
     * @param pos The 2D position of the range.
     * @param radius The radius of the range.
     * @param validBiomes The valid biomes as a 64-bit value.
     * @param rng The random number generator.
     * @param passes The number of passes to perform.
     * @return The position of the located biome.
     */
    Pos2D locateBiome(Pos2D pos, i32 radius, u64 validBiomes, RNG &rng, i32 *passes) const;

    /**
     * @brief Maps approximate height values for a given range.
     * @param y The height values.
     * @param ids The biome IDs.
     * @param sn The surface noise.
     * @param x The starting X-coordinate.
     * @param z The starting Z-coordinate.
     * @param w The width of the range.
     * @param h The height of the range.
     * @return The number of heights mapped.
     */
    i32 mapApproxHeight(float *y, i32 *ids, const SurfaceNoise *sn, i32 x, i32 z, i32 w, i32 h) const;

    /**
     * @brief Estimates the spawn position using a random number generator.
     * @param rng The random number generator.
     * @return The estimated spawn position.
     */
    ND Pos2D estimateSpawn(RNG &rng) const;

    /**
     * @brief Retrieves the spawn block position.
     * @return The spawn block position.
     */
    ND Pos2D getSpawnBlock() const;

    /**
     * @brief Checks if a biome ID matches the valid biomes.
     * @param id The biome ID.
     * @param validBiomes The valid biomes as a 64-bit value.
     * @param mutatedValidBiomes The mutated valid biomes as a 64-bit value (optional).
     * @return True if the biome ID matches, false otherwise.
     */
    static bool id_matches(i32 id, u64 validBiomes, u64 mutatedValidBiomes = 0);
};
