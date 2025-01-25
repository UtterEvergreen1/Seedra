#pragma once

class Biome;

class Colors {
public:
    static constexpr int FOLIAGE_BUFFER_SIZE = 65536;

    /**
     * The color modifier to use for grass. Stored in ABGR format.
     */
    static int grassBuffer[FOLIAGE_BUFFER_SIZE];
    static int foliageBuffer[FOLIAGE_BUFFER_SIZE];

    /**
     * Sets the color modifier to use for grass.
     */
    static void setGrassBiomeColorizer(int grassBufferIn[FOLIAGE_BUFFER_SIZE]);

    /**
     * Sets the color modifier to use for foliage.
     */
    static void setFoliageBiomeColorizer(int foliageBufferIn[FOLIAGE_BUFFER_SIZE]);

    /**
     * Gets the color modifier to use for grass.
     */
    static int getGrassColor(double temperature, double humidity);

    static int getFoliageColor(double temperature, double humidity);
};
