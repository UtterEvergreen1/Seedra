#pragma once

#include <utility>

#include "lce/include/picture.hpp"
#include "common/util.hpp"
#include "terrain/generator.hpp"



inline std::string getBiomeImageFileNameFromGenerator(const Generator* g, const std::string& directory) {
    std::string file = directory + std::to_string(g->getWorldSeed()) + "_" + LceVersionToString(g->getLCEVersion()) +
                       "_" + biomeScaleToString(g->getBiomeScale()) + "_" + worldSizeToString(g->getWorldSize()) +
                       ".png";
    return file;
}


class MU WorldPicture : public Picture {
    Generator* g;

public:
    MU WorldPicture(Generator* g, c_int width, c_int height) : Picture(width, height), g(g) {}

    MU explicit WorldPicture(Generator* g) : Picture(g->getWorldCoordinateBounds() >> 1), g(g) {}

    MU void drawBiomes() const {
        if (myWidth == 0) return;
        if (myHeight == 0) return;

        unsigned char biomeColors[256][3];
        initBiomeColors(biomeColors);

        c_auto [fst, snd] = g->generateAllBiomes();

        for (u32 y = 0; y < getHeight(); ++y) {
            for (u32 x = 0; x < getWidth(); ++x) {
                c_int id = snd[getIndex(x, y)];
                drawPixel(&biomeColors[id][0], x, y);
            }
        }
    }

    MU void drawBiomesWithSize(c_int widthIn, c_int heightIn) {
        if (widthIn == 0) return;
        if (heightIn == 0) return;

        myWidth = widthIn;
        myHeight = heightIn;

        allocate(3);

        u8 biomeColors[256][3];
        initBiomeColors(biomeColors);

        c_int x = static_cast<int>(myWidth);
        c_int y = static_cast<int>(myHeight);
        c_int w = static_cast<int>(myWidth);
        c_int h = static_cast<int>(myHeight);
        c_int* ids = g->getBiomeRange(4, x, y, w, h);

        for (u32 yi = 0; yi < getHeight(); ++yi) {
            for (u32 xi = 0; xi < getWidth(); ++xi) {
                c_int id = ids[getIndex(xi, yi)];
                drawPixel(&biomeColors[id][0], xi, yi);
            }
        }
    }

    void save(const std::string& directory) const;
};
