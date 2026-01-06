// Reverse world seeds from validated god dungeon populate seeds
// Takes the output from truly_valid_seeds.txt and searches for world seeds
// that would generate these populate seeds in chunks from -27 to 27

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cuda_runtime.h>

#include "reverser/PopulationReverser.cuh"
#include "reverser/rng.cuh"

constexpr int MAX_PER_SEED = 8;

struct SeedPos {
    int64_t seed;
    int chunkX;
    int chunkZ;
    int dungeonIndex;
    int dungeonX;
    int dungeonY;
    int dungeonZ;
    bool isDesert;

    __host__ __device__ SeedPos() : seed(0), chunkX(0), chunkZ(0), dungeonIndex(0),
                                     dungeonX(0), dungeonY(0), dungeonZ(0),
                                     isDesert(true) {}

    __host__ __device__ SeedPos(int64_t s, int x, int z, int idx, int dx, int dy, int dz, bool desert)
        : seed(s), chunkX(x), chunkZ(z), dungeonIndex(idx),
          dungeonX(dx), dungeonY(dy), dungeonZ(dz), isDesert(desert) {}

    __host__ bool operator==(const SeedPos &other) const {
        return seed == other.seed && chunkX == other.chunkX && chunkZ == other.chunkZ;
    }

    __host__ friend std::ostream& operator<<(std::ostream &os, const SeedPos &sp) {
        return os << sp.seed << " " << sp.chunkX << " " << sp.chunkZ << " "
                  << sp.dungeonIndex << " " << sp.dungeonX << " " << sp.dungeonY << " " << sp.dungeonZ
                  << " " << (sp.isDesert ? "Desert" : "Non-Desert");
    }

    struct Less {
        __host__ bool operator()(const SeedPos &s1, const SeedPos &s2) const {
            if (s1.chunkX < s2.chunkX) return true;
            if (s1.chunkX > s2.chunkX) return false;
            if (s1.chunkZ < s2.chunkZ) return true;
            if (s1.chunkZ > s2.chunkZ) return false;
            return s1.seed < s2.seed;
        }
    };
};

struct ChunkCoord {
    int x;
    int z;

    __host__ __device__ ChunkCoord() : x(0), z(0) {}
    __host__ __device__ ChunkCoord(int _x, int _z) : x(_x), z(_z) {}
};

// RNG constants
constexpr uint64_t RNG_MULTIPLIER = 0x5DEECE66DULL;
constexpr uint64_t RNG_ADDEND = 0xBULL;
constexpr uint64_t RNG_MASK = (1ULL << 48) - 1;
constexpr uint64_t RNG_MULT_INV = 246154705703781ULL;

// Helper to check CUDA calls
static void checkCuda(cudaError_t err, const char* msg) {
    if (err != cudaSuccess) {
        std::fprintf(stderr, "%s: %s\n", msg, cudaGetErrorString(err));
        std::exit(1);
    }
}

__global__ void processSeedsKernel(uint64_t populateSeed,
                                   int dungeonIndex,
                                   int dungeonX,
                                   int dungeonY,
                                   int dungeonZ,
                                   bool isDesert,
                                   const ChunkCoord* chunkCoords,
                                   int numCoords,
                                   SeedPos* outSeeds,
                                   int* outCount,
                                   int maxTotal)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= numCoords) return;

    // Per-thread scratch buffer
    int64_t buf[MAX_PER_SEED];
    ChunkCoord coord = chunkCoords[tid];
    int cnt = PopulationReverser::getSeedsFromChunkSeed(populateSeed, coord.x, coord.z, buf);

    // Reserve space atomically
    int base = atomicAdd(outCount, cnt);
    for (int i = 0; i < cnt; ++i) {
        int idx = base + i;
        if (idx < maxTotal) {
            outSeeds[idx] = SeedPos(buf[i], coord.x, coord.z, dungeonIndex,
                                    dungeonX, dungeonY, dungeonZ, isDesert);
        }
    }
}

struct ValidatedDungeon {
    uint64_t populateSeed;
    int dungeonIndex;
    int godDungeonNum;
    bool isDesert;
    int dungeonX;
    int dungeonY;
    int dungeonZ;
};

std::vector<ValidatedDungeon> parseValidatedSeeds(const std::string& filename) {
    std::vector<ValidatedDungeon> dungeons;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return dungeons;
    }

    std::string line;
    ValidatedDungeon current{};
    current.dungeonX = 0;
    current.dungeonY = 0;
    current.dungeonZ = 0;
    bool hasPopulateSeed = false;
    bool hasDungeonIndex = false;
    bool hasGodDungeonNum = false;
    bool hasDungeonPosition = false;

    while (std::getline(inFile, line)) {
        // Parse "God Dungeon #123 (Desert)" or "God Dungeon #123 (Non-Desert)"
        if (line.find("God Dungeon #") != std::string::npos) {
            size_t hashPos = line.find('#');
            size_t parenPos = line.find('(');
            if (hashPos != std::string::npos && parenPos != std::string::npos) {
                std::string numStr = line.substr(hashPos + 1, parenPos - hashPos - 2);
                current.godDungeonNum = std::stoi(numStr);
                current.isDesert = (line.find("(Desert)") != std::string::npos);
                hasGodDungeonNum = true;
            }
        }
        // Parse "  Populate Seed: 123456789 (0xhex)"
        else if (line.find("Populate Seed:") != std::string::npos) {
            size_t colonPos = line.find(':');
            size_t parenPos = line.find('(');
            if (colonPos != std::string::npos && parenPos != std::string::npos) {
                std::string seedStr = line.substr(colonPos + 1, parenPos - colonPos - 2);
                // Trim whitespace
                seedStr.erase(0, seedStr.find_first_not_of(" \t"));
                current.populateSeed = std::stoull(seedStr);
                hasPopulateSeed = true;
            }
        }
        // Parse "  Dungeon Index: 5"
        else if (line.find("Dungeon Index:") != std::string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string indexStr = line.substr(colonPos + 1);
                // Trim whitespace
                indexStr.erase(0, indexStr.find_first_not_of(" \t"));
                current.dungeonIndex = std::stoi(indexStr);
                hasDungeonIndex = true;
            }
        }
        // Parse "  Dungeon Position: (14, 87, 12)"
        else if (line.find("Dungeon Position:") != std::string::npos) {
            size_t openParen = line.find('(');
            size_t closeParen = line.find(')');
            if (openParen != std::string::npos && closeParen != std::string::npos) {
                std::string posStr = line.substr(openParen + 1, closeParen - openParen - 1);
                // Parse x, y, z
                size_t comma1 = posStr.find(',');
                size_t comma2 = posStr.rfind(',');
                if (comma1 != std::string::npos && comma2 != std::string::npos) {
                    std::string xStr = posStr.substr(0, comma1);
                    std::string yStr = posStr.substr(comma1 + 1, comma2 - comma1 - 1);
                    std::string zStr = posStr.substr(comma2 + 1);
                    // Trim whitespace
                    xStr.erase(0, xStr.find_first_not_of(" \t"));
                    yStr.erase(0, yStr.find_first_not_of(" \t"));
                    zStr.erase(0, zStr.find_first_not_of(" \t"));
                    current.dungeonX = std::stoi(xStr);
                    current.dungeonY = std::stoi(yStr);
                    current.dungeonZ = std::stoi(zStr);
                    hasDungeonPosition = true;
                }
            }
        }

        // When we have all required fields, save the dungeon
        if (hasPopulateSeed && hasDungeonIndex && hasGodDungeonNum && hasDungeonPosition) {
            dungeons.push_back(current);
            hasPopulateSeed = false;
            hasDungeonIndex = false;
            hasGodDungeonNum = false;
            hasDungeonPosition = false;
        }
    }

    inFile.close();
    return dungeons;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "World Seed Reverser for God Dungeons" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    // Read validated populate seeds
    std::cout << "Reading truly_valid_seeds.txt..." << std::endl;
    std::vector<ValidatedDungeon> validatedDungeons = parseValidatedSeeds("truly_valid_seeds.txt");

    if (validatedDungeons.empty()) {
        std::cerr << "No validated dungeons found!" << std::endl;
        std::cerr << "Run validate_reversed_seeds first." << std::endl;
        return 1;
    }

    std::cout << "Found " << validatedDungeons.size() << " validated god dungeon(s)." << std::endl << std::endl;

    // Generate chunk coordinates from -27 to 27
    std::vector<ChunkCoord> coords;
    for (int x = -27; x <= 27; ++x) {
        for (int z = -27; z <= 27; ++z) {
            coords.emplace_back(x, z);
        }
    }

    std::cout << "Searching in " << coords.size() << " chunk coordinates (-27 to 27)..." << std::endl << std::endl;

    size_t N = coords.size();
    int threads = 256;
    int blocks = (N + threads - 1) / threads;
    int maxTotal = N * MAX_PER_SEED;

    std::vector<SeedPos> outputSeeds;

    // Allocate device memory for chunk coordinates (reused for all dungeons)
    ChunkCoord* d_coords = nullptr;
    checkCuda(cudaMalloc(&d_coords, N * sizeof(ChunkCoord)), "cudaMalloc d_coords failed");
    checkCuda(cudaMemcpy(d_coords, coords.data(), N * sizeof(ChunkCoord), cudaMemcpyHostToDevice),
              "cudaMemcpy H2D d_coords failed");

    // Process each validated dungeon
    for (size_t i = 0; i < validatedDungeons.size(); ++i) {
        const auto& dungeon = validatedDungeons[i];

        std::cout << "Processing God Dungeon #" << dungeon.godDungeonNum
                  << " (" << (dungeon.isDesert ? "Desert" : "Non-Desert")
                  << ") [" << (i + 1) << "/" << validatedDungeons.size() << "]..." << std::flush;

        // Allocate output buffer and counter
        SeedPos* d_out = nullptr;
        int* d_count = nullptr;
        checkCuda(cudaMalloc(&d_out, maxTotal * sizeof(SeedPos)), "cudaMalloc d_out failed");
        checkCuda(cudaMalloc(&d_count, sizeof(int)), "cudaMalloc d_count failed");
        checkCuda(cudaMemset(d_count, 0, sizeof(int)), "cudaMemset d_count failed");

        // Launch kernel
        processSeedsKernel<<<blocks, threads>>>(
            dungeon.populateSeed,
            dungeon.dungeonIndex,
            dungeon.dungeonX,
            dungeon.dungeonY,
            dungeon.dungeonZ,
            dungeon.isDesert,
            d_coords,
            (int)N,
            d_out,
            d_count,
            maxTotal
        );
        checkCuda(cudaGetLastError(), "Kernel launch failed");
        checkCuda(cudaDeviceSynchronize(), "cudaDeviceSynchronize failed");

        // Copy back count and results
        int h_count;
        checkCuda(cudaMemcpy(&h_count, d_count, sizeof(int), cudaMemcpyDeviceToHost),
                  "cudaMemcpy D2H d_count failed");

        if (h_count > 0) {
            std::vector<SeedPos> outSeeds(h_count);
            checkCuda(cudaMemcpy(outSeeds.data(), d_out, h_count * sizeof(SeedPos), cudaMemcpyDeviceToHost),
                      "cudaMemcpy D2H d_out failed");

            // Add to output
            outputSeeds.insert(outputSeeds.end(),
                             std::make_move_iterator(outSeeds.begin()),
                             std::make_move_iterator(outSeeds.end()));

            std::cout << " Found " << h_count << " world seed candidates" << std::endl;
        } else {
            std::cout << " No candidates found" << std::endl;
        }

        // Clean up GPU mem
        cudaFree(d_out);
        cudaFree(d_count);
    }

    cudaFree(d_coords);

    std::cout << std::endl;
    std::cout << "Total world seed candidates found: " << outputSeeds.size() << std::endl;

    // Sort and remove duplicates
    // std::cout << "Sorting and removing duplicates..." << std::endl;
    std::cout << "Sorting..." << std::endl;
    std::sort(outputSeeds.begin(), outputSeeds.end(), SeedPos::Less());
    // outputSeeds.erase(std::unique(outputSeeds.begin(), outputSeeds.end()), outputSeeds.end());

    // std::cout << "Unique world seeds: " << outputSeeds.size() << std::endl << std::endl;
    std::cout << "Total world seeds: " << outputSeeds.size() << std::endl << std::endl;

    // Write results
    std::string outputFile = "world_seeds_from_god_dungeons.txt";
    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return 1;
    }

    output << "World Seeds from God Dungeons\n";
    output << "==============================\n";
    output << "Format: WorldSeed ChunkX ChunkZ DungeonIndex DungeonX DungeonY DungeonZ Biome GodDungeonNum\n";
    output << "  WorldSeed: The world seed value\n";
    output << "  ChunkX/Z: Chunk coordinates where dungeon generates\n";
    output << "  DungeonIndex: Which dungeon attempt (0-7)\n";
    output << "  DungeonX/Y/Z: Exact position of dungeon in chunk\n";
    output << "  Biome: Desert or Non-Desert\n";
    output << "  GodDungeonNum: Original god dungeon number from verification\n\n";

    for (const auto &s : outputSeeds) {
        output << s << "\n";
    }

    output.close();

    std::cout << "========================================" << std::endl;
    std::cout << "Results written to: " << outputFile << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}

