#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include "util.hpp"
#include "layers.hpp"



const char *biome2str(int id)
{
    switch (id)
    {
    case ocean: return "ocean";
    case plains: return "plains";
    case desert: return "desert";
    case mountains: return "mountains";
    case forest: return "forest";
    case taiga: return "taiga";
    case swamp: return "swamp";
    case river: return "river";
    case nether_wastes: return "nether_wastes";
    case the_end: return "the_end";
    // 10
    case frozen_ocean: return "frozen_ocean";
    case frozen_river: return "frozen_river";
    case snowy_tundra: return "snowy_tundra";
    case snowy_mountains: return "snowy_mountains";
    case mushroom_fields: return "mushroom_fields";
    case mushroom_field_shore: return "mushroom_field_shore";
    case beach: return "beach";
    case desert_hills: return "desert_hills";
    case wooded_hills: return "wooded_hills";
    case taiga_hills: return "taiga_hills";
    // 20
    case mountain_edge: return "mountain_edge";
    case jungle: return "jungle";
    case jungle_hills: return "jungle_hills";
    case jungle_edge: return "jungle_edge";
    case deep_ocean: return "deep_ocean";
    case stone_shore: return "stone_shore";
    case snowy_beach: return "snowy_beach";
    case birch_forest: return "birch_forest";
    case birch_forest_hills: return "birch_forest_hills";
    case dark_forest: return "dark_forest";
    // 30
    case snowy_taiga: return "snowy_taiga";
    case snowy_taiga_hills: return "snowy_taiga_hills";
    case giant_tree_taiga: return "giant_tree_taiga";
    case giant_tree_taiga_hills: return "giant_tree_taiga_hills";
    case wooded_mountains: return "wooded_mountains";
    case savanna: return "savanna";
    case savanna_plateau: return "savanna_plateau";
    case badlands: return "badlands";
    case wooded_badlands_plateau: return "wooded_badlands_plateau";
    case badlands_plateau: return "badlands_plateau";
    // 40  --  1.13
    case small_end_islands: return "small_end_islands";
    case end_midlands: return "end_midlands";
    case end_highlands: return "end_highlands";
    case end_barrens: return "end_barrens";
    case warm_ocean: return "warm_ocean";
    case lukewarm_ocean: return "lukewarm_ocean";
    case cold_ocean: return "cold_ocean";
    case deep_warm_ocean: return "deep_warm_ocean";
    case deep_lukewarm_ocean: return "deep_lukewarm_ocean";
    case deep_cold_ocean: return "deep_cold_ocean";
    // 50
    case deep_frozen_ocean: return "deep_frozen_ocean";

    case the_void: return "the_void";

    // mutated variants
    case sunflower_plains: return "sunflower_plains";
    case desert_lakes: return "desert_lakes";
    case gravelly_mountains: return "gravelly_mountains";
    case flower_forest: return "flower_forest";
    case taiga_mountains: return "taiga_mountains";
    case swamp_hills: return "swamp_hills";
    case ice_spikes: return "ice_spikes";
    case modified_jungle: return "modified_jungle";
    case modified_jungle_edge: return "modified_jungle_edge";
    case tall_birch_forest: return "tall_birch_forest";
    case tall_birch_hills: return "tall_birch_hills";
    case dark_forest_hills: return "dark_forest_hills";
    case snowy_taiga_mountains: return "snowy_taiga_mountains";
    case giant_spruce_taiga: return "giant_spruce_taiga";
    case giant_spruce_taiga_hills: return "giant_spruce_taiga_hills";
    case modified_gravelly_mountains: return "modified_gravelly_mountains";
    case shattered_savanna: return "shattered_savanna";
    case shattered_savanna_plateau: return "shattered_savanna_plateau";
    case eroded_badlands: return "eroded_badlands";
    case modified_wooded_badlands_plateau: return "modified_wooded_badlands_plateau";
    case modified_badlands_plateau: return "modified_badlands_plateau";
    // 1.14
    case bamboo_jungle: return "bamboo_jungle";
    case bamboo_jungle_hills: return "bamboo_jungle_hills";
    default: return nullptr;
    }
    return nullptr;
}

const char* getStructureName(StructureType type) {
    switch (type) {
    case StructureType::DesertPyramid: return "DesertPyramid";
    case StructureType::JunglePyramid: return "JunglePyramid";
    case StructureType::SwampHut: return "SwampHut";
    case StructureType::Igloo: return "Igloo";
    case StructureType::Village: return "Village";
    case StructureType::OceanRuin: return "OceanRuin";
    case StructureType::Mansion: return "Mansion";
    case StructureType::Monument: return "Monument";
    case StructureType::Treasure: return "Buried Treasure";
    case StructureType::Shipwreck: return "Shipwreck";
    case StructureType::Outpost: return "Outpost";
    case StructureType::Mineshaft: return "Mineshaft";
    case StructureType::Fortress: return "Fortress";
    case StructureType::EndCity: return "EndCity";
    case StructureType::EndGateway: return "EndGateway";
    default: return "NONE";
    }
}

void setBiomeColor(unsigned char biomeColor[256][3], int id,
        unsigned char r, unsigned char g, unsigned char b) {
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

void initBiomeColors(unsigned char biomeColors[256][3]) {
    // This coloring scheme is largely inspired by the AMIDST program:
    // https://github.com/toolbox4minecraft/amidst
    // https://sourceforge.net/projects/amidst.mirror/

    memset(biomeColors, 0, 256*3);

    setBiomeColor(biomeColors, ocean, 0, 0, 112);
    setBiomeColor(biomeColors, plains, 141, 179, 96);
    setBiomeColor(biomeColors, desert, 250, 148, 24);
    setBiomeColor(biomeColors, mountains, 96, 96, 96);
    setBiomeColor(biomeColors, forest, 5, 102, 33);
    setBiomeColor(biomeColors, taiga, 11, 106, 95);//11, 102, 89);
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
    setBiomeColor(biomeColors, jungle, 80, 123, 10);//83, 123, 9);
    setBiomeColor(biomeColors, jungle_hills, 44, 66, 5);
    setBiomeColor(biomeColors, jungle_edge, 96, 147, 15);//98, 139, 23);
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
    setBiomeColor(biomeColors, wooded_mountains, 91, 115, 82);//80, 112, 80);
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

    setBiomeColor(biomeColors, bamboo_jungle, 132, 149, 0);//118, 142, 20);
    setBiomeColor(biomeColors, bamboo_jungle_hills, 92, 108, 4);//;59, 71, 10);
}

void initBiomeTypeColors(unsigned char biomeColors[256][3]) {
    memset(biomeColors, 0, 256 * 3);
    setBiomeColor(biomeColors, Oceanic,  0x00, 0x00, 0xa0);
    setBiomeColor(biomeColors, Warm,     0xff, 0xc0, 0x00);
    setBiomeColor(biomeColors, Lush,     0x00, 0xa0, 0x00);
    setBiomeColor(biomeColors, Cold,     0x60, 0x60, 0x60);
    setBiomeColor(biomeColors, Freezing, 0xff, 0xff, 0xff);
}


// find the longest biome name contained in 's'
static int _str2id(const char *s)
{
    if (*s == 0)
        return -1;
    const char *f = nullptr;
    int ret = -1, id;
    for (id = 0; id < 256; id++)
    {
        const char *p = biome2str(id);
        if (p && (!f || strlen(f) < strlen(p)))
            if (strstr(s, p)) {f = p; ret = id;}

        const char *t = biome2str(id);
        if (t && t != p && (!f || strlen(f) < strlen(p)))
            if (strstr(s, t)) {f = t; ret = id;}
    }
    return ret;
}

int parseBiomeColors(unsigned char biomeColors[256][3], const char *buf)
{
    const char *p = buf;
    char bstr[64];
    int id, col[4], n, ib, ic;
    n = 0;
    while (*p)
    {
        for (ib = ic = 0; *p && *p != '\n'; p++)
        {
            if (isalpha(*p) && (size_t)ib+1 < sizeof(bstr))
                bstr[ib++] = *p;
            if (ic < 4 && (*p == '#' || (p[0] == '0' && p[1] == 'x')))
                col[ic++] = strtol(p+1+(*p=='0'), (char**)&p, 16);
            else if (ic < 4 && *p >= '0' && *p <= '9')
                col[ic++] = strtol(p, (char**)&p, 10);
            if (*p == '\n')
                break;
        }
        while (*p == '\n') p++;

        bstr[ib] = 0;
        id = _str2id(bstr);
        if (id >= 0 && id < 256)
        {
            if (ic == 3)
            {
                biomeColors[id][0] = col[0] & 0xff;
                biomeColors[id][1] = col[1] & 0xff;
                biomeColors[id][2] = col[2] & 0xff;
                n++;
            }
            else if (ic == 1)
            {
                biomeColors[id][0] = (col[0] >> 16) & 0xff;
                biomeColors[id][1] = (col[0] >>  8) & 0xff;
                biomeColors[id][2] = (col[0] >>  0) & 0xff;
                n++;
            }
        }
        else if (ic == 4)
        {
            id = col[0] & 0xff;
            biomeColors[id][0] = col[1] & 0xff;
            biomeColors[id][1] = col[2] & 0xff;
            biomeColors[id][2] = col[3] & 0xff;
            n++;
        }
        else if (ic == 2)
        {
            id = col[0] & 0xff;
            biomeColors[id][0] = (col[1] >> 16) & 0xff;
            biomeColors[id][1] = (col[1] >>  8) & 0xff;
            biomeColors[id][2] = (col[1] >>  0) & 0xff;
            n++;
        }
    }
    return n;
}
