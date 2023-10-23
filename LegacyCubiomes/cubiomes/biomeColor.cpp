
#include <cstring>

#include "biomeColor.hpp"
#include "biomeID.hpp"


void setBiomeColor(unsigned char biomeColor[256][3], int id, unsigned char r, unsigned char g, unsigned char b) {
    biomeColor[id][0] = r;
    biomeColor[id][1] = g;
    biomeColor[id][2] = b;
}

void setMutationColor(unsigned char biomeColor[256][3], int mutated, int parent) {
    unsigned int c;
    biomeColor[mutated][0] = (c = biomeColor[parent][0] + 40) > 255 ? 255 : c;
    biomeColor[mutated][1] = (c = biomeColor[parent][1] + 40) > 255 ? 255 : c;
    biomeColor[mutated][2] = (c = biomeColor[parent][2] + 40) > 255 ? 255 : c;
}

/**
 * This coloring scheme is largely inspired by the AMIDST program:
 * \n
 * https://github.com/toolbox4minecraft/amidst
 * \n
 * https://sourceforge.net/projects/amidst.mirror/
 *
 * @param biomeColors
 */
void initBiomeColors(unsigned char biomeColors[256][3]) {
    memset(biomeColors, 0, 256 * 3);

    setBiomeColor(biomeColors, ocean, 0, 0, 112);
    setBiomeColor(biomeColors, plains, 141, 179, 96);
    setBiomeColor(biomeColors, desert, 250, 148, 24);
    setBiomeColor(biomeColors, mountains, 96, 96, 96);
    setBiomeColor(biomeColors, forest, 5, 102, 33);
    setBiomeColor(biomeColors, taiga, 11, 106, 95); //11, 102, 89);
    setBiomeColor(biomeColors, swamp, 7, 249, 178);
    setBiomeColor(biomeColors, river, 0, 0, 255);
    setBiomeColor(biomeColors, nether_wastes, 87, 37, 38);
    setBiomeColor(biomeColors, the_end, 128, 128, 255);
    setBiomeColor(biomeColors, frozen_ocean, 112, 112, 214);
    setBiomeColor(biomeColors, frozen_river, 160, 160, 255);
    setBiomeColor(biomeColors, snowy_tundra, 255, 255, 255);
    setBiomeColor(biomeColors, snowy_mountains, 160, 160, 160);
    setBiomeColor(biomeColors, mushroom_fields, 255, 0, 255);
    setBiomeColor(biomeColors, mushroom_field_shore, 160, 0, 255);
    setBiomeColor(biomeColors, beach, 250, 222, 85);
    setBiomeColor(biomeColors, desert_hills, 210, 95, 18);
    setBiomeColor(biomeColors, wooded_hills, 34, 85, 28);
    setBiomeColor(biomeColors, taiga_hills, 22, 57, 51);
    setBiomeColor(biomeColors, mountain_edge, 114, 120, 154);
    setBiomeColor(biomeColors, jungle, 80, 123, 10); //83, 123, 9);
    setBiomeColor(biomeColors, jungle_hills, 44, 66, 5);
    setBiomeColor(biomeColors, jungle_edge, 96, 147, 15); //98, 139, 23);
    setBiomeColor(biomeColors, deep_ocean, 0, 0, 48);
    setBiomeColor(biomeColors, stone_shore, 162, 162, 132);
    setBiomeColor(biomeColors, snowy_beach, 250, 240, 192);
    setBiomeColor(biomeColors, birch_forest, 48, 116, 68);
    setBiomeColor(biomeColors, birch_forest_hills, 31, 95, 50);
    setBiomeColor(biomeColors, dark_forest, 64, 81, 26);
    setBiomeColor(biomeColors, snowy_taiga, 49, 85, 74);
    setBiomeColor(biomeColors, snowy_taiga_hills, 36, 63, 54);
    setBiomeColor(biomeColors, giant_tree_taiga, 89, 102, 81);
    setBiomeColor(biomeColors, giant_tree_taiga_hills, 69, 79, 62);
    setBiomeColor(biomeColors, wooded_mountains, 91, 115, 82); //80, 112, 80);
    setBiomeColor(biomeColors, savanna, 189, 178, 95);
    setBiomeColor(biomeColors, savanna_plateau, 167, 157, 100);
    setBiomeColor(biomeColors, badlands, 217, 69, 21);
    setBiomeColor(biomeColors, wooded_badlands_plateau, 176, 151, 101);
    setBiomeColor(biomeColors, badlands_plateau, 202, 140, 101);

    setBiomeColor(biomeColors, small_end_islands, 75, 75, 171);
    setBiomeColor(biomeColors, end_midlands, 201, 201, 89);
    setBiomeColor(biomeColors, end_highlands, 181, 181, 54);
    setBiomeColor(biomeColors, end_barrens, 112, 112, 204);

    setBiomeColor(biomeColors, warm_ocean, 0, 0, 172);
    setBiomeColor(biomeColors, lukewarm_ocean, 0, 0, 144);
    setBiomeColor(biomeColors, cold_ocean, 32, 32, 112);
    setBiomeColor(biomeColors, deep_warm_ocean, 0, 0, 80);
    setBiomeColor(biomeColors, deep_lukewarm_ocean, 0, 0, 64);
    setBiomeColor(biomeColors, deep_cold_ocean, 32, 32, 56);
    setBiomeColor(biomeColors, deep_frozen_ocean, 64, 64, 144);

    setBiomeColor(biomeColors, the_void, 0, 0, 0);

    setMutationColor(biomeColors, sunflower_plains, plains);
    setMutationColor(biomeColors, desert_lakes, desert);
    setMutationColor(biomeColors, gravelly_mountains, mountains);
    setMutationColor(biomeColors, flower_forest, forest);
    setMutationColor(biomeColors, taiga_mountains, taiga);
    setMutationColor(biomeColors, swamp_hills, swamp);
    setBiomeColor(biomeColors, ice_spikes, 180, 220, 220);
    setMutationColor(biomeColors, modified_jungle, jungle);
    setMutationColor(biomeColors, modified_jungle_edge, jungle_edge);
    setMutationColor(biomeColors, tall_birch_forest, birch_forest);
    setMutationColor(biomeColors, tall_birch_hills, birch_forest_hills);
    setMutationColor(biomeColors, dark_forest_hills, dark_forest);
    setMutationColor(biomeColors, snowy_taiga_mountains, snowy_taiga);
    setMutationColor(biomeColors, giant_spruce_taiga, giant_tree_taiga);
    setMutationColor(biomeColors, giant_spruce_taiga_hills, giant_tree_taiga_hills);
    setMutationColor(biomeColors, modified_gravelly_mountains, wooded_mountains);
    setMutationColor(biomeColors, shattered_savanna, savanna);
    setMutationColor(biomeColors, shattered_savanna_plateau, savanna_plateau);
    setMutationColor(biomeColors, eroded_badlands, badlands);
    setMutationColor(biomeColors, modified_wooded_badlands_plateau, wooded_badlands_plateau);
    setMutationColor(biomeColors, modified_badlands_plateau, badlands_plateau);

    setBiomeColor(biomeColors, bamboo_jungle, 132, 149, 0);      //118, 142, 20);
    setBiomeColor(biomeColors, bamboo_jungle_hills, 92, 108, 4); //;59, 71, 10);
}


void initBiomeTypeColors(unsigned char biomeColors[256][3]) {
    memset(biomeColors, 0, 256 * 3);
    setBiomeColor(biomeColors, Oceanic, 0x00, 0x00, 0xa0);
    setBiomeColor(biomeColors, Warm, 0xff, 0xc0, 0x00);
    setBiomeColor(biomeColors, Lush, 0x00, 0xa0, 0x00);
    setBiomeColor(biomeColors, Cold, 0x60, 0x60, 0x60);
    setBiomeColor(biomeColors, Freezing, 0xff, 0xff, 0xff);
}
