#pragma once

#include <utility>

#include "biomeColor.hpp"
#include "generator.hpp"

#include "lce/include/picture.hpp"


inline std::string getBiomeImageFileNameFromGenerator(const Generator* g, const std::string& directory) {
    std::string file = directory + std::to_string(g->getWorldSeed()) + "_" + LceVersionToString(g->getLCEVersion()) +
                       "_" + biomeScaleToString(g->getBiomeScale()) + "_" + worldSizeToString(g->getWorldSize()) +
                       ".png";
    return file;
}


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

        allocate(3);

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

    void save(const std::string& directory) const;
};
