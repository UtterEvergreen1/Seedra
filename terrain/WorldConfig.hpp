#pragma once
#include <cstdint>

#include "biomes/biome_t.hpp"
#include "common/enums.hpp"
#include "components/BoundingBox.hpp"
#include "lce/enums.hpp"

// TODO: add more options as needed like custom ore height, etc...
/**
 * @struct WorldSettings
 * @brief Options for world generation, such as fixed biome for superflat and stronghold count.
 */
struct WorldSettings {
    biome_t fixedBiome = biome_t::plains; ///< Fixed biome for superflat worlds.
    int strongholdCount = 1;         ///< Number of strongholds to generate in the world.
};

/**
 * @class WorldConfig
 * @brief Represents the configuration settings for a world.
 *
 * The WorldConfig class encapsulates options
 * used for generating a world, including seed, console type and version,
 * biome scale, world size, and other generation settings.
 */
class WorldConfig {
protected:
    /**
     * @brief The world seed used for biome and terrain generation.
     */
    int64_t m_worldSeed;

    /**
     * @brief The LCE console type used for terrain and stronghold generation.
     */
    lce::CONSOLE m_console;

    /**
     * @brief The LCE version used for biome generation.
     */
    LCEVERSION m_version;

    /**
     * @brief The world size used for calculating world bounds.
     */
    lce::WORLDSIZE m_worldSize;

    /**
     * @brief The biome scale used for biome generation.
     */
    lce::BIOMESCALE m_biomeScale;

    /**
     * @brief The world generator enum used for terrain and stronghold generation.
     */
    WORLDGENERATOR m_worldGen;

    /**
     * @brief Options for world generation, such as fixed biome for superflat and stronghold count.
     */
    WorldSettings worldOptions{};

    /**
     * @brief The block positions of the world bounds.
     */
    int m_worldCoordinateBounds;

    /**
     * @brief The bounding box representing the world bounds.
     */
    BoundingBox m_worldBounds;

public:
    /** @brief Default constructor for WorldConfig.
     *
     * Initializes the world configuration with default values.
     */
    WorldConfig() = default;

    /**
     * @brief Constructs a WorldConfig object with the specified parameters.
     *
     * This constructor initializes the world configuration with the given seed, console type,
     * LCE version, world size, biome scale, and world generator. It also calculates the world
     * coordinate bounds and initializes the bounding box representing the world bounds.
     *
     * @param seed The world seed used for biome and terrain generation.
     * @param console The LCE console type used for terrain and stronghold generation.
     * @param version The LCE version used for biome generation.
     * @param size The world size used for calculating world bounds.
     * @param scale The biome scale used for biome generation.
     * @param worldGen The world generator enum used for terrain and stronghold generation.
     *                 Defaults to WORLDGENERATOR::DEFAULT.
     */
    WorldConfig(int64_t seed, const lce::CONSOLE console, const LCEVERSION version, const lce::WORLDSIZE size,
                const lce::BIOMESCALE scale, WORLDGENERATOR worldGen = WORLDGENERATOR::DEFAULT)
        : m_worldSeed(seed), m_console(console), m_version(version), m_worldSize(size), m_biomeScale(scale),
          m_worldGen(worldGen), m_worldCoordinateBounds(getChunkWorldBounds(size) << 4),
          m_worldBounds(-m_worldCoordinateBounds, -m_worldCoordinateBounds, m_worldCoordinateBounds,
                        m_worldCoordinateBounds) {
    }

    /**
     * @brief Retrieves the stored world seed.
     * @return The world seed.
     */
    ND int64_t getWorldSeed() const { return this->m_worldSeed; }

    /**
     * @brief Sets a new world seed.
     * @param seed The new world seed.
     */
    void setWorldSeed(int64_t seed) { this->m_worldSeed = seed; }

    /**
     * @brief Retrieves the stored LCE version.
     * @return The LCE version.
     */
    ND LCEVERSION getLCEVersion() const { return this->m_version; }

    /**
     * @brief Sets the LCE version.
     * @param versionIn The new LCE version.
     */
    void setLCEVersion(LCEVERSION versionIn) { this->m_version = versionIn; }

    /**
     * @brief Retrieves the stored LCE console type.
     * @return The LCE console type.
     */
    ND lce::CONSOLE getConsole() const { return this->m_console; }

    /**
     * @brief Sets the LCE console type.
     * @param consoleIn The new LCE console type.
     */
    void setConsole(const lce::CONSOLE consoleIn) { this->m_console = consoleIn; }

    /**
     * @brief Retrieves the stored biome scale.
     * @return The biome scale.
     */
    ND lce::BIOMESCALE getBiomeScale() const { return this->m_biomeScale; }

    /**
     * @brief Sets the biome scale.
     * @param size The new biome scale.
     */
    void setBiomeScale(lce::BIOMESCALE size) { this->m_biomeScale = size; }

    /**
     * @brief Retrieves the stored world size.
     * @return The world size.
     */
    ND lce::WORLDSIZE getWorldSize() const { return this->m_worldSize; }

    /**
     * @brief Sets the world size.
     * @param size The new world size.
     */
    void setWorldSize(lce::WORLDSIZE size) {
        this->m_worldSize = size;
        this->m_worldCoordinateBounds = getChunkWorldBounds(size) << 4;
        this->m_worldBounds = BoundingBox(-this->m_worldCoordinateBounds, -this->m_worldCoordinateBounds,
                                         this->m_worldCoordinateBounds, this->m_worldCoordinateBounds);
    }

    /**
     * @brief Retrieves the world generator enum.
     * @return The world generator enum.
     */
    ND WORLDGENERATOR getWorldGenerator() const { return this->m_worldGen; }

    /**
     * @brief Sets the world generator enum.
     * @param gen The new world generator enum.
     */
    void setWorldGenerator(const WORLDGENERATOR gen) { this->m_worldGen = gen; }

    /**
     * @brief Sets the fixed biome for superflat worlds.
     * @param b The fixed biome ID.
     */
    void setFixedBiome(const biome_t b) { this->worldOptions.fixedBiome = b; }

    /**
     * @brief Retrieves the fixed biome for superflat worlds.
     * @return The fixed biome ID.
     */
    ND biome_t getFixedBiome() const { return this->worldOptions.fixedBiome; }

    /**
     * @brief Sets the number of strongholds to generate in the world.
     * @param count The number of strongholds.
     */
    void setStrongholdCount(const int count) { this->worldOptions.strongholdCount = count; }

    /**
     * @brief Retrieves the number of strongholds to generate in the world.
     * @return The number of strongholds.
     */
    ND int getStrongholdCount() const { return this->worldOptions.strongholdCount; }

    /**
     * @brief Retrieves the world coordinate bounds.
     * @return The world coordinate bounds.
     */
    ND int getWorldCoordinateBounds() const { return this->m_worldCoordinateBounds; }

    /**
     * @brief Retrieves the world chunk bounds.
     * @return The world bounds in chunks.
     */
    ND int getWorldChunkBounds() const { return this->m_worldCoordinateBounds >> 4; }

    /**
     * @brief Retrieves the bounding box representing the world bounds.
     * @return The bounding box of the world bounds.
     */
    ND BoundingBox getWorldBounds() const { return this->m_worldBounds; }
};
