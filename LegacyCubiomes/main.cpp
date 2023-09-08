#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>
#include "LegacyCubiomes/loot/stronghold_library.hpp"
#include "LegacyCubiomes/utils/constants.hpp"
#include "LegacyCubiomes/structures/structure_placement/DynamicStructures.hpp"
#include "LegacyCubiomes/utils/json.hpp"
//#pragma warning(disable : 4996) // stupid VS compiler complains
using json = nlohmann::json;

std::vector<std::thread> runningThreads;
std::queue<int> freeIndexes;

static std::mutex resultsLock;
static std::ofstream resultsFile;
static std::string resultsFileName = "results.txt";

static std::fstream optionsFile;
static std::string optionsFileName = "options.json";
static json optionsData;

/*customization for search*/
static WORLDSIZE worldSize = WORLDSIZE::CLASSIC;
static BIOMESCALE biomeSize = BIOMESCALE::SMALL;
static const int chunkXSearch = 0;
static const int chunkZSearch = 0;
static const int regionXSearch = 0;
static const int regionZSearch = 0;

void updateOptionsFile() {
    optionsFile.open(optionsFileName, std::ios_base::out);
    optionsFile << std::setw(4) << optionsData << std::endl;
    optionsFile.close();
}

// mutithreaded function
void CheckSisterSeeds(int threadIndex, Generator* g) {
    std::vector<int64_t> validSeeds;
    // get the lowest sister seed
    int64_t sisterSeed = g->seed - 9223372036854775808ULL;
    for (; sisterSeed <= 9223090561878065152LL; sisterSeed += 281474976710656) {
        g->applySeed(sisterSeed);
        int id = g->getBiomeAt(1, 8, 8);
        if EXPECT_FALSE(!Structure::Treasure::MAIN_VALID_BIOMES[id])
            continue;

        if EXPECT_FALSE(Structure::Treasure::getPosition(g, 0, 0) != 8)
            continue;
        // add result to vector
        validSeeds.push_back(sisterSeed);
    }

    // write found results to file
    resultsLock.lock();
    resultsFile.open(resultsFileName, std::ios::app);
    for (const int64_t& seed : validSeeds)
        resultsFile << seed << std::endl;
    resultsFile.close();
    freeIndexes.push(threadIndex);
    resultsLock.unlock();
}

void CheckSeed(Generator* g) {
    // check if the base seed can spawn at chunk
    if EXPECT_FALSE(!Structure::Treasure::canSpawnAtChunk(g->seed, chunkXSearch, chunkZSearch, regionXSearch, regionZSearch))
        return;
    int freeIndex = freeIndexes.front();
    runningThreads[freeIndex] = std::thread(CheckSisterSeeds, freeIndex, g);
    freeIndexes.pop();
}

/*find 5 cake seeds from file*/
int main(int argc, char* argv[]) {
    int numThreads = 0;
    int64_t savedOffsetNumber = 0;
    std::ifstream seedList;
    std::string seedListFileName;
    bool restartSearch = (argc > 0) && argv[0] == "-restart";

    if (restartSearch) {
        std::string confirmReset;
        std::cout << "Are you sure you wish you reset the data (Y/N)?" << std::endl;
        std::cin >> confirmReset;
        if (confirmReset == "Y") {
            std::cout << "Resetting data." << std::endl;
        }
        else {
            std::cout << "Reset cancelled." << std::endl;
            restartSearch = false;
        }
    }
    else {
        std::cout << "Run with -restart to reset progress data" << std::endl;
    }

    // check if output file exists or have user input data
    optionsFile.open(optionsFileName);
    if (!optionsFile.is_open() || restartSearch) {
        if(!optionsFile.is_open())
            std::cout << "Output file does not exist yet, please input following data." << std::endl;

        std::cout << "Input seeds file name: ";
        std::cin >> seedListFileName;
        optionsData["FileName"] = seedListFileName;
        updateOptionsFile();
        std::cout << "Number of threads: ";
        std::cin >> numThreads;
        optionsData["ThreadCount"] = numThreads;
        updateOptionsFile();
    }
    else {
        std::cout << "Getting saved progress..." << std::endl;
        try {
            optionsData = json::parse(optionsFile);
        }
        catch (std::exception& e) {
            std::cout << "Error when parsing options json file: " << e.what() << std::endl;
        }
        optionsFile.close();
        bool errorFound = false;
        std::string missing;
        auto& checkpointPointer = optionsData["Checkpoint"];
        if (checkpointPointer != nullptr) {
            auto& numLinePointer = checkpointPointer["Line"];
            if (numLinePointer != nullptr) {
                savedOffsetNumber = numLinePointer;
            }
            else {
                missing = "line number in checkpoint";
                errorFound = true;
            }
        }
        else {
            missing = "checkpoint";
            errorFound = true;
        }
        if (errorFound)
            std::cout << "Saved progress is missing " << missing << ", starting from beginning." << std::endl;

        auto& fileNamePointer = optionsData["FileName"];
        if (fileNamePointer != nullptr) {
            seedListFileName = fileNamePointer;
        }
        else {
            std::cout << "Saved file name is missing, please input file name: ";
            std::cin >> seedListFileName;
            optionsData["FileName"] = seedListFileName;
            updateOptionsFile();
            errorFound = true;
        }
        auto& threadCountPointer = optionsData["ThreadCount"];
        if (threadCountPointer != nullptr) {
            numThreads = threadCountPointer;
        }
        else {
            std::cout << "Thread count is missing, please input number of threads: ";
            std::cin >> numThreads;
            optionsData["ThreadCount"] = numThreads;
            updateOptionsFile();
            errorFound = true;
        }
        if (!errorFound)
            std::cout << "Success!" << std::endl;

        optionsFile.close();
    }

    // check if it can run
    if (numThreads < 1) {
        std::cout << "Cannot start with " << numThreads << " threads" << std::endl;
        return 1;
    }

    // open input list of seeds
    seedList.open(seedListFileName);
    if (!seedList.is_open()) {
        std::cout << "Input file does not exist." << std::endl;
        std::cout << "Please run with -restart to reset the values or modify the json file to match the input file name." << std::endl;
        return 1;
    }

    // skip to offset saved
    seedList.seekg(savedOffsetNumber);

    // set up for buried treasure placement
    Structure::Treasure::setup(worldSize);

    // set up for multithread
    int64_t worldSeed = 0;
    bool saveCheckpoint = false;
    std::vector<Generator*> generators = std::vector<Generator*>(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        generators[i] = new Generator(WIIU_LATEST, worldSize, biomeSize);
    }
    runningThreads = std::vector<std::thread>(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        freeIndexes.push(i);
    }

    while (!seedList.eof()) {
        if EXPECT_FALSE(freeIndexes.empty()){
            for (std::thread& thread : runningThreads)
            {
                if(thread.joinable())
                    thread.join();
            }
            saveCheckpoint = true;
        }

        //output checkpoint
        if EXPECT_FALSE(saveCheckpoint) {
            optionsData["Checkpoint"] = {
                { "Line", (int64_t)seedList.tellg() },
                { "LastSeed", worldSeed }
            };
            updateOptionsFile();
            saveCheckpoint = false;
        }
        seedList >> worldSeed;

        int freeIndex = freeIndexes.front();
        generators[freeIndex]->applySeed(worldSeed);
        CheckSeed(generators[freeIndex]);
    }
    // ensure all the threads finish
    for (std::thread& thread : runningThreads)
    {
        if (thread.joinable())
            thread.join();
    }

    // clean up
    for (Generator* g : generators) {
        delete g;
    }

    seedList.close();
    std::cout << "Done!" << std::endl;
    return 0;
}*/

