#pragma once

#include "lce/processor.hpp"
#include "terrain/biomes/biome_t.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/**
 * @brief Sets the RGB color for a specific biome ID.
 *
 * @param colors A 2D array of RGB colors.
 * @param id The biome ID to set the color for.
 * @param hex The color in hexadecimal format (e.g., 0xRRGGBB).
 */
MU static void setColor(unsigned char colors[256][3], const biome_t id, c_u32 hex) {
    colors[static_cast<biome_u>(id)][0] = (hex >> 16) & 0xff;
    colors[static_cast<biome_u>(id)][1] = (hex >> 8) & 0xff;
    colors[static_cast<biome_u>(id)][2] = (hex >> 0) & 0xff;
}

MU static void setColor(unsigned char colors[256][3], const biome_category_t id, c_u32 hex) {
    colors[static_cast<biome_u>(id)][0] = (hex >> 16) & 0xff;
    colors[static_cast<biome_u>(id)][1] = (hex >> 8) & 0xff;
    colors[static_cast<biome_u>(id)][2] = (hex >> 0) & 0xff;
}

/**
 * @brief Initializes the biome colors with predefined values.
 *
 * This function sets up a color scheme for various biomes, inspired by the AMIDST program,
 * with additional biomes for Minecraft 1.18+ and improved contrast for new world generation.
 *
 * @param colors A 2D array to store the RGB colors for each biome.
 */
MU static void initBiomeColors(u8 colors[256][3]) {
    memset(colors, 0, 256 * 3);

    // Set colors for various biomes
    setColor(colors, biome_t::ocean, 0x0b0b73);
    setColor(colors, biome_t::plains, 0x86b359);
    setColor(colors, biome_t::desert, 0xf28518);
    setColor(colors, biome_t::mountains, 0x595959);
    setColor(colors, biome_t::forest, 0x0a611b);
    setColor(colors, biome_t::taiga, 0x0f615a);
    setColor(colors, biome_t::swamp, 0x17e6a1);
    setColor(colors, biome_t::river, 0x0c0cf2);
    setColor(colors, biome_t::nether_wastes, 0xbf4439);
    setColor(colors, biome_t::the_end, 0x8080ff);
    setColor(colors, biome_t::frozen_ocean, 0x6c75d9);
    setColor(colors, biome_t::frozen_river, 0xa6adff);
    setColor(colors, biome_t::snowy_tundra, 0xffffff);
    setColor(colors, biome_t::snowy_mountains, 0xb3b3b3);
    setColor(colors, biome_t::mushroom_fields, 0xd520e6);
    setColor(colors, biome_t::mushroom_field_shore, 0xa420e6);
    setColor(colors, biome_t::beach, 0xe6d145);
    setColor(colors, biome_t::desert_hills, 0xd1621d);
    setColor(colors, biome_t::wooded_hills, 0x234d1f);
    setColor(colors, biome_t::taiga_hills, 0x164038);
    setColor(colors, biome_t::mountain_edge, 0x737c99);
    setColor(colors, biome_t::jungle, 0x4f7311);
    setColor(colors, biome_t::jungle_hills, 0x2a400a);
    setColor(colors, biome_t::jungle_edge, 0x6c8c1c);
    setColor(colors, biome_t::deep_ocean, 0x000538);
    setColor(colors, biome_t::stone_shore, 0x9e9b77);
    setColor(colors, biome_t::snowy_beach, 0xe6d9a5);
    setColor(colors, biome_t::birch_forest, 0x28733b);
    setColor(colors, biome_t::birch_forest_hills, 0x1f5932);
    setColor(colors, biome_t::dark_forest, 0x3f591b);
    setColor(colors, biome_t::snowy_taiga, 0x365950);
    setColor(colors, biome_t::snowy_taiga_hills, 0x234034);
    setColor(colors, biome_t::giant_tree_taiga, 0x586b50);
    setColor(colors, biome_t::giant_tree_taiga_hills, 0x4e5943);
    setColor(colors, biome_t::wooded_mountains, 0x53734e);
    setColor(colors, biome_t::savanna, 0xb3b06b);
    setColor(colors, biome_t::savanna_plateau, 0xad965f);
    setColor(colors, biome_t::badlands, 0xb33212);
    setColor(colors, biome_t::wooded_badlands_plateau, 0xa69563);
    setColor(colors, biome_t::badlands_plateau, 0xb37459);
    setColor(colors, biome_t::small_end_islands, 0x4b4bab);
    setColor(colors, biome_t::end_midlands, 0xc9c959);
    setColor(colors, biome_t::end_highlands, 0xb5b536);
    setColor(colors, biome_t::end_barrens, 0x7070cc);
    setColor(colors, biome_t::warm_ocean, 0x0808a6);
    setColor(colors, biome_t::lukewarm_ocean, 0x07128c);
    setColor(colors, biome_t::cold_ocean, 0x223073);
    setColor(colors, biome_t::deep_warm_ocean, 0x040452);
    setColor(colors, biome_t::deep_lukewarm_ocean, 0x030840);
    setColor(colors, biome_t::deep_cold_ocean, 0x232840);
    setColor(colors, biome_t::deep_frozen_ocean, 0x404280);
    setColor(colors, biome_t::the_void, 0x000000);
    setColor(colors, biome_t::sunflower_plains, 0xb5d982);
    setColor(colors, biome_t::desert_lakes, 0xe69e39);
    setColor(colors, biome_t::gravelly_mountains, 0x808080);
    setColor(colors, biome_t::flower_forest, 0x2a8c3e);
    setColor(colors, biome_t::taiga_mountains, 0x328f79);
    setColor(colors, biome_t::swamp_hills, 0x2ee6cd);
    setColor(colors, biome_t::ice_spikes, 0xb3e6e1);
    setColor(colors, biome_t::modified_jungle, 0x81a632);
    setColor(colors, biome_t::modified_jungle_edge, 0x8eb33f);
    setColor(colors, biome_t::tall_birch_forest, 0x549965);
    setColor(colors, biome_t::tall_birch_hills, 0x408050);
    setColor(colors, biome_t::dark_forest_hills, 0x5d733f);
    setColor(colors, biome_t::snowy_taiga_mountains, 0x598072);
    setColor(colors, biome_t::giant_spruce_taiga, 0x808c77);
    setColor(colors, biome_t::giant_spruce_taiga_hills, 0x697365);
    setColor(colors, biome_t::modified_gravelly_mountains, 0x7a997b);
    setColor(colors, biome_t::shattered_savanna, 0xd1ca7d);
    setColor(colors, biome_t::shattered_savanna_plateau, 0xbfba86);
    setColor(colors, biome_t::eroded_badlands, 0xe65c39);
    setColor(colors, biome_t::modified_wooded_badlands_plateau, 0xd1b482);
    setColor(colors, biome_t::modified_badlands_plateau, 0xd99877);
    setColor(colors, biome_t::bamboo_jungle, 0x64800f);
    setColor(colors, biome_t::bamboo_jungle_hills, 0x3c4d0b);
}

/**
 * @brief Initializes biome type colors with predefined values.
 *
 * @param colors A 2D array to store the RGB colors for each biome type.
 */
MU static void initBiomeTypeColors(unsigned char colors[256][3]) {
    memset(colors, 0, 256 * 3);

    setColor(colors, biome_category_t::oceanic, 0x0000a0);
    setColor(colors, biome_category_t::warm, 0xffc000);
    setColor(colors, biome_category_t::lush, 0x00a000);
    setColor(colors, biome_category_t::cold, 0x606060);
    setColor(colors, biome_category_t::freezing, 0xffffff);
}

/**
 * @brief Converts biome data into an image representation.
 *
 * @param pixels The output pixel array.
 * @param biomeColors The RGB color array for biomes.
 * @param biomes The biome data array.
 * @param sx The width of the biome data.
 * @param sy The height of the biome data.
 * @param pixScale The scaling factor for each pixel.
 * @return 1 if invalid biomes are encountered, 0 otherwise.
 */
MU static int biomesToImage(u8 *pixels, u8 biomeColors[256][3], c_int *biomes, c_u32 sx, c_u32 sy, c_u32 pixScale) {
    int containsInvalidBiomes = 0;

    for (size_t j = 0; j < sy; j++) {
        for (size_t i = 0; i < sx; i++) {
            const int id = biomes[j * sx + i];
            u32 r, g, b;

            if (id < 0 || id >= 256) {
                // Handle invalid biome IDs
                containsInvalidBiomes = 1;
                r = biomeColors[id & 0x7f][0] - 40;
                r = (r > 0xff) ? 0x00 : r & 0xff;
                g = biomeColors[id & 0x7f][1] - 40;
                g = (g > 0xff) ? 0x00 : g & 0xff;
                b = biomeColors[id & 0x7f][2] - 40;
                b = (b > 0xff) ? 0x00 : b & 0xff;
            } else {
                r = biomeColors[id][0];
                g = biomeColors[id][1];
                b = biomeColors[id][2];
            }

            for (u32 m = 0; m < pixScale; m++) {
                for (u32 n = 0; n < pixScale; n++) {
                    const int idx = static_cast<int>(pixScale) * i + n + (pixScale * sx) * (pixScale * j + m);

                    unsigned char *pix = pixels + 3 * idx;
                    pix[0] = static_cast<u8>(r);
                    pix[1] = static_cast<u8>(g);
                    pix[2] = static_cast<u8>(b);
                }
            }
        }
    }

    return containsInvalidBiomes;
}

/**
 * @brief Saves a PPM image to a file.
 *
 * @param path The file path to save the image.
 * @param pixels The pixel data array.
 * @param sx The width of the image.
 * @param sy The height of the image.
 * @return 0 on success, -1 on failure.
 */
MU static int savePPM(const char *path, c_u8 *pixels, c_u32 sx, c_u32 sy) {
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "P6\n%d %d\n255\n", static_cast<i32>(sx), static_cast<i32>(sy));
    const size_t pixelsLen = 3 * sx * sy;
    const size_t written = fwrite(pixels, sizeof pixels[0], pixelsLen, fp);
    fclose(fp);
    return written != pixelsLen;
}
