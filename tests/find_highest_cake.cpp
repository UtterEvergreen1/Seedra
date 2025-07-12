#include <filesystem>
#include <thread>
#include <fstream>
#include <unordered_set>

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
    std::vector<std::thread> threads;
    threads.reserve(maxThreads);
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
        std::vector<Pos2D> validCoords;
        invalidSeed.store(false);
        threads.clear();
        for (int t = 0; t < maxThreads; ++t) {
            threads.emplace_back([&, t]() {
                int regionNum = t;
                int regionX = regions[regionNum].x;
                int regionZ = regions[regionNum].z;
                if (regionX == currentRegionX && regionZ == currentRegionZ) return;

                loot::Container27 container;
                bool foundHighCakeInRegion = false;
                for (auto [x, z]: coords[regionNum]) {
                    if (invalidSeed.load() == true) {
                        return; // early exit if seed is already invalid
                    }

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
                    if (Placement::BuriedTreasure::isPossibleChunkPos(seed, regionX, regionZ, {x, z})) {
                        foundHighCakeInRegion = true;
                        std::lock_guard<std::mutex> lock(validSeedsMutex);
                        validCoords.emplace_back(x, z);
                    }
                }
                if (!foundHighCakeInRegion) {
                    invalidSeed.store(true);
                }
            });
        }

        for (auto &thread: threads) thread.join();

        if (invalidSeed.load() == true) continue;

        std::cout << "Seed: " << seed << " has cakes in all regions." << std::endl;
        std::sort(validCoords.begin(), validCoords.end());
        validSeeds.emplace_back(seed, validCoords);
        outFile << seed;
        for (const auto &pos: validCoords) {
            outFile << " " << pos.x << " " << pos.z;
        }
        outFile << "\n";
        outFile.flush();
    }

    std::sort(validSeeds.begin(), validSeeds.end(), SeedVecPos::Less());
    for (const auto &seedVecPos: validSeeds) {
        finalOutFile << seedVecPos.seed;
        for (const auto &pos: seedVecPos.pos) {
            finalOutFile << pos.x << " " << pos.z;
        }
        finalOutFile << "\n";
    }
    file.close();
    finalOutFile.close();
}

void findValidCake() {
    constexpr i64 structureSeed = 67674954898411;
    constexpr i64 maxSisterSeed = 0x7FFF000000000000;
    std::vector<Pos2D> regions = {
        {-1, -1}
    };
    std::vector<std::vector<Pos2D>> correctChunks = {
        {{-24, -21}, {-17, 18}},
        {{19, -7}, {15, 7}}
    };

    Generator g(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    g.generateCache(4);
    for (auto seed = (int64_t) ((uint64_t) (structureSeed & 0xFFFFFFFFFFFFLL) | 0x8000000000000000ULL);
         seed <= maxSisterSeed; seed += 0x1000000000000) {
        g.applyWorldSeed(seed);

        for (auto &region: regions) {
            Pos2D &correctChunk = correctChunks[region.x + 1][region.z + 1];
            Pos2D buriedTreasureChunk = Placement::BuriedTreasure::getPosition(&g, region.x, region.z).toChunkPos();
            if (buriedTreasureChunk.x != correctChunk.x || buriedTreasureChunk.z != correctChunk.z) {
                goto INVALID_SEED;
            }
        }
        std::cout << "Seed: " << seed << std::endl;
        INVALID_SEED:
    }
}

int main() {
    findMostCake();
    return 0;
}
