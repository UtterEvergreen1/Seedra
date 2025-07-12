#include <thread>
#include <fstream>
#include "terrain/PopulationReverser.hpp"
#include "common/rng.hpp"
#include "loot/tables/buried_treasure.hpp"

void testReverser(i64 structureSeed, int posX, int posZ) {
    u64 chunkSeed = RNG::getChunkSeed(structureSeed, posX, posZ);
    std::cout << "Chunk seed: " << chunkSeed << std::endl;
    auto results = PopulationReverser::getSeedsFromChunkSeed(chunkSeed, posX, posZ);
    for (const auto &seed: results) {
        if (seed == structureSeed)
            std::cout << "Found original seed: " << seed << std::endl;
        else
            std::cout << "Found seed: " << seed << std::endl;
    }
    std::cout << std::endl;
}


void findMostCake() {
    std::vector<std::pair<int,int>> regions = {
         {-1, -1},{0, 0},
        {0, -1}, {-1, 0}
    };

    std::vector<i64> seeds;
    seeds.reserve(50); // Reserve space for 50 seeds per region

    // Build full list of chunk coords at each region
    std::vector<std::vector<std::pair<int,int>>> coords;
    for (const auto& region : regions) {
        int regionX = region.first;
        int regionZ = region.second;
        std::vector<std::pair<int, int>> regionCoords;
        for (int x = regionX * 24; x < (regionX + 1) * 24; ++x) {
            for (int z = regionZ * 24; z < (regionZ + 1) * 24; ++z) {
                regionCoords.emplace_back(x, z);
            }
        }
        coords.push_back(regionCoords);
    }

    loot::Container27 container;
    for (int regionNum = 0; regionNum < regions.size(); ++regionNum) {
        for (int regionNum1 = 0; regionNum1 < regions.size(); ++regionNum1) {
            int regionX = regions[regionNum].first;
            int regionZ = regions[regionNum].second;
            int regionX1 = regions[regionNum1].first;
            int regionZ1 = regions[regionNum1].second;

            std::string inputFile = "cakePopulationSeedsIn_" + std::to_string(regionX) + "_" + std::to_string(regionZ)
                           + "__" + std::to_string(regionX1) + "_" + std::to_string(regionZ1) + ".txt";

            std::ifstream file(inputFile);
            if (!file.is_open()) continue; // does not exist, skip
            seeds.clear();

            std::cout << "Reading seeds from file: " << inputFile << std::endl;
            i64 fileSeed;
            while (file >> fileSeed) {
                seeds.push_back(fileSeed);
            }

            for (i64 seed : seeds) {
                // test the region not read from for 4+ cake at chunk (x, z) in the remaining regions
                for (int regionNum2 = 0; regionNum2 < regions.size(); ++regionNum2) {
                    if (regionNum2 == regionNum || regionNum2 == regionNum1) continue; // skip the regions we read from

                    for (auto [x, z] : coords[regionNum2]) {
                        // test if any of the seeds can generate 4+ cakes in the chunk at (x, z)
                        int cakeCount = 0;
                        container.clear();
                        loot::buried_treasure.getLootFromChunk<loot::GenMode::MOD_NO_SHUF>(container, seed, x, z);
                        for (int itemAt = 0; itemAt < container.slotCount(); itemAt++) {
                            lce::ItemState item = container.getSlotAt(itemAt);
                            if (item.id == lce::items::ids::CAKE_ID) {
                                ++cakeCount;
                            }
                        }

                        if (cakeCount < 2) continue;

                        int regionX2 = regions[regionNum2].first;
                        int regionZ2 = regions[regionNum2].second;
                        std::cout << "Found seed with " << cakeCount << " cakes: " << seed << " at chunk (" << x << ", " << z << ") in region ("
                                  << regionX2 << ", " << regionZ2 << ")\n";
                    }
                }
            }
        }
    }
}

int main() {
    //findMostCake();
    // 12 eye seeds
    /*testReverser(194391861658974LL, -132 >> 4, -42 >> 4);
    testReverser(242527663422655LL, -130 >> 4, 43 >> 4);
    testReverser(25194350356927LL, -46 >> 4, 212 >> 4);
    testReverser(126931460534349LL, -26 >> 4, 201 >> 4);

    // both the same (2 outputs for the same chunk seed)
    testReverser(34845665517315LL, -4 >> 4, 124 >> 4);
    testReverser(132606759487365LL, -4 >> 4, 124 >> 4);

    // other
    testReverser(123, -180 >> 4, 0);
    testReverser(321, 0, 32 >> 4);

    testReverser(0, -1, -10);
    testReverser(273768374452400, -1, -10);*/
    /*u64 chunkSeed = RNG::getChunkSeed(320723133, 13, 14);
    RNG rng(chunkSeed);
    rng.nextInt();
    std::cout << "Treasure Loot table: " << rng.nextInt() << std::endl;*/

    /*auto testResults = PopulationReverser::getSeedsFromChunkSeed(456268685, -24, -24);
    for (const auto& seed : testResults) {
        std::cout << "Found seed: " << seed << std::endl;
    }*/
    /*FILE* file_909926571 = fopen("-909926571.txt", "r");
    FILE* file_1770379634 = fopen("-1770379634.txt", "r");
    FILE* file_2043685600 = fopen("-2043685600.txt", "r");
    // read the three files and combine them
    std::vector<i64> seeds;
    if (file_909926571 && file_1770379634 && file_2043685600) {
        i64 seed;
        while (fscanf(file_909926571, "%lld", &seed) == 1) {
            seeds.push_back(seed);
        }
        fclose(file_909926571);

        while (fscanf(file_1770379634, "%lld", &seed) == 1) {
            seeds.push_back(seed);
        }
        fclose(file_1770379634);

        while (fscanf(file_2043685600, "%lld", &seed) == 1) {
            seeds.push_back(seed);
        }
        fclose(file_2043685600);

        std::sort(seeds.begin(), seeds.end());
        auto last = std::unique(seeds.begin(), seeds.end());
        seeds.erase(last, seeds.end());
    } else {
        std::cerr << "Failed to open one or more input files." << std::endl;
    }
    FILE* out = fopen("5cakePopulationSeed.txt", "w");
    if (out) {
        for (const auto& seed : seeds) {
            fprintf(out, "%lld\n", seed);
        }
        fclose(out);
    } else {
        std::cerr << "Failed to open output file." << std::endl;
    }*/

    // reverse all population seed from -24 to 24 in both x and z direction
    // 1. Read all cake population seeds into memory
    /*std::cout << JAVA_LCG::M2 << std::endl;
    std::cout << JAVA_LCG::A2 << std::endl;
    std::cout << JAVA_LCG::M4 << std::endl;
    std::cout << JAVA_LCG::A4 << std::endl;
    std::vector<i64> inputSeeds;
    {
        FILE *in = fopen("5cakePopulationSeeds.txt", "r");
        if (!in) { std::cerr<<"Failed to open input\n"; return 1; }
        i64 s;
        while (fscanf(in, "%lld", &s) == 1) inputSeeds.push_back(s);
        fclose(in);
    }

    // 2. Build list of all (x,z) chunks
    std::vector<std::pair<int,int>> coords;
    for (int x = -24; x <= 24; ++x)
        for (int z = -24; z <= 24; ++z)
            coords.emplace_back(x,z);

    // Determine thread count
    unsigned numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 1;

    // 3. For each chunk, parallelize across seeds
    for (auto [x, z] : coords) {
        // Launch threads, each accumulates into its own vector
        std::vector<std::vector<i64>> partialOut(numThreads);
        std::vector<std::thread> threads;
        std::cout << "Processing chunk (" << x << ", " << z << ") with " << numThreads << " threads...\n";
        auto start = std::chrono::steady_clock::now();
        for (unsigned t = 0; t < numThreads; ++t) {
            threads.emplace_back([&, t]() {
                auto& out = partialOut[t];
                for (size_t i = t; i < inputSeeds.size(); i += numThreads) {
                    if (i % 1000 == 0) {
                        auto elapsed = std::chrono::steady_clock::now() - start;
                        printerMutex.lock();
                        std::cout << "Speed: " << ((float)elapsed.count() / 1000.0f) / 1000 << " seeds/s" << std::endl;
                        printerMutex.unlock();
                        start = std::chrono::steady_clock::now();
                    }
                    auto res = PopulationReverser::getSeedsFromChunkSeed(inputSeeds[i], x, z);
                    out.insert(out.end(), res.begin(), res.end());
                }
            });
        }

        // Join and merge results
        for (auto& th : threads) th.join();
        std::vector<i64> outSeeds;
        for (auto& vec : partialOut)
            outSeeds.insert(outSeeds.end(), vec.begin(), vec.end());

        // Sort, unique and write file
        std::sort(outSeeds.begin(), outSeeds.end());
        outSeeds.erase(std::unique(outSeeds.begin(), outSeeds.end()), outSeeds.end());

        std::ofstream file(
            std::string("cakePopulationSeed_") + std::to_string(x)
            + "_" + std::to_string(z) + ".txt"
        );
        for (i64 seed : outSeeds) file << seed << "\n";
    }*/

    /*std::vector<std::pair<int,int>> regions;
    for (int rx = -1; rx <= 0; ++rx)
        for (int rz = -1; rz <= 0; ++rz)
            regions.emplace_back(rx, rz);

    std::unordered_map<i64, int> regionSeeds;
    i64 seed;
    int regionCount = 0;

    for (auto [regionX, regionZ] : regions) {
        ++regionCount;
        std::string folder = "results/region_" + std::to_string(regionX) + "_" + std::to_string(regionZ) + "/cakePopulationSeed_";

        // collect unique seeds in this region
        std::unordered_set<i64> currentSeeds;
        for (int x = -24; x <= 24; ++x) {
            for (int z = -24; z <= 24; ++z) {
                std::ifstream file(folder + std::to_string(x) + "_" + std::to_string(z) + ".txt");
                if (!file.is_open()) continue;
                while (file >> seed) {
                    currentSeeds.insert(seed);
                }
            }
        }

        if (regionCount == 1) {
            // first region: initialize counts
            for (i64 s : currentSeeds) {
                regionSeeds[s] = 1;
            }
        } else {
            // subsequent regions: only bump those seen in all previous
            for (i64 s : currentSeeds) {
                auto it = regionSeeds.find(s);
                if (it != regionSeeds.end() && it->second == regionCount - 1) {
                    it->second = regionCount;
                }
            }
        }

        // remove any seeds that didn't appear in every region
        for (auto it = regionSeeds.begin(); it != regionSeeds.end(); ) {
            if (it->second != regionCount) {
                it = regionSeeds.erase(it);
            } else {
                ++it;
            }
        }
    }

    // regionSeeds now contains only seeds present in all regions
    FILE* outFile = fopen("cakePopulationSeedsInAllRegions.txt", "w");
    if (!outFile) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }
    for (const auto& [s, count] : regionSeeds) {
        fprintf(outFile, "%lld\n", s);
    }*/

    return 0;
}
