#include "terrain/biomes/biome.hpp"
float getMinTemperature(const Biome* biome, int y) {
    constexpr double noiseVal = -0.707106781186548; // -sqrt(2/4)
    constexpr double f = noiseVal * 4.0;
    return biome->m_temperature - (f + y - 64.0F) * 0.05F / 30.0F;
}

float getMaxTemperature(const Biome* biome, int y) {
    constexpr double noiseVal = 0.707106781186548; // sqrt(2/4)
    constexpr double f = noiseVal * 4.0;
    return biome->m_temperature - (f + y - 64.0F) * 0.05F / 30.0F;
}

struct MinMaxTemp {
    std::string biomeName;
    int minY = 0;
    int maxY = 255;
    bool hasSnow = false;
    bool alwaysSnow = false;
};

int main() {
    Biome::registerBiomes();
    std::vector<MinMaxTemp> minMaxTemps(Biome::registry.size());
    /*for (auto& biome : Biome::registry) {
        std::cout << biome.second->biomeName << ": " << getMinTemperature(biome.second, 255) << std::endl;
    }*/

    //std::cout << std::endl << "Always valid snow biomes:" << std::endl;
    int biomeCount = 0;
    for (auto* biome : Biome::registry) {
        int yMin = 0;
        int yMax = 255;
        std::string biomeString = biome->m_biomeName;
        std::ranges::transform(biomeString, biomeString.begin(), ::toupper);
        std::ranges::replace(biomeString, ' ', '_');
        for (int i = 255; i >= 0; i--) {
            float temp = getMaxTemperature(biome, i);
            if (temp >= 0.15F) {
                // Upper case all letters and replace spaces with underscores
                //std::cout << biomeString << " y <= " << i << std::endl;
                yMin = i;
                break;
            }
        }

        for (int i = 0; i < 256; i++) {
            float temp = getMinTemperature(biome, i);
            if (temp < 0.15F) {
                //std::cout << biomeString << " y >= " << i << std::endl;
                yMax = i;
                break;
            }
        }

        minMaxTemps[biomeCount++] = {biomeString, yMin, yMax, yMin != 255, yMax == 0};
    }

    std::cout << "Biomes with possible snow:" << std::endl;
    for (auto& minMaxTemp : minMaxTemps) {
        if (minMaxTemp.hasSnow) {
            if (!minMaxTemp.alwaysSnow) {
                std::cout << minMaxTemp.minY << " <= y <= " << minMaxTemp.maxY << " " << minMaxTemp.biomeName << std::endl;
            }
        }
    }
    std::cout << std::endl << "Biomes that always have snow:" << std::endl;
    for (auto& minMaxTemp : minMaxTemps) {
        if (minMaxTemp.alwaysSnow) {
            std::cout << minMaxTemp.biomeName << std::endl;
        }
    }
    return 0;
}