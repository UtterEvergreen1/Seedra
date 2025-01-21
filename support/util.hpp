#include "lce/processor.hpp"
#include "terrain/biomes/biomeID.hpp"
#include <cstdint>
#include <cstdio>
#include <stdlib.h>
#include <string.h>

static void setColor(unsigned char colors[256][3], int id, u32 hex) {
    colors[id][0] = (hex >> 16) & 0xff;
    colors[id][1] = (hex >>  8) & 0xff;
    colors[id][2] = (hex >>  0) & 0xff;
}

static void initBiomeColors(u8 colors[256][3]) {
    // This coloring scheme is largely inspired by the AMIDST program:
    // https://github.com/toolbox4minecraft/amidst/wiki/Biome-Color-Table
    // but with additional biomes for 1.18+, and with some subtle changes to
    // improve contrast for the new world generation.

    memset(colors, 0, 256*3);

    setColor(colors, ocean,                            0x0b0b73);
    setColor(colors, plains,                           0x86b359);
    setColor(colors, desert,                           0xf28518);
    setColor(colors, mountains,                        0x595959);
    setColor(colors, forest,                           0x0a611b);
    setColor(colors, taiga,                            0x0f615a);
    setColor(colors, swamp,                            0x17e6a1);
    setColor(colors, river,                            0x0c0cf2);
    setColor(colors, nether_wastes,                    0xbf4439);
    setColor(colors, the_end,                          0x8080ff);
    setColor(colors, frozen_ocean,                     0x6c75d9);
    setColor(colors, frozen_river,                     0xa6adff);
    setColor(colors, snowy_tundra,                     0xffffff);
    setColor(colors, snowy_mountains,                  0xb3b3b3);
    setColor(colors, mushroom_fields,                  0xd520e6);
    setColor(colors, mushroom_field_shore,             0xa420e6);
    setColor(colors, beach,                            0xe6d145);
    setColor(colors, desert_hills,                     0xd1621d);
    setColor(colors, wooded_hills,                     0x234d1f);
    setColor(colors, taiga_hills,                      0x164038);
    setColor(colors, mountain_edge,                    0x737c99);
    setColor(colors, jungle,                           0x4f7311);
    setColor(colors, jungle_hills,                     0x2a400a);
    setColor(colors, jungle_edge,                      0x6c8c1c);
    setColor(colors, deep_ocean,                       0x000538);
    setColor(colors, stone_shore,                      0x9e9b77);
    setColor(colors, snowy_beach,                      0xe6d9a5);
    setColor(colors, birch_forest,                     0x28733b);
    setColor(colors, birch_forest_hills,               0x1f5932);
    setColor(colors, dark_forest,                      0x3f591b);
    setColor(colors, snowy_taiga,                      0x365950);
    setColor(colors, snowy_taiga_hills,                0x234034);
    setColor(colors, giant_tree_taiga,                 0x586b50);
    setColor(colors, giant_tree_taiga_hills,           0x4e5943);
    setColor(colors, wooded_mountains,                 0x53734e);
    setColor(colors, savanna,                          0xb3b06b);
    setColor(colors, savanna_plateau,                  0xad965f);
    setColor(colors, badlands,                         0xb33212);
    setColor(colors, wooded_badlands_plateau,          0xa69563);
    setColor(colors, badlands_plateau,                 0xb37459);
    setColor(colors, small_end_islands,                0x4b4bab);
    setColor(colors, end_midlands,                     0xc9c959);
    setColor(colors, end_highlands,                    0xb5b536);
    setColor(colors, end_barrens,                      0x7070cc);
    setColor(colors, warm_ocean,                       0x0808a6);
    setColor(colors, lukewarm_ocean,                   0x07128c);
    setColor(colors, cold_ocean,                       0x223073);
    setColor(colors, deep_warm_ocean,                  0x040452);
    setColor(colors, deep_lukewarm_ocean,              0x030840);
    setColor(colors, deep_cold_ocean,                  0x232840);
    setColor(colors, deep_frozen_ocean,                0x404280);
    setColor(colors, the_void,                         0x000000);
    setColor(colors, sunflower_plains,                 0xb5d982);
    setColor(colors, desert_lakes,                     0xe69e39);
    setColor(colors, gravelly_mountains,               0x808080);
    setColor(colors, flower_forest,                    0x2a8c3e);
    setColor(colors, taiga_mountains,                  0x328f79);
    setColor(colors, swamp_hills,                      0x2ee6cd);
    setColor(colors, ice_spikes,                       0xb3e6e1);
    setColor(colors, modified_jungle,                  0x81a632);
    setColor(colors, modified_jungle_edge,             0x8eb33f);
    setColor(colors, tall_birch_forest,                0x549965);
    setColor(colors, tall_birch_hills,                 0x408050);
    setColor(colors, dark_forest_hills,                0x5d733f);
    setColor(colors, snowy_taiga_mountains,            0x598072);
    setColor(colors, giant_spruce_taiga,               0x808c77);
    setColor(colors, giant_spruce_taiga_hills,         0x697365);
    setColor(colors, modified_gravelly_mountains,      0x7a997b);
    setColor(colors, shattered_savanna,                0xd1ca7d);
    setColor(colors, shattered_savanna_plateau,        0xbfba86);
    setColor(colors, eroded_badlands,                  0xe65c39);
    setColor(colors, modified_wooded_badlands_plateau, 0xd1b482);
    setColor(colors, modified_badlands_plateau,        0xd99877);
    setColor(colors, bamboo_jungle,                    0x64800f);
    setColor(colors, bamboo_jungle_hills,              0x3c4d0b);
}

static void initBiomeTypeColors(unsigned char colors[256][3])
{
    memset(colors, 0, 256*3);

    setColor(colors, Oceanic,  0x0000a0);
    setColor(colors, Warm,     0xffc000);
    setColor(colors, Lush,     0x00a000);
    setColor(colors, Cold,     0x606060);
    setColor(colors, Freezing, 0xffffff);
}

static int biomesToImage(u8 *pixels, u8 biomeColors[256][3], c_int *biomes,
                         c_u32 sx, c_u32 sy, c_u32 pixScale) {
    u32 i, j;
    int containsInvalidBiomes = 0;

    for (j = 0; j < sy; j++) {
        for (i = 0; i < sx; i++) {
            int id = biomes[j*sx+i];
            u32 r, g, b;

            if (id < 0 || id >= 256) {
                // This may happen for some intermediate layers
                containsInvalidBiomes = 1;
                r = biomeColors[id&0x7f][0]-40; r = (r>0xff) ? 0x00 : r&0xff;
                g = biomeColors[id&0x7f][1]-40; g = (g>0xff) ? 0x00 : g&0xff;
                b = biomeColors[id&0x7f][2]-40; b = (b>0xff) ? 0x00 : b&0xff;
            }
            else
            {
                r = biomeColors[id][0];
                g = biomeColors[id][1];
                b = biomeColors[id][2];
            }

            u32 m, n;
            for (m = 0; m < pixScale; m++) {
                for (n = 0; n < pixScale; n++) {
                    const int idx = pixScale * i + n + (pixScale * sx) * (pixScale * j + m);

                    unsigned char *pix = pixels + 3*idx;
                    pix[0] = (u8)r;
                    pix[1] = (u8)g;
                    pix[2] = (u8)b;
                }
            }
        }
    }

    return containsInvalidBiomes;
}

static int savePPM(const char *path, c_u8 *pixels, c_u32 sx, c_u32 sy)
{
    FILE *fp = fopen(path, "wb");
    if (!fp)
        return -1;
    fprintf(fp, "P6\n%d %d\n255\n", sx, sy);
    size_t pixelsLen = 3 * sx * sy;
    size_t written = fwrite(pixels, sizeof pixels[0], pixelsLen, fp);
    fclose(fp);
    return written != pixelsLen;
}


