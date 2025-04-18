#pragma once

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

#include "generator.hpp"
#include "common/timer.hpp"
#include "terrain/biomes/biomeID.hpp"

namespace BalancedSeed {
    inline int getBiomeGroup(c_int biomeID) {
        switch (biomeID) {
            // Category 0: Cold biomes (snow and ice)
            case snowy_tundra:
            case ice_mountains:
            case snowy_beach:
            case snowy_taiga:
            case snowy_taiga_hills:
            case ice_spikes:
            case snowy_taiga_mountains:
                return 0;
            // Category 1: High elevation biomes (mountains)
            case extreme_hills:
            case taiga:
            //case the_end:
            case taiga_hills:
            case extreme_hills_edge:
            case stone_beach:
            case mega_taiga:
            case mega_taiga_hills:
            case wooded_mountains:
            case gravelly_mountains:
            case taiga_mutated:
            case mega_spruce_taiga:
            case giant_spruce_taiga_hills:
            case modified_gravelly_mountains:
                return 1;
            // Category 2: Moderate climate biomes (forests, plains, beaches, etc.)
            case plains:
            case forest:
            case beach:
            case forest_hills:
            case birch_forest:
            case birch_forest_hills:
            case roofed_forest:
            case sunflower_plains:
            case flower_forest:
            case tall_birch_forest:
            case tall_birch_hills:
            case dark_forest_hills:
                return 2;
            // Category 3: High humidity biomes (swamps, jungles)
            case swampland:
            case jungle:
            case jungle_hills:
            case jungle_edge:
            case swamp_hills:
            case modified_jungle:
            case modified_jungle_edge:
                return 3;
            // Category 4: Hot and dry biomes (deserts, savannas, mesas)
            case desert:
            case desert_hills:
            case savanna:
            case savanna_plateau:
            case mesa:
            case mesa_plateau_stone:
            case mesa_plateau:
            case desert_lakes:
            case shattered_savanna:
            case shattered_savanna_plateau:
            case mesa_bryce:
            case mesa_plateau_stone_mutated:
            case mesa_plateau_mutated:
                return 4;
            // Category 5: Ocean biomes (ocean, deep ocean, warm ocean, etc.)
            case ocean:
            case legacy_frozen_ocean:
            case deep_ocean:
            case warm_ocean:
            case deep_warm_ocean:
            case lukewarm_ocean:
            case deep_lukewarm_ocean:
            case cold_ocean:
            case deep_cold_ocean:
            case frozen_ocean:
            case deep_frozen_ocean:
                return 5;
            // Category 6: River biomes (river, frozen river)
            case river:
            case frozen_river:
                return 6;
            // Category 7: Mushroom biomes (mushroom fields, mushroom field shore)
            case mushroom_island:
            case mushroom_island_shore:
                return 7;
            // Category 8: Other biomes (the end, nether, etc.)
            default:
                return 8;
        }
    }

    inline std::vector<float> getFracs(c_int* biomes) {
        constexpr int biomesSize = 40000; // size of biomes
        std::vector fracs(8, 0.0F);
        int index = 0;
        while (index < biomesSize) {
            c_int biomeGroup = getBiomeGroup(
                    biomes[index]); // getting the biome group it is in (take the biomeID and assign it a catagory)
            if (biomeGroup != 8)
                fracs[biomeGroup]++;
            index++;
        }
        for (int div = 0; div < 8; div++) { fracs[div] /= biomesSize; }
        return fracs;
    }

    inline bool getIsMatch(c_int* biomes) {
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
    MU inline i64 findBalancedSeed(const LCEVERSION version, int numThreads) {
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
