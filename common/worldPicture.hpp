#pragma once

#include <utility>

#include "lce/include/picture.hpp"
#include "common/util.hpp"
#include "terrain/generator.hpp"

/**
 * @brief Generates a file name for a biome image based on the generator's properties.
 *
 * The file name includes the world seed, LCE version, biome scale, and world size.
 *
 * @param g Pointer to the Generator object.
 * @param directory The directory where the file will be saved.
 * @return The generated file name as a string.
 */
inline std::string getBiomeImageFileNameFromGenerator(const Generator *g, const std::string &directory) {
    std::string file = directory + std::to_string(g->getWorldSeed()) + "_" + LceVersionToString(g->getLCEVersion()) +
                       "_" + biomeScaleToString(g->getBiomeScale()) + "_" + worldSizeToString(g->getWorldSize()) +
                       ".png";
    return file;
}

/**
 * @class WorldPicture
 * @brief A class for generating and managing biome images based on a world generator.
 *
 * This class extends the Picture class and provides functionality to draw and save
 * biome images using a Generator object.
 */
class MU WorldPicture : public Picture {
    Generator *g; ///< Pointer to the Generator object.

public:
    /**
     * @brief Constructs a WorldPicture object with a specified width and height.
     *
     * @param g Pointer to the Generator object.
     * @param width The width of the picture.
     * @param height The height of the picture.
     */
    MU WorldPicture(Generator *g, c_int width, c_int height) : Picture(width, height, 3), g(g) {
    }

    /**
     * @brief Constructs a WorldPicture object with dimensions based on the generator's world bounds.
     *
     * @param g Pointer to the Generator object.
     */
    MU explicit WorldPicture(Generator *g) : Picture(
        g->getWorldCoordinateBounds() << 1, g->getWorldCoordinateBounds() << 1, 3), g(g) {
    }

    /**
     * @brief Draws the biomes onto the picture using the generator's biome data.
     *
     * This method initializes biome colors and uses the generator to retrieve biome data,
     * which is then drawn pixel by pixel.
     */
    MU void drawBiomes() const {
        if (m_width == 0) return;
        if (m_height == 0) return;

        unsigned char biomeColors[256][3];
        initBiomeColors(biomeColors);

        if (g->getWorldBiomes() == nullptr) {
            g->generateCache(1);
        }

        for (i32 y = 0; y < m_height; ++y) {
            for (i32 x = 0; x < m_width; ++x) {
                const biome_t id = *g->getCacheAtBlock(1, x - m_width / 2, y - m_height / 2);
                drawPixel(&biomeColors[static_cast<size_t>(id)][0], x, y);
            }
        }
    }

    /**
     * @brief Draws the biomes onto the picture with specified dimensions.
     *
     * This method resizes the picture, initializes biome colors, and uses the generator
     * to retrieve biome data for the specified dimensions, which is then drawn pixel by pixel.
     *
     * @param widthIn The width of the picture.
     * @param heightIn The height of the picture.
     */
    MU void drawBiomesWithSize(c_int widthIn, c_int heightIn) {
        if (widthIn == 0) return;
        if (heightIn == 0) return;

        m_width = widthIn;
        m_height = heightIn;

        allocate(3);

        u8 biomeColors[256][3];
        initBiomeColors(biomeColors);

        c_int x = static_cast<int>(m_width);
        c_int y = static_cast<int>(m_height);
        c_int w = static_cast<int>(m_width);
        c_int h = static_cast<int>(m_height);
        const biome_t *ids = g->getBiomeRange(4, x, y, w, h);

        for (i32 yi = 0; yi < m_height; ++yi) {
            for (i32 xi = 0; xi < m_width; ++xi) {
                const biome_t id = ids[getIndex(xi, yi)];
                drawPixel(&biomeColors[static_cast<size_t>(id)][0], xi, yi);
            }
        }
    }

    /**
     * @brief Saves the biome image to a specified directory.
     *
     * The file name is generated using the generator's properties.
     *
     * @param directory The directory where the image will be saved.
     */
    void save(const std::string &directory) const;
};
