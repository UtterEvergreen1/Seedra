#include <filesystem>

#include "common/timer.hpp"
#include "structures/__include.hpp"
#include "terrain/generator.hpp"
#define INIT_STATIC numStaticRegions
#define INIT_DYNAMIC numDynamicRegions + 1, RegionPos2D(numDynamicRegions + 1)
int main() {
    typedef std::vector<std::vector<Pos2D>> RegionPos2D;
    constexpr int ALL_BIOMES[] = {
        plains,
        desert,
        forest,
        taiga,
        swampland,
        river,
        frozen_river,
        ice_plains,
        ice_mountains,
        mushroom_island,
        mushroom_island_shore,
        beach,
        jungle,
        stone_beach,
        cold_beach,
        birch_forest,
        roofed_forest,
        cold_taiga,
        mega_taiga,
        wooded_mountains,
        savanna,
        savanna_plateau,
        mesa,
        mesa_plateau_stone,
        mesa_plateau,
        // warm_ocean,
        deep_warm_ocean,
        // lukewarm_ocean,
        deep_lukewarm_ocean,
        // cold_ocean,
        deep_cold_ocean,
        // frozen_ocean,
        deep_frozen_ocean
    };
    constexpr int ALL_MUTATED_BIOMES[] = {
        // sunflower_plains - 128,
        // desert_mutated - 128,
        // extreme_hills_mutated - 128,
        flower_forest - 128,
        // taiga_mutated - 128,
        // swampland_mutated - 128,
        ice_spikes - 128,
        //jungle_mutated - 128,
        //jungle_edge_mutated - 128,
        // tall_birch_forest - 128,
        //tall_birch_hills - 128,
        // roofed_forest_mutated - 128,
        // cold_taiga_mutated - 128,
        // mega_spruce_taiga - 128,
        // giant_spruce_taiga_hills - 128,
        // modified_gravelly_mountains - 128,
        // savanna_mutated - 128,
        // savanna_plateau_mutated - 128,
        // mesa_bryce - 128,
        // mesa_plateau_stone_mutated - 128,
        // mesa_plateau_mutated - 128
    };
    constexpr int ALL_BIOMES_SIZE = sizeof(ALL_BIOMES) / sizeof(ALL_BIOMES[0]);
    constexpr int ALL_MUTATED_BIOMES_SIZE = sizeof(ALL_MUTATED_BIOMES) / sizeof(ALL_MUTATED_BIOMES[0]);

    constexpr int CLASSIC_WORLD_SIZE = 746496; // Classic world size in blocks (area)

    constexpr int64_t MAX_STRUCTURE_SEED = (1LL << 48); // Maximum seed value for 48-bit seeds
    constexpr int64_t MAX_SISTER_SEED = 0x7FFF000000000000;
    constexpr int NUM_SISTER_SEEDS = 0x10000; // 2^16
    constexpr int numXStaticRegions = 2;
    constexpr int numZStaticRegions = 2;
    constexpr int numXDynamicRegions = 1;
    constexpr int numZDynamicRegions = 1;

    constexpr int numStaticRegions = numXStaticRegions * numZStaticRegions * 4;
    constexpr int numDynamicRegions = numXDynamicRegions * numZDynamicRegions;

    std::string checkPointFile = "ABAS_checkpoint.txt";
    std::string resultsFile = "ABAS_results.txt";

    int64_t structureSeed;
    std::ifstream checkPointIn(checkPointFile);
    if (checkPointIn.is_open()) {
        checkPointIn >> structureSeed;
        if (structureSeed < 0 || structureSeed >= MAX_STRUCTURE_SEED) {
            std::cerr << "Invalid checkpoint seed: " << structureSeed << std::endl;
            return 1;
        }
        std::cout << "Resuming from seed: " << structureSeed << std::endl;
        checkPointIn.close();
    } else {
        std::cout << "No checkpoint found, starting from seed 0." << std::endl;
    }

    std::ofstream checkPointOut;

    std::ofstream results(resultsFile, std::ios::app);
    if (!results.is_open()) {
        std::cerr << "Failed to open results file." << std::endl;
        return 1;
    }

    Timer timer;

    Generator g(lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    std::vector<Pos2D> possibleFeatures(INIT_STATIC);
    std::vector<Pos2D> possibleVillages(INIT_STATIC);
    std::vector<RegionPos2D> possibleMansions(INIT_DYNAMIC); // stores attempts per region
    std::vector<RegionPos2D> possibleMonuments(INIT_DYNAMIC); // stores attempts per region
    std::vector<RegionPos2D> possibleTreasures(INIT_DYNAMIC); // stores attempts per region
    std::vector<RegionPos2D> possibleShipwrecks(INIT_DYNAMIC); // stores attempts per region

    for (; structureSeed < MAX_STRUCTURE_SEED; ++structureSeed) {
        double timeElapsed = timer.getSeconds();
        std::cout << "Time: " << timeElapsed << " Seeds per second: " << ((double)NUM_SISTER_SEEDS / timeElapsed) << std::endl;
        timer.reset();
        checkPointOut.open(checkPointFile);
        if (!checkPointOut.is_open()) {
            std::cerr << "Failed to open checkpoint file." << std::endl;
            return 1;
        }
        checkPointOut << structureSeed << std::endl;
        checkPointOut.close();

        possibleFeatures.clear();
        possibleVillages.clear();

        // Initialize possible positions for static structures
        for (int regionX = -numXStaticRegions; regionX < numXStaticRegions; ++regionX) {
            for (int regionZ = -numZStaticRegions; regionZ < numZStaticRegions; ++regionZ) {
                Pos2D featurePos = Placement::Feature::getRegionBlockPosition(g.getWorldSeed(), regionX, regionZ);
                possibleFeatures.push_back(featurePos);

                Pos2D villagePos = Placement::Village<false>::getRegionBlockPosition(g.getWorldSeed(), regionX, regionZ);
                possibleVillages.push_back(villagePos);
            }
        }

        // initialize possible structures for dynamic regions
        for (int regionX = -numXDynamicRegions; regionX < numXDynamicRegions; ++regionX) {
            for (int regionZ = -numZDynamicRegions; regionZ < numZDynamicRegions; ++regionZ) {
                possibleMansions[regionX + numDynamicRegions][regionZ + numDynamicRegions] =
                    Placement::Mansion::getAllPossibleChunks(g.getWorldSeed(), regionX, regionZ);

                possibleMonuments[regionX + numDynamicRegions][regionZ + numDynamicRegions] =
                    Placement::Monument::getAllPossibleChunks(g.getWorldSeed(), regionX, regionZ);

                possibleTreasures[regionX + numDynamicRegions][regionZ + numDynamicRegions] =
                    Placement::BuriedTreasure::getAllPossibleChunks(g.getWorldSeed(), regionX, regionZ);

                possibleShipwrecks[regionX + numDynamicRegions][regionZ + numDynamicRegions] =
                    Placement::Shipwreck::getAllPossibleChunks(g.getWorldSeed(), regionX, regionZ);
            }
        }

        // test sister seeds now for all possible features
        for (int64_t sisterSeed = structureSeed | 0x8000000000000000LL; sisterSeed < MAX_SISTER_SEED; sisterSeed += 0x1000000000000) {
            bool validSeed = true;
            g.applyWorldSeed(sisterSeed);
            bool validFeatures[(int)StructureType::FEATURE_NUM] = {};
            for (const auto &pos : possibleFeatures) {
                StructureType type = Placement::Feature::getFeatureType(&g, pos);
                if (type != StructureType::NONE) {
                    validFeatures[(int)type] = true;
                }
            }

            for (bool& valid : validFeatures) {
                // check if all features are valid
                validSeed &= valid;
            }

            if (!validSeed) continue;
            validSeed = false;
            // check villages
            for (const auto &villagePos : possibleVillages) {
                if (!Placement::Village<false>::verifyBlockPosition(&g, villagePos)) continue;

                gen::Village villageGen(&g, false);
                villageGen.generate(villagePos);
                if (villageGen.hasMoreThanTwoComponents()) {
                    validSeed = true; // has at least one village
                    break; // no need to check further villages
                }
            }

            if (!validSeed) continue;

            validSeed = false; // reset for structure checks
            uint8_t hasStructure = 0; // using the bits to track which structures are found
            for (int regionX = -numXDynamicRegions; regionX < numXDynamicRegions && !validSeed; ++regionX) {
                for (int regionZ = -numZDynamicRegions; regionZ < numZDynamicRegions && !validSeed; ++regionZ) {
                    // check mansions
                    if ((hasStructure & 1) == 0) {
                        for (const auto &pos : possibleMansions[regionX + numDynamicRegions][regionZ + numDynamicRegions]) {
                            if (!Placement::Mansion::verifyBlockPosition(&g, pos.x, pos.z)) continue;
                            hasStructure |= 1;
                            break;
                        }
                    }

                    // check monuments
                    if ((hasStructure & 2) == 0) {
                        for (const auto &pos : possibleMonuments[regionX + numDynamicRegions][regionZ + numDynamicRegions]) {
                            if (!Placement::Monument::verifyBlockPosition(&g, pos.x, pos.z)) continue;
                            hasStructure |= 2;
                            break;
                        }
                    }

                    // check buried treasures
                    if ((hasStructure & 4) == 0) {
                        for (const auto &pos : possibleTreasures[regionX + numDynamicRegions][regionZ + numDynamicRegions]) {
                            if (!Placement::BuriedTreasure::verifyBlockPosition(&g, pos.x, pos.z)) continue;
                            hasStructure |= 4;
                            break;
                        }
                    }

                    // check shipwrecks
                    if ((hasStructure & 8) == 0) {
                        for (const auto &pos : possibleShipwrecks[regionX + numDynamicRegions][regionZ + numDynamicRegions]) {
                            if (!Placement::Shipwreck::verifyBlockPosition(&g, pos.x, pos.z)) continue;
                            hasStructure |= 8;
                            break;
                        }
                    }

                    // check if all structures are found
                    if (hasStructure == 15) {
                        validSeed = true; // has at least one of each structure
                    }
                }
            }

            if (!validSeed) continue;

            constexpr int mutatedBiomesSize = modified_badlands_plateau - 127;
            bool hasBiomes[ (int)BiomeID::small_end_islands] = {};
            bool hasMutatedBiomes[mutatedBiomesSize] = {};
            const int worldSize = g.getWorldCoordinateBounds() * 2; // world size in blocks
            int* biomes = g.getBiomeRange(1, -g.getWorldCoordinateBounds(), -g.getWorldCoordinateBounds(), worldSize,
                                          worldSize);
            for (int i = 0; i < CLASSIC_WORLD_SIZE; ++i) {
                int biome = biomes[i];
                if (biome > 128) {
                    if (biome <= modified_badlands_plateau) {
                        hasMutatedBiomes[biome - 128] = true;
                    }
                    continue;
                }
                hasBiomes[biome] = true;
            }
            free(biomes);

            // check if all biomes are present
            bool allBiomesPresent = true;
            for (int i : ALL_BIOMES) {
                if (!hasBiomes[i]) {
                    allBiomesPresent = false;
                    break;
                }
            }
            if (!allBiomesPresent) continue;

            // check if all mutated biomes are present
            for (int i : ALL_MUTATED_BIOMES) {
                if (!hasMutatedBiomes[i]) {
                    allBiomesPresent = false;
                    break;
                }
            }
            if (!allBiomesPresent) continue;

            std::cout << "Valid seed found: " << sisterSeed << std::endl;
            results << "Valid seed found: " << sisterSeed << std::endl;
            results.flush();
        }
    }
}