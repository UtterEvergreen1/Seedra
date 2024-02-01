#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <cstdio>
#include "LegacyCubiomes/cubiomes/biomeID.hpp"

static void setColor(unsigned char colors[256][3], int id, uint32_t hex)
{
    colors[id][0] = (hex >> 16) & 0xff;
    colors[id][1] = (hex >>  8) & 0xff;
    colors[id][2] = (hex >>  0) & 0xff;
}

static void initBiomeColors(unsigned char colors[256][3])
{
    // This coloring scheme is largely inspired by the AMIDST program:
    // https://github.com/toolbox4minecraft/amidst/wiki/Biome-Color-Table
    // but with additional biomes for 1.18+, and with some subtle changes to
    // improve contrast for the new world generation.

    memset(colors, 0, 256*3);

    setColor(colors, ocean,                            0x000070);
    setColor(colors, plains,                           0x8db360);
    setColor(colors, desert,                           0xfa9418);
    setColor(colors, mountains,                        0x606060);
    setColor(colors, forest,                           0x056621);
    setColor(colors, taiga,                            0x0b6659);
    setColor(colors, swamp,                            0x07f9b2);
    setColor(colors, river,                            0x0000ff);
    setColor(colors, nether_wastes,                    0xbf3b3b);
    setColor(colors, the_end,                          0x8080ff);
    setColor(colors, frozen_ocean,                     0x7070d6);
    setColor(colors, frozen_river,                     0xa0a0ff);
    setColor(colors, snowy_tundra,                     0xffffff);
    setColor(colors, snowy_mountains,                  0xa0a0a0);
    setColor(colors, mushroom_fields,                  0xff00ff);
    setColor(colors, mushroom_field_shore,             0xa000ff);
    setColor(colors, beach,                            0xfade55);
    setColor(colors, desert_hills,                     0xd25f12);
    setColor(colors, wooded_hills,                     0x22551c);
    setColor(colors, taiga_hills,                      0x163933);
    setColor(colors, mountain_edge,                    0x72789a);
    setColor(colors, jungle,                           0x537b09);
    setColor(colors, jungle_hills,                     0x2c4205);
    setColor(colors, deep_ocean,                       0x000030);
    setColor(colors, stone_shore,                      0xa2a284);
    setColor(colors, snowy_beach,                      0xfaf0c0);
    setColor(colors, birch_forest,                     0x307444);
    setColor(colors, birch_forest_hills,               0x1f5f32);
    setColor(colors, dark_forest,                      0x40511a);
    setColor(colors, snowy_taiga,                      0x31554a);
    setColor(colors, snowy_taiga_hills,                0x243f36);
    setColor(colors, giant_tree_taiga,                 0x596651);
    setColor(colors, giant_tree_taiga_hills,           0x454f3e);
    setColor(colors, wooded_mountains,                 0x507050);
    setColor(colors, savanna,                          0xbdb25f);
    setColor(colors, savanna_plateau,                  0xa79d64);
    setColor(colors, badlands,                         0xd94515);
    setColor(colors, wooded_badlands_plateau,          0xb09765);
    setColor(colors, badlands_plateau,                 0xca8c65);
    setColor(colors, small_end_islands,                0x4b4bab);
    setColor(colors, end_midlands,                     0xc9c959);
    setColor(colors, end_highlands,                    0xb5b536);
    setColor(colors, end_barrens,                      0x7070cc);
    setColor(colors, warm_ocean,                       0x0000ac);
    setColor(colors, lukewarm_ocean,                   0x000090);
    setColor(colors, cold_ocean,                       0x202070);
    setColor(colors, deep_warm_ocean,                  0x000050);
    setColor(colors, deep_lukewarm_ocean,              0x000040);
    setColor(colors, deep_cold_ocean,                  0x202038);
    setColor(colors, deep_frozen_ocean,                0x404090);
    setColor(colors, the_void,                         0x000000);
    setColor(colors, sunflower_plains,                 0xb5db88);
    setColor(colors, desert_lakes,                     0xffbc40);
    setColor(colors, gravelly_mountains,               0x888888);
    setColor(colors, flower_forest,                    0x2d8e49);
    setColor(colors, taiga_mountains,                  0x338e81);
    setColor(colors, swamp_hills,                      0x2fffda);
    setColor(colors, ice_spikes,                       0xb4dcdc);
    setColor(colors, modified_jungle,                  0x7ba331);
    setColor(colors, modified_jungle_edge,             0x8ab33f);
    setColor(colors, tall_birch_forest,                0x589c6c);
    setColor(colors, tall_birch_hills,                 0x47875a);
    setColor(colors, dark_forest_hills,                0x687942);
    setColor(colors, snowy_taiga_mountains,            0x597d72);
    setColor(colors, giant_spruce_taiga,               0x818e79);
    setColor(colors, giant_spruce_taiga_hills,         0x6d7766);
    setColor(colors, modified_gravelly_mountains,      0x789878);
    setColor(colors, shattered_savanna,                0xe5da87);
    setColor(colors, shattered_savanna_plateau,        0xcfc58c);
    setColor(colors, eroded_badlands,                  0xff6d3d);
    setColor(colors, modified_wooded_badlands_plateau, 0xd8bf8d);
    setColor(colors, modified_badlands_plateau,        0xf2b48d);
    setColor(colors, bamboo_jungle,                    0x768e14);
    setColor(colors, bamboo_jungle_hills,              0x3b470a);
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

static int biomesToImage(unsigned char *pixels,
        unsigned char biomeColors[256][3], const int *biomes,
        const unsigned int sx, const unsigned int sy,
        const unsigned int pixscale, const int flip)
{
    unsigned int i, j;
    int containsInvalidBiomes = 0;

    for (j = 0; j < sy; j++)
    {
        for (i = 0; i < sx; i++)
        {
            int id = biomes[j*sx+i];
            unsigned int r, g, b;

            if (id < 0 || id >= 256)
            {
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

            unsigned int m, n;
            for (m = 0; m < pixscale; m++) {
                for (n = 0; n < pixscale; n++) {
                    int idx = pixscale * i + n;
                    if (flip)
                        idx += (sx * pixscale) * ((pixscale * j) + m);
                    else
                        idx += (sx * pixscale) * ((pixscale * (sy-1-j)) + m);

                    unsigned char *pix = pixels + 3*idx;
                    pix[0] = (unsigned char)r;
                    pix[1] = (unsigned char)g;
                    pix[2] = (unsigned char)b;
                }
            }
        }
    }

    return containsInvalidBiomes;
}

static int savePPM(const char *path, const unsigned char *pixels, const unsigned int sx, const unsigned int sy)
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


