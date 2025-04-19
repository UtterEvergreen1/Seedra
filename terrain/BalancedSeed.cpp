#include "BalancedSeed.hpp"
#include "generator.hpp"
#include "common/timer.hpp"
#include "terrain/biomes/biomeID.hpp"

namespace BalancedSeed {
    int getBiomeGroup(c_int biomeID) {
        switch (biomeID) {
            // Category 0: Cold biomes (snow and ice)
            case BiomeID::snowy_tundra:
            case BiomeID::ice_mountains:
            case BiomeID::snowy_beach:
            case BiomeID::snowy_taiga:
            case BiomeID::snowy_taiga_hills:
            case BiomeID::ice_spikes:
            case BiomeID::snowy_taiga_mountains:
                return 0;
            // Category 1: High elevation biomes (mountains)
            case BiomeID::extreme_hills:
            case BiomeID::taiga:
            //case BiomeID::the_end:
            case BiomeID::taiga_hills:
            case BiomeID::extreme_hills_edge:
            case BiomeID::stone_beach:
            case BiomeID::mega_taiga:
            case BiomeID::mega_taiga_hills:
            case BiomeID::wooded_mountains:
            case BiomeID::gravelly_mountains:
            case BiomeID::taiga_mutated:
            case BiomeID::mega_spruce_taiga:
            case BiomeID::giant_spruce_taiga_hills:
            case BiomeID::modified_gravelly_mountains:
                return 1;
            // Category 2: Moderate climate biomes (forests, plains, beaches, etc.)
            case BiomeID::plains:
            case BiomeID::forest:
            case BiomeID::beach:
            case BiomeID::forest_hills:
            case BiomeID::birch_forest:
            case BiomeID::birch_forest_hills:
            case BiomeID::roofed_forest:
            case BiomeID::sunflower_plains:
            case BiomeID::flower_forest:
            case BiomeID::tall_birch_forest:
            case BiomeID::tall_birch_hills:
            case BiomeID::dark_forest_hills:
                return 2;
            // Category 3: High humidity biomes (swamps, jungles)
            case BiomeID::swampland:
            case BiomeID::jungle:
            case BiomeID::jungle_hills:
            case BiomeID::jungle_edge:
            case BiomeID::swamp_hills:
            case BiomeID::modified_jungle:
            case BiomeID::modified_jungle_edge:
                return 3;
            // Category 4: Hot and dry biomes (deserts, savannas, mesas)
            case BiomeID::desert:
            case BiomeID::desert_hills:
            case BiomeID::savanna:
            case BiomeID::savanna_plateau:
            case BiomeID::mesa:
            case BiomeID::mesa_plateau_stone:
            case BiomeID::mesa_plateau:
            case BiomeID::desert_lakes:
            case BiomeID::shattered_savanna:
            case BiomeID::shattered_savanna_plateau:
            case BiomeID::mesa_bryce:
            case BiomeID::mesa_plateau_stone_mutated:
            case BiomeID::mesa_plateau_mutated:
                return 4;
            // Category 5: Ocean biomes (ocean, deep ocean, warm ocean, etc.)
            case BiomeID::ocean:
            case BiomeID::legacy_frozen_ocean:
            case BiomeID::deep_ocean:
            case BiomeID::warm_ocean:
            case BiomeID::deep_warm_ocean:
            case BiomeID::lukewarm_ocean:
            case BiomeID::deep_lukewarm_ocean:
            case BiomeID::cold_ocean:
            case BiomeID::deep_cold_ocean:
            case BiomeID::frozen_ocean:
            case BiomeID::deep_frozen_ocean:
                return 5;
            // Category 6: River biomes (river, frozen river)
            case BiomeID::river:
            case BiomeID::frozen_river:
                return 6;
            // Category 7: Mushroom biomes (mushroom fields, mushroom field shore)
            case BiomeID::mushroom_island:
            case BiomeID::mushroom_island_shore:
                return 7;
            // Category 8: Other biomes (the end, nether, etc.)
            default:
                return 8;
        }
    }

    std::vector<float> getFracs(c_int* biomes) {
        constexpr int biomesSize = 40000; // size of biomes
        std::vector fracs(8, 0.0F);
        int index = 0;
        while (index < biomesSize) {
            c_int biomeGroup = getBiomeGroup(
                    biomes[index]); // getting the biome group it is in (take the biomeID and assign it a category)
            if (biomeGroup != 8)
                fracs[biomeGroup]++;
            index++;
        }
        for (int div = 0; div < 8; div++) { fracs[div] /= biomesSize; }
        return fracs;
    }

    bool getIsMatch(c_int* biomes) {
        const std::vector<float> floatFrac = getFracs(biomes);
        // Check if the biome distribution is balanced
        // Check mushroom biomes first since they are the rarest
        if (floatFrac[7] < 0.001F) { return false; } // mushroom biomes
        // Check for the minimum amount of each biome group
        for (int i = 0; i < 5; i++) {
            if (floatFrac[i] < 0.01F) { return false; } // cold, high elevation, moderate climate, high humidity, hot and dry
        }
        // Check for the maximum amount of ocean and river biomes
        if (0.25F < floatFrac[5]) { return false; } // ocean
        if (0.1F < floatFrac[6]) { return false; } // river
        return true;
    }
    //test seeds
    // wow: 580880946693975163, -8560550246955989385
    //shipwreck and buried treasure: -2529672569851994728
    // lots of j temples: 7985003115668529754
    template <bool returnAfter10Seconds>
    MU i64 findBalancedSeed(const LCEVERSION version, int numThreads) {
        MU std::atomic_bool found(false);
        std::atomic<i64> result(-1);
        Timer timer;

        if (numThreads < 1)
            numThreads = 1;

        if constexpr (returnAfter10Seconds) {
            timer.reset();
        }

        auto searchSeed = [&](Generator* g, RNG localRng) {
            while (!found.load()) {
                c_i64 seed = localRng.nextLongI();
                g->applyWorldSeed(seed);
                int* biomes = g->getBiomeRange(4, -100, -100, 200, 200);
                c_bool isMatch = getIsMatch(biomes);
                free(biomes);
                if (isMatch) {
                    if (!found.exchange(true)) {
                        result = seed;
                    }
                    return;
                }
                if constexpr (returnAfter10Seconds) {
                    if (10.0F < timer.getSeconds()) {
                        found.store(true);
                        return;
                    }
                }
            }
        };

        std::vector<std::thread> threads;
        std::vector<Generator*> generators;
        for (int i = 0; i < numThreads; ++i) {
            RNG rng = RNG::initializeWithRandomSeed();
            rng.setValue(rng.getSeed() + i);
            auto* g = new Generator(lce::CONSOLE::XBOX360, version, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
            generators.push_back(g);
            threads.emplace_back(searchSeed, g, rng);
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        for (auto* g : generators) {
            delete g;
        }

        return result;
    }
}
