#pragma once

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

#include "generator.hpp"

namespace BalancedSeed {
    /**
     * @brief Retrieves the biome group for a given biome ID.
     *
     * This function maps a biome ID to its corresponding biome group.
     *
     * @param biomeID The ID of the biome.
     * @return The biome group as an integer.
     */
    int getBiomeGroup(c_int biomeID);

    /**
     * @brief Calculates fractional values for a set of biomes.
     *
     * This function processes an array of biome IDs and returns a vector
     * of fractional values associated with those biomes.
     *
     * @param biomes Pointer to an array of biome IDs.
     * @return A vector of fractional values corresponding to the biomes.
     */
    std::vector<float> getFracs(c_int *biomes);

    /**
     * @brief Determines if a set of biomes matches specific criteria.
     *
     * This function checks whether the provided array of biome IDs satisfies
     * certain matching conditions.
     *
     * @param biomes Pointer to an array of biome IDs.
     * @return True if the biomes match the criteria, false otherwise.
     */
    bool getIsMatch(c_int *biomes);

    // Test seeds for specific scenarios:
    // - wow: 580880946693975163, -8560550246955989385
    // - shipwreck and buried treasure: -2529672569851994728
    // - lots of jungle temples: 7985003115668529754

    /**
     * @brief Finds a balanced seed for biome generation.
     *
     * This function searches for a seed that produces a balanced biome
     * distribution. The search can be configured to stop after 10 seconds
     * if the template parameter `returnAfter10Seconds` is set to true.
     *
     * @tparam returnAfter10Seconds If true, the function will return after 10 seconds.
     * @param version The LCE version to use for biome generation.
     * @param numThreads The number of threads to use for the search.
     * @return The balanced seed as a 64-bit integer.
     */
    template<bool returnAfter10Seconds>
    MU i64 findBalancedSeed(const LCEVERSION version, int numThreads);
}
