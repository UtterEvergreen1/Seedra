#include "BalancedSeed.hpp"

#include "generator.hpp"
#include "terrain/biomes/biome_t.hpp"

namespace BalancedSeed {
    int getBiomeGroup(const biome_t biomeID) {
        switch (biomeID) {
            // Category 0: Cold biomes (snow and ice)
            case biome_t::snowy_tundra:
            case biome_t::ice_mountains:
            case biome_t::snowy_beach:
            case biome_t::snowy_taiga:
            case biome_t::snowy_taiga_hills:
            case biome_t::ice_spikes:
            case biome_t::snowy_taiga_mountains:
                return 0;
            // Category 1: High elevation biomes (mountains)
            case biome_t::extreme_hills:
            case biome_t::taiga:
            //case biome_t::the_end:
            case biome_t::taiga_hills:
            case biome_t::extreme_hills_edge:
            case biome_t::stone_beach:
            case biome_t::mega_taiga:
            case biome_t::mega_taiga_hills:
            case biome_t::wooded_mountains:
            case biome_t::gravelly_mountains:
            case biome_t::taiga_mutated:
            case biome_t::mega_spruce_taiga:
            case biome_t::giant_spruce_taiga_hills:
            case biome_t::modified_gravelly_mountains:
                return 1;
            // Category 2: Moderate climate biomes (forests, plains, beaches, etc.)
            case biome_t::plains:
            case biome_t::forest:
            case biome_t::beach:
            case biome_t::forest_hills:
            case biome_t::birch_forest:
            case biome_t::birch_forest_hills:
            case biome_t::roofed_forest:
            case biome_t::sunflower_plains:
            case biome_t::flower_forest:
            case biome_t::tall_birch_forest:
            case biome_t::tall_birch_hills:
            case biome_t::dark_forest_hills:
                return 2;
            // Category 3: High humidity biomes (swamps, jungles)
            case biome_t::swampland:
            case biome_t::jungle:
            case biome_t::jungle_hills:
            case biome_t::jungle_edge:
            case biome_t::swamp_hills:
            case biome_t::modified_jungle:
            case biome_t::modified_jungle_edge:
                return 3;
            // Category 4: Hot and dry biomes (deserts, savannas, mesas)
            case biome_t::desert:
            case biome_t::desert_hills:
            case biome_t::savanna:
            case biome_t::savanna_plateau:
            case biome_t::mesa:
            case biome_t::mesa_plateau_stone:
            case biome_t::mesa_plateau:
            case biome_t::desert_lakes:
            case biome_t::shattered_savanna:
            case biome_t::shattered_savanna_plateau:
            case biome_t::mesa_bryce:
            case biome_t::mesa_plateau_stone_mutated:
            case biome_t::mesa_plateau_mutated:
                return 4;
            // Category 5: Ocean biomes (ocean, deep ocean, warm ocean, etc.)
            case biome_t::ocean:
            case biome_t::legacy_frozen_ocean:
            case biome_t::deep_ocean:
            case biome_t::warm_ocean:
            case biome_t::deep_warm_ocean:
            case biome_t::lukewarm_ocean:
            case biome_t::deep_lukewarm_ocean:
            case biome_t::cold_ocean:
            case biome_t::deep_cold_ocean:
            case biome_t::frozen_ocean:
            case biome_t::deep_frozen_ocean:
                return 5;
            // Category 6: River biomes (river, frozen river)
            case biome_t::river:
            case biome_t::frozen_river:
                return 6;
            // Category 7: Mushroom biomes (mushroom fields, mushroom field shore)
            case biome_t::mushroom_island:
            case biome_t::mushroom_island_shore:
                return 7;
            // Category 8: Other biomes (the end, nether, etc.)
            default:
                return 8;
        }
    }

    std::vector<float> getFracs(const biome_t* biomes) {
        constexpr int biomesSize = 40000; // size of biomes
        std::vector fracs(8, 0.0F);
        int index = 0;
        while (index < biomesSize) {
            // getting the biome group it is in (take the biomeID and assign it a category)
            const auto biomeGroup = static_cast<size_t>(getBiomeGroup(biomes[index]));
            if (biomeGroup != 8)
                fracs[biomeGroup]++;
            index++;
        }
        for (size_t div = 0; div < 8; div++) { fracs[div] /= biomesSize; }
        return fracs;
    }

    bool getIsMatch(const biome_t* biomes) {
        const std::vector<float> floatFrac = getFracs(biomes);
        // Check if the biome distribution is balanced
        // Check mushroom biomes first since they are the rarest
        if (floatFrac[7] < 0.001F) { return false; } // mushroom biomes
        // Check for the minimum amount of each biome group
        for (size_t i = 0; i < 5; i++) {
            if (floatFrac[i] < 0.01F) { return false; } // cold, high elevation, moderate climate, high humidity, hot and dry
        }
        // Check for the maximum amount of ocean and river biomes
        if (0.25F < floatFrac[5]) { return false; } // ocean
        if (0.1F < floatFrac[6]) { return false; } // river
        return true;
    }
}
