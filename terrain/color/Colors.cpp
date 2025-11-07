#include "Colors.hpp"
#include "terrain/biomes/biome.hpp"

int Colors::grassBuffer[FOLIAGE_BUFFER_SIZE]{};
int Colors::foliageBuffer[FOLIAGE_BUFFER_SIZE]{};

void Colors::setGrassBiomeColorizer(int grassBufferIn[FOLIAGE_BUFFER_SIZE]) {
    memcpy(grassBuffer, grassBufferIn, FOLIAGE_BUFFER_SIZE * sizeof(int));
}

void Colors::setFoliageBiomeColorizer(int foliageBufferIn[FOLIAGE_BUFFER_SIZE]) {
    memcpy(foliageBuffer, foliageBufferIn, FOLIAGE_BUFFER_SIZE * sizeof(int));
}

int Colors::getGrassColor(double temperature, double humidity) {
    int bufferIndex = (int) (-(humidity * temperature - 1.0) * 255.0) << 8
                      | (int) ((1.0 - temperature) * 255.0);
    int grassColor = 0xFFFF00FF; // Default magenta color for out-of-bounds
    if (bufferIndex >= 0 && bufferIndex < FOLIAGE_BUFFER_SIZE) {
        grassColor = grassBuffer[bufferIndex];
    }
    return grassColor;
}

int Colors::getFoliageColor(double temperature, double humidity) {
    humidity = humidity * temperature;
    int i = (int) ((1.0 - temperature) * 255.0);
    int j = (int) ((1.0 - humidity) * 255.0);
    return foliageBuffer[j << 8 | i];
}
