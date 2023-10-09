#pragma once

#include "generator.hpp"
#include "util.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "LegacyCubiomes/utils/stb_image_write.h"


std::string getBiomeImageFileNameFromGenerator(Generator* g, const std::string& directory) {
    std::string file = directory + std::to_string(g->getWorldSeed()) + "_" + LceVersionToString(g->getLCEVersion()) +
                       "_" + biomeScaleToString(g->getBiomeScale()) + "_" + worldSizeToString(g->getWorldSize()) + ".png";
    return file;
}


/**
 * Takes a Generator* object and generates all chunk biomes, given
 * it's world size and biome scale. It outputs a .png file at the
 * passed file directory.
 * \n
 * This will auto-create the file name!
 * @param g generator object
 * @param filename DIRECTORY to place the file in
 */
void writeBiomesToImage(Generator* g, const std::string& directory) {
    std::string filename = getBiomeImageFileNameFromGenerator(g, directory);

    unsigned char biomeColors[256][3];
    initBiomeColors(biomeColors);

    auto pair = g->generateAllBiomes();
    int size = pair.first;
    int* ids = pair.second;

    const int width = size;
    const int height = size;
    auto* data = new unsigned char[width * height * 3];

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            int index = x + y * height;
            int id = ids[index];
            std::memcpy(&data[index * 3], &biomeColors[id][0], 3);
        }
    }

    stbi_write_png(filename.c_str(), width, height, 3, data, width * 3);

    delete[] data;

}