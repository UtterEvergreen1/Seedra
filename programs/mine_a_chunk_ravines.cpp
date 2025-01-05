
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>
#include <immintrin.h>
#include <set>
#include <thread>

#include "LegacyCubiomes/chunk_detector/RavineDetector.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"
#include "LegacyCubiomes/utils/timer.hpp"


int writeChunkToFile() {
    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::AQUATIC;
    Generator g(console, version, 6172480,
                lce::WORLDSIZE::CLASSIC,
                lce::BIOMESCALE::SMALL);

    int WIDTH = 5;
    int X_OFF = -4;//10 15 or 5 11
    int Z_OFF = 15;



    std::string filename = R"(D:\PycharmProjects\idk\chunks\chunk_data_)"
                           + std::to_string(WIDTH) + ".bin";
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }



    auto* world = new World(&g);
    world->getOrCreateChunk({X_OFF, Z_OFF});
    world->decorateChunks({X_OFF, Z_OFF}, WIDTH);
    for (int cx = -WIDTH + X_OFF; cx < WIDTH + X_OFF + 1; cx++) {
        for (int cz = -WIDTH + Z_OFF; cz < WIDTH + Z_OFF + 1; cz++) {
            auto* chunk = world->getChunk({cx, cz});
            if (!chunk) {
                std::cerr << "Error getting chunk: " << cx << ", " << cz << std::endl;
                continue;
            }



            file.write(reinterpret_cast<const char*>(&cx), sizeof(cx));
            file.write(reinterpret_cast<const char*>(&cz), sizeof(cz));

            file.write(reinterpret_cast<const char*>(chunk->blocks),
                       sizeof(chunk->blocks));

            if (!file) {
                std::cerr << "Error writing to file: " << filename << std::endl;
            } else {
                std::cout << "Chunk (" << cx << ", " << cz
                          << ") written to file: " << filename << std::endl;
            }

        }
    }

    std::cout << "finished!\n";

    file.close();
    return 0;
}





int countZeroValuesSIMD(const ChunkPrimer* chunk) {
    const uint16_t* blocks = chunk->blocks;
    __m256i zero = _mm256_setzero_si256();
    int zeroCount = 0;

    for (int i = 0; i < 65536; i += 256) {

        for (int section = 0; section < 128; section += 16) {
            __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&blocks[i + section]));
            __m256i cmp = _mm256_cmpeq_epi16(data, zero);
            alignas(32) uint16_t result[16];
            _mm256_store_si256(reinterpret_cast<__m256i*>(result), cmp);
            for (unsigned short j : result) {zeroCount += (j == 0xFFFF);}
        }

    }

    zeroCount += 32768;
    return zeroCount;
}




struct SeedsFound {
    int64_t seed;
    uint32_t air;
    int32_t chunkX{};
    int32_t chunkZ{};
    SeedsFound() : seed(0), air(0) {}
};

// ----------------- Thread-safe processing function -----------------

// Shared data protected by a mutex
struct SharedData {
    std::mutex mtx;                         // Protects the following members
    std::vector<SeedsFound>& seedResults;   // Reference to the vector of results
    int& resultIndex;                       // Reference to the ring-buffer index
    int& largestSeedAir;                    // Reference to the best air count so far
    float ravineTestTimeTaken = 0.0f;
    std::ofstream* outputFile;              // Pointer to the output file stream

    // Constructor now takes an additional parameter for the file stream
    SharedData(std::vector<SeedsFound>& sr, int& ri, int& la, std::ofstream* of)
        : seedResults(sr), resultIndex(ri), largestSeedAir(la), outputFile(of) {}
};

std::mutex printMutex;

/**
 * Each thread processes a range [startSeed, endSeed).
 */
void processSeedRange(
        int64_t startSeed,
        int64_t endSeed,
        int WIDTH,
        lce::CONSOLE console,
        LCEVERSION version,
        SharedData& shared) {

    for (int64_t worldSeed = startSeed; worldSeed < endSeed; ++worldSeed) {
        // Create generator, get spawn
        Generator g(console, version, worldSeed,
                    lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        RNG rng;
        Pos2D spawn = g.estimateSpawn(rng);

        // Determine chunk offsets
        int X_OFF = spawn.x / 16;
        int Z_OFF = spawn.z / 16;

        int minChunkX = -WIDTH + X_OFF;
        int maxChunkX = WIDTH + X_OFF;
        int minChunkZ = -WIDTH + Z_OFF;
        int maxChunkZ = WIDTH + Z_OFF;

        RavineDetector detector(g);


        int chunkCountAir = 0;
        int chunkXAir = 0;
        int chunkZAir = 0;


        auto allRavineChunks = detector.getAllRavinesInRange(
                minChunkX, maxChunkX,
                minChunkZ, maxChunkZ,
                true
        );


        {
            std::lock_guard<std::mutex> lock(shared.mtx);
            shared.ravineTestTimeTaken += detector.myTimeTaken;
        }


        // Evaluate each chunk in this (2*WIDTH+1)x(2*WIDTH+1) region
        for (auto& pos : allRavineChunks) {
            auto* chunk = Chunk::provideChunk(g, pos.x, pos.z);
            if (!chunk) {
                std::cerr << "Error getting chunk: " << pos.x << ", " << pos.z << std::endl;
                continue;
            }

            int airCount = countZeroValuesSIMD(chunk);
            if (airCount >= chunkCountAir) {
                chunkCountAir = airCount;
                chunkXAir = pos.x;
                chunkZAir = pos.z;
            }

            delete chunk;
        }

        if (chunkCountAir >= 54000) {
            std::lock_guard<std::mutex> lock(shared.mtx);
            if (shared.outputFile && *shared.outputFile) {
                *(shared.outputFile) << worldSeed
                                     << ", " << chunkXAir << ", " << chunkZAir << ", " << chunkCountAir << "\n";
            }
            // std::cout << worldSeed
            //           << " (" << chunkXAir << ", " << chunkZAir << ")" << chunkCountAir << "\n";



            if (chunkCountAir >= shared.largestSeedAir) {
                shared.seedResults[shared.resultIndex].seed   = worldSeed;
                shared.seedResults[shared.resultIndex].air    = chunkCountAir;
                shared.seedResults[shared.resultIndex].chunkX = chunkXAir;
                shared.seedResults[shared.resultIndex].chunkZ = chunkZAir;

                shared.largestSeedAir = chunkCountAir;

                // Move in a round-robin fashion
                shared.resultIndex = (shared.resultIndex + 1) % (int)shared.seedResults.size();
            }
        }
    }
}


int do_stuff() {
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;

    // Settings
    int WIDTH = 5;
    int64_t START_SEED = 0;
    int64_t MAX_WORLD_TO_CHECK = 8'000'000;
    int64_t totalSeeds = (MAX_WORLD_TO_CHECK - START_SEED);
    int OUTPUT_SIZE = 100;


    std::string outputFilename = R"(D:\PycharmProjects\idk\chunks\ravine_output.txt)";
    std::ofstream outputFile(outputFilename, std::ios::out);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFilename << std::endl;
        return -1;
    }

    // Shared result data
    std::vector<SeedsFound> seedResults(OUTPUT_SIZE);
    int resultIndex = 0;
    int largestSeedAir = 0;

    // Prepare for multithreading
    unsigned int numThreads = std::max(1U, std::thread::hardware_concurrency());
    // For a small range, you might not benefit from many threads—but this scales for large.
    // If totalSeeds is huge, chunk it so each thread gets a portion.
    int64_t chunkSize = (totalSeeds + numThreads - 1) / numThreads; // ceiling division

    // Create our SharedData struct
    SharedData shared(seedResults, resultIndex, largestSeedAir, &outputFile);

    // Spawn threads
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    int64_t currentStart = START_SEED;
    for (unsigned int t = 0; t < numThreads; ++t) {
        int64_t rangeStart = currentStart;
        int64_t rangeEnd = std::min(rangeStart + chunkSize, MAX_WORLD_TO_CHECK);

        threads.emplace_back(processSeedRange, rangeStart, rangeEnd, WIDTH, console, version, std::ref(shared));

        currentStart = rangeEnd;
        if (currentStart >= MAX_WORLD_TO_CHECK) {
            break;
        }
    }

    Timer timer;
    std::cout << "Beginning Thread processing.\n";
    for (auto& th: threads) { th.join(); }
    std::cout << "Total Time: " << timer.getSeconds() << std::endl;
    std::cout << "Time Taken to test for ravines: " << shared.ravineTestTimeTaken << std::endl;

    outputFile.close();

    std::cout << "\nFinal top " << OUTPUT_SIZE << " seeds:\n";
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        SeedsFound& obj = seedResults[i];
        std::cout << "Seed: " << obj.seed << " | Chunk: (" << obj.chunkX << ", " << obj.chunkZ << ")"
                  << " | Air: " << obj.air << "\n";
    }

    std::cout << "Finished!\n";
    return 0;
}


std::vector<std::vector<int>> readDataFromFile(const std::string& filename) {
    std::vector<std::vector<int>> data;       // To store the final data
    std::ifstream infile(filename);           // Open the file

    if (!infile.is_open()) {                   // Check if the file was opened successfully
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return data;                           // Return an empty vector if file can't be opened
    }

    std::string line;
    while (std::getline(infile, line)) {       // Read the file line by line
        std::vector<int> row;                  // To store integers of the current line
        std::stringstream ss(line);            // Create a string stream from the line
        std::string token;

        while (std::getline(ss, token, ',')) { // Split the line by commas
            try {
                int number = std::stoi(token);  // Convert each token to an integer
                row.push_back(number);          // Add the integer to the current row
            } catch (const std::invalid_argument& e) {
                std::cerr << "Warning: Invalid number '" << token << "' in line: " << line << std::endl;
                // Handle the error as needed (e.g., skip the line, skip the token, etc.)
            }
        }

        if (!row.empty()) {                     // If the row has data, add it to the main data vector
            data.push_back(row);
        }
    }

    infile.close();                             // Close the file
    return data;                                // Return the populated data vector
}




int main() {
    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    // writeChunkToFile();
    return do_stuff();

    /*
    std::string filename = R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\ravine_sorted.txt)";
    std::vector<std::vector<int>> data = readDataFromFile(filename);


    std::string outputFilename = R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\ravine_step2.txt)";
    std::ofstream outputFile(outputFilename, std::ios::out);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFilename << std::endl;
        return -1;
    }



    for (auto line : data) {
        i64 worldSeed = line[0];
        int chunkX = line[1];
        int chunkZ = line[2];
        Pos2D chunkPos = {chunkX, chunkZ};


        Generator g(console, version, worldSeed,
                    lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);


        auto* chunk = Chunk::provideChunk(g, chunkPos.x, chunkPos.z);
        if (!chunk) {
            std::cerr << "Error getting chunk: " << chunkPos.x
                      << ", " << chunkPos.z << std::endl;
            continue;
        }


        u32 stone = 0;
        u32 water = 0;
        u32 lava = 0;
        u32 air = 0;

        for (unsigned short block : chunk->blocks) {
            switch (block >> 4) {
                case lce::blocks::ids::STONE_ID:
                    stone++;
                    continue;
                case lce::blocks::ids::STILL_WATER_ID:
                    water++;
                    continue;
                case lce::blocks::ids::STILL_LAVA_ID:
                    lava++;
                    continue;
                case lce::blocks::ids::AIR_ID:
                    air++;
                    continue;
            }
        }

        if (outputFile) {
            outputFile << worldSeed << ", "
                       << "chunk:" << chunkPos.toBlockPos() << ", "
                       << "air:" << air << ", "
                       << "lava:" << lava << ", "
                       << "water:" << water << ", "
                       << "stone:" << stone << "\n";

        }




        delete chunk;
    }

    outputFile.close();

    std::cout << "Done!" << std::endl;
     */
}

























