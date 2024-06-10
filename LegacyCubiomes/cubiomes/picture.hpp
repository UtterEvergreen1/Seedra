#pragma once

#include <utility>

#include "biomeColor.hpp"
#include "generator.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lce/include/stb_image_write.h"


inline std::string getBiomeImageFileNameFromGenerator(const Generator* g, const std::string& directory) {
    std::string file = directory + std::to_string(g->getWorldSeed()) + "_" + LceVersionToString(g->getLCEVersion()) +
                       "_" + biomeScaleToString(g->getBiomeScale()) + "_" + worldSizeToString(g->getWorldSize()) +
                       ".png";
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
class Picture {
public:
    static constexpr int myRGBSize = 3;
    uint32_t myWidth = 0;
    uint32_t myHeight = 0;
    /**
     * goes left to right, top to bottom I think
     */
    uint8_t* myData;

    void allocate() {
        delete[] myData;
        myData = new uint8_t[myWidth * myHeight * myRGBSize];
        std::memset(myData, 0, myWidth * myHeight * myRGBSize);
    }

    Picture(const int width, const int height) : myWidth(width), myHeight(height) {
        myData = nullptr;
        allocate();
    }

    explicit Picture(const int size) : Picture(size, size) {}

    ND uint32_t getWidth() const { return myWidth; }
    ND uint32_t getHeight() const { return myHeight; }
    ND uint32_t getIndex(const uint32_t x, const uint32_t y) const { return x + y * myHeight; }

    MU void drawPixel(const unsigned char* rgb, const uint32_t x, const uint32_t y) const {
        const uint32_t index = getIndex(x, y);
        std::memcpy(&myData[index * 3], rgb, 3);
    }

    ND bool drawBox(const uint32_t startX, const uint32_t startY, const uint32_t endX, const uint32_t endY,
                    const uint8_t red, const uint8_t green, const uint8_t blue) const {

        if (startX > myWidth || startY > myHeight) return false;
        if (endX > myWidth || endY > myHeight) return false;
        if (endX < startX || endY < startY) return false;

        for (uint32_t x = startX; x < endX; x++) {
            const uint32_t index = getIndex(x, startY) * myRGBSize;
            myData[index] = red;
            myData[index + 1] = green;
            myData[index + 2] = blue;
        }

        const uint32_t rowSize = (endX - startX) * myRGBSize;
        const uint32_t firstRowIndex = getIndex(startX, startY) * myRGBSize;
        for (uint32_t y = startY + 1; y < endY; y++) {
            const uint32_t index = getIndex(startX, y) * myRGBSize;
            std::memcpy(&myData[index], &myData[firstRowIndex], rowSize);
        }
        return true;
    }

    void fillColor(const uint8_t red, const uint8_t green, const uint8_t blue) const {
        for (uint32_t x = 0; x < myWidth; x++) {
            const uint32_t index = getIndex(x, 0) * myRGBSize;
            myData[index] = red;
            myData[index + 1] = green;
            myData[index + 2] = blue;
        }

        const uint32_t rowSize = (myWidth) *myRGBSize;
        for (uint32_t y = 0; y < myHeight; y++) {
            const uint32_t index = getIndex(0, y) * myRGBSize;
            std::memcpy(&myData[index], &myData[0], rowSize);
        }
    }


    void saveWithName(std::string filename, const std::string& directory) const {
        filename = directory + filename;
        stbi_write_png(filename.c_str(), static_cast<int>(myWidth), static_cast<int>(myHeight), myRGBSize, myData,
                       static_cast<int>(myWidth) * myRGBSize);
    }

    ~Picture() { delete[] myData; }
};


class WorldPicture : public Picture {
    Generator* g;

public:
    WorldPicture(Generator* g, const int width, const int height) : Picture(width, height), g(g) {}

    explicit WorldPicture(Generator* g) : Picture(g->getWorldCoordinateBounds() >> 1), g(g) {}

    ~WorldPicture() { delete[] myData; }


    MU void drawBiomes() const {
        if (myWidth == 0) return;
        if (myHeight == 0) return;

        unsigned char biomeColors[256][3];
        initBiomeColors(biomeColors);

        const auto [fst, snd] = g->generateAllBiomes();

        for (int y = 0; y < getHeight(); ++y) {
            for (int x = 0; x < getWidth(); ++x) {
                const int id = snd[getIndex(x, y)];
                drawPixel(&biomeColors[id][0], x, y);
            }
        }
    }

    MU void drawBiomesWithSize(const int widthIn, const int heightIn) {
        if (widthIn == 0) return;
        if (heightIn == 0) return;

        myWidth = widthIn;
        myHeight = heightIn;

        allocate();

        unsigned char biomeColors[256][3];
        initBiomeColors(biomeColors);

        const int x = static_cast<int>(myWidth);
        const int y = static_cast<int>(myHeight);
        const int w = static_cast<int>(myWidth);
        const int h = static_cast<int>(myHeight);
        const int* ids = g->getBiomeRange(4, x, y, w, h);

        for (int yi = 0; yi < getHeight(); ++yi) {
            for (int xi = 0; xi < getWidth(); ++xi) {
                const int id = ids[getIndex(xi, yi)];
                drawPixel(&biomeColors[id][0], xi, yi);
            }
        }
    }

    void save(const std::string& directory) const {
        const std::string filename = getBiomeImageFileNameFromGenerator(g, directory);
        stbi_write_png(filename.c_str(), static_cast<int>(myWidth), static_cast<int>(myHeight), myRGBSize, myData,
                       static_cast<int>(myWidth) * myRGBSize);
    }
};
