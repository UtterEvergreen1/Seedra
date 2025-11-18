#pragma once

#include "WorldConfig.hpp"
#include "biomes/biome_t.hpp"
#include "terrain/biomes/layers.hpp"

#include "common/Pos2DTemplate.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/rng.hpp"
#include "components/BoundingBox.hpp"
#include "noise/NoiseGen.hpp"


class Biome;
struct Layer;
struct LayerStack;
struct Range;
struct ChunkNoise;

class BiomeCache {
    u32 m_scale;
    BoundingBox m_box;
    biome_t *m_biomes;
    bool m_generated;

public:
    BiomeCache(c_u32 scale, const BoundingBox box)
        : m_scale(scale), m_box(box), m_biomes(nullptr), m_generated(false) {
    }

    ~BiomeCache() {
        free(m_biomes);
    }

    BiomeCache(const BiomeCache& other)
    : m_scale(other.m_scale),
      m_box(other.m_box),
      m_biomes(nullptr),
      m_generated(false)
    {
        if (other.m_generated && other.m_biomes) {
            this->m_biomes = nullptr;
            this->m_generated = false;
        }
    }

    BiomeCache& operator=(const BiomeCache& other) {
        if (this == &other) return *this;
        free(m_biomes);

        m_scale = other.m_scale;
        m_box   = other.m_box;

        m_biomes   = nullptr;
        m_generated = false;

        return *this;
    }

    // transfers ownership of biomes
    BiomeCache(BiomeCache&& other) noexcept
        : m_scale(other.m_scale),
          m_box(other.m_box),
          m_biomes(other.m_biomes),
          m_generated(other.m_generated) {
        other.m_biomes = nullptr;
        other.m_generated = false;
    }

    BiomeCache& operator=(BiomeCache&& other) noexcept {
        if (this != &other) {
            free(m_biomes);
            m_scale = other.m_scale;
            m_box = other.m_box;
            m_biomes = other.m_biomes;
            m_generated = other.m_generated;

            other.m_biomes = nullptr;
            other.m_generated = false;
        }
        return *this;
    }

    ND u32 getScale() const { return m_scale; }

    ND const BoundingBox &getBox() const { return m_box; }

    ND biome_t *getBiomes() const { return m_biomes; }

    ND bool isGenerated() const { return m_generated; }

    void setBiomes(biome_t *_biomes) {
        free(this->m_biomes);
        this->m_generated = true;
        this->m_biomes = _biomes;
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

    static constexpr u64 SPAWN_BIOMES
            = makeBiomeBitmask<biome_t::forest, biome_t::plains, biome_t::taiga, biome_t::taiga_hills,
                               biome_t::wooded_hills, biome_t::jungle, biome_t::jungle_hills>();

    /**
     * @brief The world config containing settings for world generation.
     */
    WorldConfig m_config;

    /**
     * @brief The stack of layers used for biome generation.
     */
    LayerStack m_layerStack{};

    /**
     * @brief The noise used by ChunkGenerator.
     */
    mutable ChunkNoise m_chunk_noise;

    /**
     * @brief An array of biome caches used for caching world biomes at certain scales.
     */
    std::vector<BiomeCache> m_biomeCaches;

public:
    /**
     * @brief Constructs a Generator with the specified world configuration. (Useful for reusing the same config)
     * @param config The world configuration.
     */
    explicit Generator(const WorldConfig& config);

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
    Generator& operator=(const Generator&) = default;

    // enable moving
    Generator(Generator&&) noexcept = default;
    Generator& operator=(Generator&&) noexcept = default;

    /**
     * @brief Retrieves the stored world seed.
     * @return The world seed.
     */
    ND i64 getWorldSeed() const { return this->m_config.getWorldSeed(); }

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
    void generateCache(u32 scale);

    /**
     * @brief Generates biome caches for all scales up to the specified maximum scale.
     * Works differently then just calling generateCache for each scale
     * as it reuses the upper layers to generate the lower ones and avoids regenerating the upper layers
     * @param maxScale The maximum scale to generate caches for.
     */
    void generateCachesUpTo(u32 maxScale);

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
     * Setups up the perlin noise layers that will be used by ChunkGenerator.
     */
    void setupNoiseStack() const;

    /**
     * @brief returns the noise (used by ChunkGenerator)
     */
    const ChunkNoise& getChunkNoise() const;

    /**
     * @brief Retrieves the location of the biome cache for the specified scale and coordinates.
     * @param scale The scale of the cache to retrieve.
     * @param x The X-coordinate of the cache.
     * @param z The Z-coordinate of the cache.
     * @return Pointer to the biome cache at the specified scale and coordinates.
     */
    ND biome_t *getCacheAtBlock(u32 scale, int x, int z) const;

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
    ND LCEVERSION getLCEVersion() const { return this->m_config.getLCEVersion(); }

    /**
     * @brief Changes the LCE version.
     * @param versionIn The new LCE version.
     */
    void changeLCEVersion(LCEVERSION versionIn);

    /**
     * @brief Retrieves the stored LCE console type.
     * @return The LCE console type.
     */
    ND lce::CONSOLE getConsole() const { return this->m_config.getConsole(); }

    /**
     * @brief Changes the LCE console type.
     * @param consoleIn The new LCE console type.
     */
    void changeConsole(const lce::CONSOLE consoleIn) { this->m_config.setConsole(consoleIn); }

    /**
     * @brief Retrieves the stored biome scale.
     * @return The biome scale.
     */
    ND lce::BIOMESCALE getBiomeScale() const { return this->m_config.getBiomeScale(); }

    /**
     * @brief Changes the biome scale.
     * @param size The new biome scale.
     */
    void changeBiomeSize(lce::BIOMESCALE size);

    /**
     * @brief Retrieves the stored world size.
     * @return The world size.
     */
    ND lce::WORLDSIZE getWorldSize() const { return this->m_config.getWorldSize(); }

    /**
     * @brief Changes the world size.
     * @param size The new world size.
     */
    void changeWorldSize(lce::WORLDSIZE size);

    /**
     * @brief Retrieves the world generator enum.
     * @return The world generator enum.
     */
    ND WORLDGENERATOR getWorldGenerator() const { return this->m_config.getWorldGenerator(); }

    /**
     * @brief Sets the world generator enum.
     * @param gen The new world generator enum.
     */
    void setWorldGenerator(const WORLDGENERATOR gen) { this->m_config.setWorldGenerator(gen); }

    /**
     * @brief Sets the fixed biome for superflat worlds.
     * @param b The fixed biome ID.
     */
    void setFixedBiome(const biome_t b) { this->m_config.setFixedBiome(b); }

    /**
     * @brief Retrieves the fixed biome for superflat worlds.
     * @return The fixed biome ID.
     */
    ND biome_t getFixedBiome() const { return this->m_config.getFixedBiome(); }

    /**
     * @brief Sets the number of strongholds to generate in the world.
     * @param count The number of strongholds.
     */
    void setStrongholdCount(const int count) { this->m_config.setStrongholdCount(count); }

    /**
     * @brief Retrieves the number of strongholds to generate in the world.
     * @return The number of strongholds.
     */
    ND int getStrongholdCount() const { return this->m_config.getStrongholdCount(); }

    /**
     * @brief Retrieves the world coordinate bounds.
     * @return The world coordinate bounds.
     */
    ND i32 getWorldCoordinateBounds() const { return this->m_config.getWorldCoordinateBounds(); }

    /**
     * @brief Retrieves the world chunk bounds.
     * @return The world chunk bounds.
     */
    ND i32 getWorldChunkBounds() const { return this->m_config.getWorldChunkBounds(); }

    //========================================================================
    // Biome Generation
    //========================================================================

    /**
     * @brief Calculates the minimum cache size for a given scale and dimensions.
     * @param scale The scale of the cache.
     * @param sx The width of the cache.
     * @param sz The height of the cache.
     * @return The minimum cache size.
     */
    ND size_t getMinCacheSize(uint32_t scale, i32 sx, i32 sz) const;

    /**
     * @brief Allocates a cache for biome generation.
     * @param range The range of the cache.
     * @return A pointer to the allocated cache.
     */
    ND biome_t *allocCache(const Range &range) const;

    /**
     * @brief Generates biomes for a given range and stores them in the cache.
     * @param cache The cache to store the generated biomes.
     * @param range The range of biomes to generate.
     * @return The number of biomes generated.
     */
    i32 genBiomes(biome_t *cache, const Range &range) const;

    /**
     * @brief Retrieves the biome at a specific scale and coordinates.
     * @param scale The scale of the biome.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return The biome ID.
     */
    ND biome_t getBiomeIdAt(u32 scale, i32 x, i32 z) const;

    /**
     * @brief Retrieves the biome at a specific scale and 2D position.
     * @param scale The scale of the biome.
     * @param pos The 2D position.
     * @return The biome ID.
     */
    ND biome_t getBiomeIdAt(u32 scale, Pos2D pos) const;

    /**
     * @brief Retrieves the biome at a specific scale and 3D position.
     * @param scale The scale of the biome.
     * @param pos The 3D position.
     * @return The biome ID.
     */
    ND biome_t getBiomeIdAt(u32 scale, Pos3D pos) const;

    /**
     * @brief Retrieves the biome at the specified coordinates.
     * @param x The X-coordinate.
     * @param z The Z-coordinate.
     * @return Pointer to the Biome.
     */
    ND Biome *getBiomeAt(u32 scale, int x, int z) const;

    /**
     * @brief Retrieves a range of biomes at a specific scale and coordinates.
     * @param scale The scale of the biomes.
     * @param x The starting X-coordinate.
     * @param z The starting Z-coordinate.
     * @param w The width of the range.
     * @param h The height of the range.
     * @return A pointer to the range of biomes.
     */
    ND biome_t *getBiomeRange(uint32_t scale, i32 x, i32 z, i32 w, i32 h) const;

    /**
     * @brief Retrieves the world biomes at a specific scale.
     * @param scale The scale of the world biomes.
     * @return A pointer to the world biomes if it exists in the cache.
     */
    ND biome_t *getWorldBiomes(u32 scale = 1) const;

    /**
     * @brief Retrieves the layer for a specific scale.
     * @param scale The scale of the layer.
     * @return A pointer to the layer.
     */
    ND Layer *getLayerForScale(u32 scale) const;

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
    ND Pos2D locateBiome(i32 x, i32 z, i32 radius, u64 validBiomes, RNG &rng, i32 *passes) const;

    /**
     * @brief Locates a biome within a given range using a 2D position.
     * @param pos The 2D position of the range.
     * @param radius The radius of the range.
     * @param validBiomes The valid biomes as a 64-bit value.
     * @param rng The random number generator.
     * @param passes The number of passes to perform.
     * @return The position of the located biome.
     */
    ND Pos2D locateBiome(Pos2D pos, i32 radius, u64 validBiomes, RNG &rng, i32 *passes) const;

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
    ND i32 mapApproxHeight(float *y, biome_t *ids, const SurfaceNoise *sn, i32 x, i32 z, i32 w, i32 h) const;

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
    ND static bool id_matches(biome_t id, u64 validBiomes, u64 mutatedValidBiomes = 0);

private:
    /**
     * @brief Sets up the generator. (Called by constructors)
     */
    void setup();
};
