#pragma once

#include <utility>

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
 int lol = 0;


class Picture {
private:
    static constexpr int RGB_SIZE = 3;
    uint32_t width = 0;
    uint32_t height = 0;
    std::string filename;
    Generator *g;

public:
    /**
     * goes left to right, top to bottom I think
     */
    uint8_t* data;

    Picture(Generator *g, int width, int height, const std::string& directory)
        : g(g), width(width), height(height) {
        data = new uint8_t[width * height * RGB_SIZE];
        filename = getBiomeImageFileNameFromGenerator(g, directory);
    }

    explicit Picture(Generator *g, int size, const std::string& directory)
        : Picture(g, size, size, directory) {}

    Picture(Generator *g, const std::string& directory) : g(g) {
        int size = g->getWorldCoordinateBounds() >> 1;
        width = size;
        height = size;
        data = new uint8_t[width * height * RGB_SIZE];
        filename = getBiomeImageFileNameFromGenerator(g, directory);
    }

    ND uint32_t getWidth() const { return width; }
    ND uint32_t getHeight() const { return height; }
    ND uint32_t getIndex(uint32_t x, uint32_t y) const { return x + y * height; }

    MU void drawPixel(unsigned char* rgb, uint32_t x, uint32_t y) const {
        uint32_t index = getIndex(x, y);
        std::memcpy(&data[index * 3], rgb, 3);
    }

    bool drawBox(uint32_t startX, uint32_t startY,
                    uint32_t endX, uint32_t endY,
                    uint8_t red, uint8_t green, uint8_t blue) const {

        if (startX > width || startY > height) return false;
        if (endX > width || endY > height) return false;
        if (endX < startX || endY < startY) return false;

        for (uint32_t x = startX; x < endX; x++) {
            uint32_t index = getIndex(x, startY) * RGB_SIZE;
            data[index] = red;
            data[index + 1] = green;
            data[index + 2] = blue;
        }

        uint32_t rowSize = (endX - startX) * RGB_SIZE;
        uint32_t firstRowIndex = getIndex(startX, startY) * RGB_SIZE;
        for (uint32_t y = startY + 1; y < endY; y++) {
            uint32_t index = getIndex(startX, y) * RGB_SIZE;
            std::memcpy(&data[index], &data[firstRowIndex], rowSize);
        }
        return true;
    }


    MU void drawBiomes() {
        if (width == 0) return;
        if (height == 0) return;

        unsigned char biomeColors[256][3];
        initBiomeColors(biomeColors);

        auto pair = g->generateAllBiomes();

        for(int y = 0; y < getHeight(); ++y) {
            for(int x = 0; x < getWidth(); ++x) {
                int id = pair.second[getIndex(x, y)];
                drawPixel(&biomeColors[id][0], x, y);
            }
        }
    }

    void save() const {
        stbi_write_png(filename.c_str(), (int)width, (int)height,
                       RGB_SIZE, data, (int)width * RGB_SIZE);
    }

    ~Picture() {
        free(data);
    }
};

