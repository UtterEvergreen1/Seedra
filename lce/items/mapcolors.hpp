#pragma once

#include <cstdint>
#include <vector>


namespace lce::items {

    enum class MAP_RGB : uint8_t {
        NONE,
        GRASS,
        SAND,
        WOOL,
        FIRE,
        ICE,
        METAL,
        PLANT,
        SNOW,
        CLAY,
        DIRT,
        STONE,
        WATER,
        WOOD,
        QUARTZ,
        COLOR_ORANGE,
        COLOR_MAGENTA,
        COLOR_LIGHT_BLUE,
        COLOR_YELLOW,
        COLOR_LIGHT_GREEN,
        COLOR_PINK,
        COLOR_GRAY,
        COLOR_LIGHT_GRAY,
        COLOR_CYAN,
        COLOR_PURPLE,
        COLOR_BLUE,
        COLOR_BROWN,
        COLOR_GREEN,
        COLOR_RED,
        COLOR_BLACK,
        GOLD,
        DIAMOND,
        LAPIS,
        EMERALD,
        PODZOL,
        NETHER,
        TERRACOTTA_WHITE,
        TERRACOTTA_ORANGE,
        TERRACOTTA_MAGENTA,
        TERRACOTTA_LIGHT_BLUE,
        TERRACOTTA_YELLOW,
        TERRACOTTA_LIGHT_GREEN,
        TERRACOTTA_PINK,
        TERRACOTTA_GRAY,
        TERRACOTTA_LIGHT_GRAY,
        TERRACOTTA_CYAN,
        TERRACOTTA_PURPLE,
        TERRACOTTA_BLUE,
        TERRACOTTA_BROWN,
        TERRACOTTA_GREEN,
        TERRACOTTA_RED,
        TERRACOTTA_BLACK
    };


    /*
    class RGB {
    public:
        uint8_t r;
        uint8_t g;
        uint8_t b;

        RGB()
            : r(0), g(0), b(0) {}

        RGB(uint8_t r, uint8_t g, uint8_t b)
            : r(r), g(g), b(b) {}
    };


    inline RGB getRGB(MAP_RGB mapRgb) {
        static std::vector<RGB> mapColors {
            {0, 0, 0},
            {127, 178, 56},
            {247, 233, 163},
            {199, 199, 199},
            {255, 0, 0},
            {160, 160, 255},
            {167, 167, 167},
            {0, 124, 0},
            {255, 255, 255},
            {164, 168, 184},
            {151, 109, 77},
            {112, 112, 112},
            {64, 64, 255},
            {143, 119, 72},
            {255, 252, 245},
            {216, 127, 51},
            {178, 76, 216},
            {102, 153, 216},
            {229, 229, 51},
            {127, 204, 25},
            {242, 127, 165},
            {76, 76, 76},
            {153, 153, 153},
            {76, 127, 153},
            {127, 63, 178},
            {51, 76, 178},
            {102, 76, 51},
            {102, 127, 51},
            {153, 51, 51},
            {25, 25, 25},
            {250, 238, 77},
            {92, 219, 213},
            {74, 128, 255},
            {0, 217, 58},
            {129, 86, 49},
            {112, 2, 0},
            {209, 177, 161},
            {159, 82, 36},
            {149, 87, 108},
            {112, 108, 138},
            {186, 133, 36},
            {103, 117, 53},
            {160, 77, 78},
            {57, 41, 35},
            {135, 107, 98},
            {87, 92, 92},
            {122, 73, 88},
            {76, 62, 92},
            {76, 50, 35},
            {76, 82, 42},
            {142, 60, 46},
            {37, 22, 16},
        };

        return mapColors[mapRgb];
    }

    */

}