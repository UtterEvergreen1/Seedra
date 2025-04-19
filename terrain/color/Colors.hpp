#pragma once

/**
 * @class Biome
 * @brief Forward declaration of the Biome class.
 */
class Biome;

/**
 * @class Colors
 * @brief Provides utilities for managing biome color modifiers for grass and foliage.
 */
class Colors {
public:
    /**
     * @brief The size of the buffer used for storing foliage and grass color modifiers.
     *
     * This constant defines the maximum number of color entries that can be stored
     * in the grass and foliage buffers.
     */
    static constexpr int FOLIAGE_BUFFER_SIZE = 65536;

    /**
     * @brief Buffer for storing grass color modifiers.
     *
     * The grass color modifiers are stored in ABGR format and are used to adjust
     * the appearance of grass in different biomes.
     */
    static int grassBuffer[FOLIAGE_BUFFER_SIZE];

    /**
     * @brief Buffer for storing foliage color modifiers.
     *
     * The foliage color modifiers are stored in ABGR format and are used to adjust
     * the appearance of foliage in different biomes.
     */
    static int foliageBuffer[FOLIAGE_BUFFER_SIZE];

    /**
     * @brief Sets the color modifier buffer for grass.
     *
     * This function initializes the grass color buffer with the provided values.
     *
     * @param grassBufferIn An array containing the grass color modifiers to set.
     */
    static void setGrassBiomeColorizer(int grassBufferIn[FOLIAGE_BUFFER_SIZE]);

    /**
     * @brief Sets the color modifier buffer for foliage.
     *
     * This function initializes the foliage color buffer with the provided values.
     *
     * @param foliageBufferIn An array containing the foliage color modifiers to set.
     */
    static void setFoliageBiomeColorizer(int foliageBufferIn[FOLIAGE_BUFFER_SIZE]);

    /**
     * @brief Retrieves the grass color modifier based on temperature and humidity.
     *
     * This function calculates the grass color modifier using the given temperature
     * and humidity values.
     *
     * @param temperature The temperature value (0.0 to 1.0).
     * @param humidity The humidity value (0.0 to 1.0).
     * @return The grass color modifier in ABGR format.
     */
    static int getGrassColor(double temperature, double humidity);

    /**
     * @brief Retrieves the foliage color modifier based on temperature and humidity.
     *
     * This function calculates the foliage color modifier using the given temperature
     * and humidity values.
     *
     * @param temperature The temperature value (0.0 to 1.0).
     * @param humidity The humidity value (0.0 to 1.0).
     * @return The foliage color modifier in ABGR format.
     */
    static int getFoliageColor(double temperature, double humidity);
};
