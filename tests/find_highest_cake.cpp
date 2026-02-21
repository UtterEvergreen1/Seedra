#include <filesystem>
#include <thread>
#include <fstream>
#include <mutex>
#include <map>
#include <sstream>

#include "common/timer.hpp"
#include "loot/tables/buried_treasure.hpp"
#include "structures/placement/DynamicStructures.hpp"
#include "terrain/generator.hpp"

class SeedVecPos {
public:
    i64 seed;
    std::vector<Pos2D> pos;

    SeedVecPos() : seed(0) {
    }

    SeedVecPos(i64 s, const std::vector<Pos2D> &p) : seed(s), pos(p) {
    }

    bool operator==(const SeedVecPos &other) const {
        return seed == other.seed && pos == other.pos;
    }

    struct Less {
        bool operator()(const SeedVecPos &s1, const SeedVecPos &s2) const {
            if (s1.seed < s2.seed) return true;
            if (s2.seed < s1.seed) return false;
            if (s1.pos.size() < s2.pos.size()) return true;
            if (s2.pos.size() < s1.pos.size()) return false;
            for (size_t i = 0; i < s1.pos.size(); ++i) {
                if (s1.pos[i] < s2.pos[i]) return true;
                if (s2.pos[i] < s1.pos[i]) return false;
            }
            return false; // all positions are equal
        }
    };
};

class SeedPos {
public:
    i64 seed;
    Pos2D pos;

    SeedPos() : seed(0), pos(0, 0) {
    }

    SeedPos(i64 s, Pos2D p) : seed(s), pos(p) {
    }

    bool operator==(const SeedPos &other) const {
        return seed == other.seed && pos == other.pos;
    }

    struct Less {
        bool operator()(const SeedPos &s1, const SeedPos &s2) const {
            if (s1.pos < s2.pos) return true;
            if (s2.pos < s1.pos) return false;
            return s1.seed < s2.seed;
        }
    };
};

void filterCake() {
    std::string outputFile = "cakeResults.txt";

    // Build full list of chunk coords once
    std::vector<std::pair<int, int> > coords;
    for (int x = -24; x <= 24; ++x)
        for (int z = -24; z <= 24; ++z)
            coords.emplace_back(x, z);

    unsigned numThreads = std::thread::hardware_concurrency();
    if (numThreads <= 0) numThreads = 1;

    std::cout << "Partitioning " << coords.size() << " coords among " << numThreads << " threads\n";
    // Partition coords list among threads
    std::vector<std::vector<std::pair<int, int> > > parts(numThreads);
    for (size_t i = 0; i < coords.size(); ++i)
        parts[i % numThreads].push_back(coords[i]);


    // Each thread collects its own set
    std::mutex seedsMutex;
    std::vector<SeedPos> realSeeds(numThreads);
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::cout << "Starting threads to read seeds...\n";
    for (unsigned t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            std::string folderBase = "results/region_";
            for (auto [x, z]: parts[t]) {
                int regionX = x >> 5; // divide by 32 to get region coordinates
                int regionZ = z >> 5;
                std::string nameBase = folderBase +
                                       std::to_string(regionX) + "_" + std::to_string(regionZ) +
                                       "/cakePopulationSeed_";
                std::ifstream file(nameBase +
                                   std::to_string(x) + "_" + std::to_string(z) + ".txt");
                if (!file.is_open()) continue;
                std::string line;
                while (std::getline(file, line)) {
                    if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) continue; // skip blank lines
                    i64 seed = std::stoll(line);
                    if (seed == 0) continue; // skip invalid seeds
                    bool isPossibleChunk = Placement::BuriedTreasure::isPossibleChunkPos(
                        seed, regionX, regionZ, {x, z});
                    if (isPossibleChunk) {
                        Pos2D pos(x, z);
                        seedsMutex.lock();
                        realSeeds.emplace_back(seed, pos);
                        seedsMutex.unlock();
                    }
                }
                file.close();
            }
        });
    }
    for (auto &th: threads) th.join();

    std::cout << "Threads finished, sorting...\n";
    // create the file if it does not exist
    if (!std::filesystem::exists(outputFile)) {
        std::cout << "Creating output file: " << outputFile << std::endl;
        std::ofstream createFile(outputFile);
        createFile.close();
    }

    // Collect unique seeds from all threads and sort them
    std::ranges::sort(realSeeds, SeedPos::Less());

    std::cout << "Writing results to file: " << outputFile << std::endl;
    std::ofstream outFile(outputFile, std::ios::app);
    for (const auto &seedPos: realSeeds) {
        outFile << seedPos.seed << " " << seedPos.pos.x << " " << seedPos.pos.z << "\n";
    }
    outFile.close();
}

void findMostCake() {
    // std::string inputFile = "cakeResults.txt";
    std::string inputFile = "C:/Users/Daniel/CLionProjects/ReversePopulationSeeds/build/cakeResults.txt";
    std::string outputFile = "17cakeStructureSeedsBSort.txt";
    std::string finalOutputFile = "17cakeStructureSeeds.txt";
    std::ifstream file(inputFile);
    std::ofstream outFile(outputFile);
    std::ofstream finalOutFile(finalOutputFile);

    if (!file.is_open()) {
        std::cerr << "Could not open input file: " << inputFile << std::endl;
        return;
    }
    if (!outFile.is_open()) {
        std::cerr << "Could not open output file: " << outputFile << std::endl;
        return;
    }
    if (!finalOutFile.is_open()) {
        std::cerr << "Could not open output file: " << finalOutputFile << std::endl;
        return;
    }
    std::vector<Pos2D> regions = {
        {-1, -1}, {0, 0},
        {0, -1}, {-1, 0}
    };

    std::vector<std::vector<Pos2D> > coords;
    for (const auto &region: regions) {
        std::vector<Pos2D> regionCoords;
        for (int x = region.x * 24; x < (region.x + 1) * 24; ++x) {
            for (int z = region.z * 24; z < (region.z + 1) * 24; ++z) {
                regionCoords.emplace_back(x, z);
            }
        }
        coords.push_back(regionCoords);
    }

    int seedCount = 0;
    Timer timer;
    i64 seed;
    int x, z;
    int maxThreads = (int) regions.size();
    std::atomic_bool invalidSeed = false;
    std::vector<SeedVecPos> validSeeds;
    std::mutex validSeedsMutex;
    while (file >> seed >> x >> z) {
        seedCount++;
        if EXPECT_FALSE (seedCount % 100000 == 0) {
            std::cout << "Processed " << seedCount << " seeds so far." << std::endl;
            std::cout << "Seeds per second: " << 100000.0f / timer.getSeconds() << std::endl;
            timer.reset();
        }
        int currentRegionX = x >> 5; // divide by 32 to get region coordinates
        int currentRegionZ = z >> 5;

        // Create a new vector for each iteration to avoid race conditions
        auto validCoords = std::make_shared<std::vector<Pos2D>>();
        invalidSeed.store(false);

        // Use array of unique_ptr to threads to avoid moves
        std::vector<std::unique_ptr<std::thread>> threads(maxThreads);

        for (int t = 0; t < maxThreads; ++t) {
            try {
                threads[t] = std::make_unique<std::thread>([&coords, &regions, validCoords, &validSeedsMutex, &invalidSeed, seed, currentRegionX, currentRegionZ, t, x, z]() {
                    int regionNum = t;
                    int regionX = regions[regionNum].x;
                    int regionZ = regions[regionNum].z;
                    if (regionX == currentRegionX && regionZ == currentRegionZ) {
                        // already done in other filter
                        /*if (!Placement::BuriedTreasure::isPossibleChunkPos(seed, regionX, regionZ, {x, z})) {
                            invalidSeed.store(true);
                            return;
                        }*/
                        std::lock_guard<std::mutex> lock(validSeedsMutex);
                        validCoords->emplace_back(x, z);
                        return;
                    }
                    Pos2DVec_t allPositionsInRegion = Placement::BuriedTreasure::getAllPossibleChunks(seed, regionX, regionZ);

                    loot::Container27 container;
                    bool foundHighCakeInRegion = false;
                    for (auto [chunkX, chunkZ]: coords[regionNum]) {
                        if (invalidSeed.load() == true) {
                            return; // early exit if seed is already invalid
                        }

                        if (std::ranges::find(allPositionsInRegion, Pos2D(chunkX, chunkZ)) == allPositionsInRegion.end()) {
                            continue;
                        }

                        // test if any of the seeds can generate 4+ cakes in the chunk at (chunkX, chunkZ)
                        int cakeCount = 0;
                        container.clear();
                        loot::buried_treasure.getLootFromChunk<loot::GenMode::MOD_NO_SHUF>(container, seed, chunkX, chunkZ);
                        for (int itemAt = 0; itemAt < container.slotCount(); itemAt++) {
                            lce::ItemState item = container.getSlotAt(itemAt);
                            if (item.id == lce::items::ids::CAKE_ID) {
                                ++cakeCount;
                            }
                        }

                        if (cakeCount < 2) continue;
                        foundHighCakeInRegion = true;
                        std::lock_guard<std::mutex> lock(validSeedsMutex);
                        validCoords->emplace_back(chunkX, chunkZ);
                    }
                    if (!foundHighCakeInRegion) {
                        invalidSeed.store(true);
                    }
                });
            } catch (const std::system_error& e) {
                std::cerr << "Failed to create thread " << t << ": " << e.what() << " (code: " << e.code() << ")" << std::endl;
                invalidSeed.store(true);
            }
        }

        // Join all threads
        for (int t = 0; t < maxThreads; ++t) {
            if (threads[t] && threads[t]->joinable()) {
                try {
                    threads[t]->join();
                } catch (const std::system_error& e) {
                    std::cerr << "Failed to join thread " << t << ": " << e.what() << " (code: " << e.code() << ")" << std::endl;
                    // Try to detach to prevent std::terminate()
                    if (threads[t]->joinable()) {
                        threads[t]->detach();
                    }
                }
            }
        }

        if (invalidSeed.load() == true) continue;

        std::cout << "Seed: " << seed << " has cakes in all regions." << std::endl;
        std::sort(validCoords->begin(), validCoords->end());
        validSeeds.emplace_back(seed, *validCoords);
        outFile << seed;
        for (const auto &pos: *validCoords) {
            outFile << " " << pos.x << " " << pos.z;
        }
        outFile << "\n";
        outFile.flush();
    }

    std::sort(validSeeds.begin(), validSeeds.end(), SeedVecPos::Less());
    for (const auto &seedVecPos: validSeeds) {
        finalOutFile << seedVecPos.seed;
        for (const auto &pos: seedVecPos.pos) {
            finalOutFile << " " << pos.x << " " << pos.z;
        }
        finalOutFile << "\n";
    }
    file.close();
    finalOutFile.close();
}

void checkCakePopulationSeeds() {
    std::string cakeResultsFile = "C:/Users/Daniel/CLionProjects/ReversePopulationSeeds/build/cakeResults.txt";

    std::ifstream cakeFile(cakeResultsFile);
    if (!cakeFile.is_open()) {
        std::cerr << "Could not open cake results file: " << cakeResultsFile << std::endl;
        return;
    }

    // Load all cake results into a multimap (seed -> count)
    std::cout << "Loading cake results..." << std::endl;
    std::map<i64, int> cakeResultsCounts;
    i64 seed;
    int x, z;
    while (cakeFile >> seed >> x >> z) {
        cakeResultsCounts[seed]++;
    }
    cakeFile.close();
    std::cout << "Loaded " << cakeResultsCounts.size() << " unique seeds from cake results." << std::endl;

    // Define the 4 regions we need to check
    std::vector<Pos2D> regions = {
        {-1, -1}, {0, 0},
        {0, -1}, {-1, 0}
    };

    // Loop through all region pairs (n^2 style)
    for (const auto& region1 : regions) {
        for (const auto& region2 : regions) {
            std::string filename = "cakePopulationSeedsIn_" +
                                   std::to_string(region1.x) + "_" + std::to_string(region1.z) +
                                   "__" +
                                   std::to_string(region2.x) + "_" + std::to_string(region2.z) + ".txt";

            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cout << "Could not open file: " << filename << " (skipping)" << std::endl;
                continue;
            }

            std::cout << "\nProcessing file: " << filename << std::endl;

            // Read all seeds from this file and check against cakeResults
            int totalSeeds = 0;
            int foundSeeds = 0;
            std::map<int, int> countDistribution; // count -> how many seeds have that count

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) continue;

                i64 currentSeed = std::stoll(line);
                if (currentSeed == 0) continue;

                totalSeeds++;

                // Check if this seed exists in cakeResults
                auto it = cakeResultsCounts.find(currentSeed);
                if (it != cakeResultsCounts.end()) {
                    foundSeeds++;
                    int count = it->second;
                    std::cout << "  Seed " << currentSeed << " found in cakeResults with count: " << count << std::endl;
                    countDistribution[count]++;
                }
            }
            file.close();

            // Output statistics for this file
            std::cout << "  Total seeds in file: " << totalSeeds << std::endl;
            std::cout << "  Seeds found in cakeResults: " << foundSeeds << std::endl;
            std::cout << "  Match percentage: " << (totalSeeds > 0 ? (100.0 * foundSeeds / totalSeeds) : 0.0) << "%" << std::endl;

            if (!countDistribution.empty()) {
                std::cout << "  Distribution of occurrence counts:" << std::endl;
                for (const auto& [count, numSeeds] : countDistribution) {
                    std::cout << "    " << numSeeds << " seeds appear " << count << " time(s) in cakeResults" << std::endl;
                }
            } else if (foundSeeds > 0) {
                std::cout << "  (No distribution data available)" << std::endl;
            }
        }
    }

    std::cout << "\nCheck complete!" << std::endl;
}

void reconstructCakeSeeds() {
    std::string inputFile = "17cakeStructureSeeds.txt";
    std::string cakeResultsFile = "C:/Users/Daniel/CLionProjects/ReversePopulationSeeds/build/cakeResults.txt";
    std::string outputFile = "17cakeStructureSeedsComplete.txt";

    std::ifstream file(inputFile);
    std::ifstream cakeFile(cakeResultsFile);
    std::ofstream outFile(outputFile);

    if (!file.is_open()) {
        std::cerr << "Could not open input file: " << inputFile << std::endl;
        return;
    }
    if (!cakeFile.is_open()) {
        std::cerr << "Could not open cake results file: " << cakeResultsFile << std::endl;
        return;
    }
    if (!outFile.is_open()) {
        std::cerr << "Could not open output file: " << outputFile << std::endl;
        return;
    }

    // Load all cake results into a multimap (seed -> position)
    std::cout << "Loading cake results..." << std::endl;
    std::multimap<i64, Pos2D> cakeResultsMap;
    i64 seed;
    int x, z;
    while (cakeFile >> seed >> x >> z) {
        cakeResultsMap.emplace(seed, Pos2D(x, z));
    }
    cakeFile.close();
    std::cout << "Loaded " << cakeResultsMap.size() << " cake results." << std::endl;

    // Read each line from 17cakeStructureSeeds.txt and reconstruct with all positions
    std::vector<SeedVecPos> allSeeds;
    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;
        if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) continue;

        std::istringstream iss(line);
        i64 structureSeed;
        iss >> structureSeed;

        // Read existing chunk coordinates from the line (these are the ones from other regions)
        std::vector<Pos2D> allPositions;
        while (iss >> x >> z) {
            allPositions.emplace_back(x, z);
        }

        // Find all matching positions from cakeResults for this seed
        auto range = cakeResultsMap.equal_range(structureSeed);
        for (auto it = range.first; it != range.second; ++it) {
            allPositions.push_back(it->second);
        }

        if (allPositions.empty()) {
            std::cerr << "Warning: No positions found for seed " << structureSeed << std::endl;
            continue;
        }

        // Sort positions before adding
        std::sort(allPositions.begin(), allPositions.end());
        allSeeds.emplace_back(structureSeed, allPositions);
    }
    file.close();

    std::cout << "Processing complete. Sorting " << allSeeds.size() << " seeds..." << std::endl;

    // Sort all seeds
    std::sort(allSeeds.begin(), allSeeds.end(), SeedVecPos::Less());

    // Write sorted results to output file
    std::cout << "Writing results to: " << outputFile << std::endl;
    for (const auto& seedVecPos : allSeeds) {
        outFile << seedVecPos.seed;
        for (const auto& pos : seedVecPos.pos) {
            outFile << " " << pos.x << " " << pos.z;
        }
        outFile << "\n";
    }
    outFile.close();

    std::cout << "Reconstruction complete! Results written to: " << outputFile << std::endl;
}

void findValidCake() {
    // std::string inputFile = "17cakeStructureSeeds.txt";
    std::string inputFile = "2_5cakeStructureSeeds.txt";
    // std::string outputFile = "validSisterSeeds.txt";
    std::string outputFile = "2_5cake_validSisterSeeds.txt";

    std::ifstream file(inputFile);
    std::ofstream outFile(outputFile);

    if (!file.is_open()) {
        std::cerr << "Could not open input file: " << inputFile << std::endl;
        return;
    }
    if (!outFile.is_open()) {
        std::cerr << "Could not open output file: " << outputFile << std::endl;
        return;
    }

    // Define the 4 regions we need to check
    std::vector<Pos2D> regions = {
        {-1, -1}, {0, 0},
        {0, -1}, {-1, 0}
    };

    constexpr i64 maxSisterSeed = 0x7FFF000000000000;

    Generator g(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::LARGE);
    // g.generateCache(4);

    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;
        if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) continue;

        std::istringstream iss(line);
        i64 structureSeed;
        iss >> structureSeed;

        // Read all chunk coordinates from the line
        // Group them by region
        std::map<Pos2D, std::vector<Pos2D>> chunksByRegion;
        int x, z;
        while (iss >> x >> z) {
            Pos2D chunkPos(x, z);
            int regionX = x >> 5; // divide by 32 to get region coordinates
            int regionZ = z >> 5;
            Pos2D regionPos(regionX, regionZ);
            chunksByRegion[regionPos].push_back(chunkPos);
        }

        // Verify we have all 4 regions
        /*bool hasAllRegions = true;
        for (const auto& region : regions) {
            if (chunksByRegion.find(region) == chunksByRegion.end()) {
                std::cerr << "Warning: Line " << lineCount << " missing region ("
                          << region.x << ", " << region.z << ")" << std::endl;
                hasAllRegions = false;
                break;
            }
        }

        if (!hasAllRegions) continue;*/

        std::cout << "Processing structure seed: " << structureSeed
                  << " (line " << lineCount << ")" << std::endl;

        // Search through sister seeds
        for (auto seed = (int64_t)((uint64_t)(structureSeed & 0xFFFFFFFFFFFFLL) | 0x8000000000000000ULL);
             seed <= maxSisterSeed; seed += 0x1000000000000) {

            g.applyWorldSeed(seed);

            bool validSeed = true;
            std::vector<Pos2D> matchedPositions; // Store which positions matched

            // Check each region
            for (const auto& region : regions) {
                if (!chunksByRegion.contains(region)) continue;
                Pos2D buriedTreasureChunk = Placement::BuriedTreasure::getPosition(&g, region.x, region.z).toChunkPos();

                // Check if this buried treasure position matches any of the valid chunks for this region
                const auto& validChunks = chunksByRegion[region];
                bool foundMatch = false;
                for (const auto& validChunk : validChunks) {
                    if (buriedTreasureChunk.x == validChunk.x && buriedTreasureChunk.z == validChunk.z) {
                        foundMatch = true;
                        matchedPositions.push_back(buriedTreasureChunk);
                        break;
                    }
                }

                if (!foundMatch) {
                    validSeed = false;
                    break;
                }
            }

            if (validSeed) {
                std::cout << "Found valid sister seed: " << seed << std::endl;
                outFile << seed;
                for (const auto& pos : matchedPositions) {
                    outFile << " " << pos.x << " " << pos.z;
                }
                outFile << "\n";
                outFile.flush();
            }
        }
    }

    file.close();
    outFile.close();
    std::cout << "Search complete. Results written to: " << outputFile << std::endl;
}

int main() {
    findValidCake();
    return 0;
}
