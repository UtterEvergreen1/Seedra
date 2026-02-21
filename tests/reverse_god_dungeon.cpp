// Reverse RNG seeds from god dungeons back to populate seed
// This takes the output from find_god_dungeon and reverses the RNG calls
// to find the populate seed that would generate the dungeon

#include "common/rng.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <iomanip>
#include <cstdint>
#include <set>
#include <map>

// Constants from Java Random
constexpr uint64_t MULTIPLIER = 0x5DEECE66DULL;
constexpr uint64_t ADDEND = 0xBULL;
constexpr uint64_t MASK = (1LL << 48) - 1;
constexpr uint64_t MULTIPLIER_INVERSE = 246154705703781ULL;

struct DungeonSize {
    int halfSizeX;
    int halfSizeZ;
    int floorBlocks;
};

// All 4 possible dungeon sizes with precomputed floor block counts
// Floor blocks are counted in the exact order of WorldGenDungeons::generate
DungeonSize DUNGEON_SIZES[4] = {
    {2, 2, 0}, // 5x5
    {2, 3, 0}, // 5x7
    {3, 2, 0}, // 7x5
    {3, 3, 0}  // 7x7
};

struct GodDungeonCandidate {
    int64_t rngSeed;
    int sizeIdx;
};

// Simple RNG for reversing
class SimpleRNG {
public:
    uint64_t seed;

    SimpleRNG(uint64_t s) : seed(s) {}

    void setSeed(uint64_t s) {
        seed = (s ^ MULTIPLIER) & MASK;
    }

    int next(int bits) {
        seed = (seed * MULTIPLIER + ADDEND) & MASK;
        return (int)(seed >> (48 - bits));
    }

    int nextInt(int bound) {
        if ((bound & -bound) == bound) {
            return (int)((bound * (int64_t)next(31)) >> 31);
        }
        int bits, val;
        do {
            bits = next(31);
            val = bits % bound;
        } while (bits - val + (bound - 1) < 0);
        return val;
    }

    double nextDouble() {
        return (((int64_t)(next(26)) << 27) + next(27)) / (double)(1LL << 53);
    }

    int64_t nextLong() {
        return ((int64_t)next(32) << 32) + next(32);
    }

    void reverse() {
        seed = ((seed - ADDEND) * MULTIPLIER_INVERSE) & MASK;
    }

    void reverseN(int n) {
        for (int i = 0; i < n; i++) {
            reverse();
        }
    }
};

// Count floor blocks that get nextInt(4) rolls for mossy cobblestone
// Based on WorldGenDungeons::generate
int countFloorRolls(int halfSizeX, int halfSizeZ) {
    int minX = -halfSizeX - 1;
    int maxX = halfSizeX + 1;
    int minY = -1;
    int maxY = 4;
    int minZ = -halfSizeZ - 1;
    int maxZ = halfSizeZ + 1;

    int floorRolls = 0;

    // Iterate in same order as WorldGenDungeons::generate
    for (int offsetX = minX; offsetX <= maxX; ++offsetX) {
        for (int offsetY = maxY - 1; offsetY >= minY; --offsetY) {
            for (int offsetZ = minZ; offsetZ <= maxZ; ++offsetZ) {
                // Skip interior blocks
                bool isInterior = (offsetX != minX && offsetY != minY && offsetZ != minZ &&
                                  offsetX != maxX && offsetY != maxY && offsetZ != maxZ);
                if (isInterior) continue;

                // Floor blocks (offsetY == minY) get nextInt(4) roll for mossy cobblestone
                if (offsetY == minY) {
                    floorRolls++;
                }
            }
        }
    }

    return floorRolls;
}

// Initialize dungeon size floor counts
void initDungeonSizes() {
    for (int i = 0; i < 4; i++) {
        DUNGEON_SIZES[i].floorBlocks = countFloorRolls(
            DUNGEON_SIZES[i].halfSizeX,
            DUNGEON_SIZES[i].halfSizeZ
        );
    }
}

// Verify forward generation matches expected RNG seed
bool verifyForwardGeneration(uint64_t populateSeed, int expectedSizeIdx,
                              int64_t expectedRngSeed, int dungeonAttempt,
                              bool hasWaterLake, bool hasLavaLake, bool desertBiome) {
    SimpleRNG rng(0);
    rng.seed = populateSeed;

    // Water lake check
    // Code: if (biomeAt == desert || biomeAt == desert_hills || rng.nextInt(4) != 0) return {};
    // For god dungeon to exist, water lake must NOT have successfully generated
    if (desertBiome) {
        // Desert: Short-circuit evaluation means nextInt(4) is NEVER called!
        // No RNG consumption at all for desert biomes
    } else {
        // Non-desert biome: nextInt(4) IS called
        // Water lake can attempt but must not have succeeded for god dungeon to exist
        int waterCheck = rng.nextInt(4);
        if (waterCheck == 0) {
            // Position calculated - generation may have been attempted but failed
            rng.nextInt(16); // x
            rng.nextInt(128); // y
            rng.nextInt(16); // z
            // Note: We don't simulate actual water lake generation (too complex)
            // We assume it failed validation if god dungeon exists
        }
        // else nextInt(4) != 0 and water lake is skipped
    }

    // Lava lake check
    if (hasLavaLake) {
        if (rng.nextInt(8) == 0) {
            rng.nextInt(16); // x
            int y = rng.nextInt(rng.nextInt(120) + 8); // y with double nextInt
            rng.nextInt(16); // z

            // Check if rejected due to y >= 63
            if (y >= 63 && rng.nextInt(10) != 0) {
                // Lava lake rejected, continue
            } else if (y >= 63 && rng.nextInt(10) == 0) {
                // Lava lake would generate
                return false;
            } else if (y < 63) {
                // Lava lake would generate
                return false;
            }
        } else {
            return false; // Should check lava but doesn't
        }
    } else {
        // Lava lake skipped
        if (rng.nextInt(8) != 0) {
            // Skipped correctly
        } else {
            return false; // Would generate lava
        }
    }

    // Dungeon attempts (8 total)
    for (int i = 0; i < 8; i++) {
        uint64_t beforeDungeon = rng.seed;

        rng.nextInt(16); // x offset
        rng.nextInt(128); // y
        rng.nextInt(16); // z offset

        // Generate dungeon size
        int halfSizeX = rng.nextInt(2) + 2;
        int halfSizeZ = rng.nextInt(2) + 2;

        const DungeonSize& expectedSize = DUNGEON_SIZES[expectedSizeIdx];

        if (i == dungeonAttempt &&
            halfSizeX == expectedSize.halfSizeX &&
            halfSizeZ == expectedSize.halfSizeZ) {

            // Skip air check (we ignore for now)
            // Do floor generation
            int floorRolls = expectedSize.floorBlocks;
            for (int j = 0; j < floorRolls; j++) {
                rng.nextInt(4);
            }

            // Check if we match expected RNG seed (before chest placement)
            if (rng.seed == (uint64_t)expectedRngSeed) {
                return true;
            }
            return false;
        }
    }

    return false;
}

// Generate all possible reversed populate seeds for a god dungeon
// Returns a map of seed -> dungeon attempt index
std::map<uint64_t, int> reverseGodDungeonWithIndex(int64_t dungeonRngSeed, int sizeIdx) {
    std::map<uint64_t, int> results; // seed -> dungeon index

    const DungeonSize& size = DUNGEON_SIZES[sizeIdx];

    // Start from the dungeon RNG seed (before chest placement)
    SimpleRNG rng(0);
    rng.seed = (uint64_t)dungeonRngSeed;

    // Reverse floor mossy cobblestone rolls
    rng.reverseN(size.floorBlocks);

    // Reverse dungeon size rolls (2 rolls)
    rng.reverseN(2);

    // Reverse dungeon position rolls (3 rolls: x, y, z)
    rng.reverseN(3);

    // Now we're at the point before this dungeon attempt
    // The dungeon could be attempt 0-7
    // We need to reverse 0-7 failed dungeon attempts

    for (int dungeonAttempt = 0; dungeonAttempt < 8; dungeonAttempt++) {
        SimpleRNG beforeThisAttempt = rng;

        // Reverse previous failed dungeon attempts
        // Each failed attempt: 3 position rolls + 2 size rolls = 5 rolls
        beforeThisAttempt.reverseN(dungeonAttempt * 5);

        // Now reverse lava lake
        // Multiple possibilities:
        // 1. No lava lake (nextInt(8) != 0) - 1 roll back
        // 2. Lava lake rejected (y >= 63, nextInt(10) != 0) - 6 rolls back

        std::vector<SimpleRNG> afterLavaReverse;

        // Path 1: No lava lake (nextInt(8) != 0)
        SimpleRNG lavaPath1 = beforeThisAttempt;
        lavaPath1.reverseN(1); // reverse nextInt(8) != 0
        afterLavaReverse.push_back(lavaPath1);

        // Path 2: Lava lake rejected (y >= 63, nextInt(10) != 0)
        SimpleRNG lavaPath2 = beforeThisAttempt;
        lavaPath2.reverseN(1); // reverse nextInt(10)
        lavaPath2.reverseN(1); // reverse z
        lavaPath2.reverseN(2); // reverse y (nextInt(nextInt(120)+8) = 2 calls)
        lavaPath2.reverseN(1); // reverse x
        lavaPath2.reverseN(1); // reverse nextInt(8) == 0
        afterLavaReverse.push_back(lavaPath2);

        // Now reverse water lake paths
        // Code: if (biomeAt == desert || biomeAt == desert_hills || rng.nextInt(4) != 0) return {};
        // For god dungeon to exist, water lake must NOT have generated successfully
        //
        // DESERT BIOMES: Short-circuit means NO RNG consumed at all!
        // NON-DESERT BIOMES: nextInt(4) is called, may also roll position

        for (const SimpleRNG& afterLava : afterLavaReverse) {
            // Desert path: NO water lake RNG consumption (biome check short-circuits)
            results[afterLava.seed] = dungeonAttempt;

            // Non-desert path 1: nextInt(4) != 0 (1 roll back)
            SimpleRNG nonDesertPath1 = afterLava;
            nonDesertPath1.reverseN(1); // reverse nextInt(4) != 0
            results[nonDesertPath1.seed] = dungeonAttempt;

            // Non-desert path 2: nextInt(4) == 0, position calculated (4 rolls back)
            // Generation failed or we wouldn't have a dungeon
            SimpleRNG nonDesertPath2 = afterLava;
            nonDesertPath2.reverseN(1); // reverse z
            nonDesertPath2.reverseN(1); // reverse y
            nonDesertPath2.reverseN(1); // reverse x
            nonDesertPath2.reverseN(1); // reverse nextInt(4) == 0
            results[nonDesertPath2.seed] = dungeonAttempt;

            // Note: We don't reverse water lake generation itself (too complex)
            // because if it generated successfully, dungeon wouldn't exist
        }
    }

    return results;
}

// Filter valid seeds by forward verification
// Returns a map of seed -> dungeon index
std::map<uint64_t, int> filterValidSeeds(const std::map<uint64_t, int>& candidates,
                                          int expectedSizeIdx, int64_t expectedRngSeed,
                                          bool desertBiome) {
    std::map<uint64_t, int> validSeeds;

    for (const auto& [candidateSeed, dungeonIndex] : candidates) {
        // Only try the specific dungeon attempt that this seed corresponds to
        if (desertBiome) {
            // Desert: no water lake, various lava states
            if (verifyForwardGeneration(candidateSeed, expectedSizeIdx, expectedRngSeed,
                                       dungeonIndex, false, false, true)) {
                uint64_t resultPopulateSeed = (candidateSeed ^ MULTIPLIER) & MASK;
                validSeeds[resultPopulateSeed] = dungeonIndex;
                continue;
            }
            if (verifyForwardGeneration(candidateSeed, expectedSizeIdx, expectedRngSeed,
                                       dungeonIndex, false, true, true)) {
                uint64_t resultPopulateSeed = (candidateSeed ^ MULTIPLIER) & MASK;
                validSeeds[resultPopulateSeed] = dungeonIndex;
                continue;
            }
        } else {
            // Non-desert: can have water lakes
            // Try: no water, no lava
            if (verifyForwardGeneration(candidateSeed, expectedSizeIdx, expectedRngSeed,
                                       dungeonIndex, false, false, false)) {
                uint64_t resultPopulateSeed = (candidateSeed ^ MULTIPLIER) & MASK;
                validSeeds[resultPopulateSeed] = dungeonIndex;
                continue;
            }
            // Try: no water, with lava rejection
            if (verifyForwardGeneration(candidateSeed, expectedSizeIdx, expectedRngSeed,
                                       dungeonIndex, false, true, false)) {
                uint64_t resultPopulateSeed = (candidateSeed ^ MULTIPLIER) & MASK;
                validSeeds[resultPopulateSeed] = dungeonIndex;
                continue;
            }
            // Try: with water, no lava
            if (verifyForwardGeneration(candidateSeed, expectedSizeIdx, expectedRngSeed,
                                       dungeonIndex, true, false, false)) {
                uint64_t resultPopulateSeed = (candidateSeed ^ MULTIPLIER) & MASK;
                validSeeds[resultPopulateSeed] = dungeonIndex;
                continue;
            }
            // Try: with water, with lava rejection
            if (verifyForwardGeneration(candidateSeed, expectedSizeIdx, expectedRngSeed,
                                       dungeonIndex, true, true, false)) {
                uint64_t resultPopulateSeed = (candidateSeed ^ MULTIPLIER) & MASK;
                validSeeds[resultPopulateSeed] = dungeonIndex;
                continue;
            }
        }
    }

    return validSeeds;
}

// Parse verified_god_dungeons.txt
std::vector<GodDungeonCandidate> parseVerifiedOutput(const std::string& filename) {
    std::vector<GodDungeonCandidate> candidates;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return candidates;
    }

    std::string line;
    GodDungeonCandidate current{};
    bool hasRngSeed = false;
    bool hasSizeIdx = false;

    std::regex rngSeedDecRegex(R"(RNG Seed \(decimal\):\s*(\d+))");
    std::regex sizeRegex(R"(halfSize:\s*(\d+)x(\d+))");

    while (std::getline(inFile, line)) {
        std::smatch match;

        if (std::regex_search(line, match, rngSeedDecRegex)) {
            current.rngSeed = std::stoll(match[1].str());
            hasRngSeed = true;
        } else if (std::regex_search(line, match, sizeRegex)) {
            int halfX = std::stoi(match[1].str());
            int halfZ = std::stoi(match[2].str());

            for (int i = 0; i < 4; i++) {
                if (DUNGEON_SIZES[i].halfSizeX == halfX &&
                    DUNGEON_SIZES[i].halfSizeZ == halfZ) {
                    current.sizeIdx = i;
                    hasSizeIdx = true;
                    break;
                }
            }
        }

        if (hasRngSeed && hasSizeIdx) {
            candidates.push_back(current);
            hasRngSeed = hasSizeIdx = false;
        }
    }

    inFile.close();
    return candidates;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "God Dungeon RNG Reverser" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    initDungeonSizes();

    std::cout << "Dungeon size floor roll counts:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "  " << (DUNGEON_SIZES[i].halfSizeX * 2 + 1) << "x"
                  << (DUNGEON_SIZES[i].halfSizeZ * 2 + 1) << " ("
                  << DUNGEON_SIZES[i].halfSizeX << "x" << DUNGEON_SIZES[i].halfSizeZ
                  << "): " << DUNGEON_SIZES[i].floorBlocks << " rolls" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Reading verified god dungeons..." << std::endl;
    std::vector<GodDungeonCandidate> candidates = parseVerifiedOutput("verified_god_dungeons.txt");

    if (candidates.empty()) {
        std::cerr << "No verified god dungeons found!" << std::endl;
        std::cerr << "Run find_god_dungeon first." << std::endl;
        return 1;
    }

    std::cout << "Found " << candidates.size() << " verified god dungeon(s)." << std::endl << std::endl;

    std::ofstream outFileDesert("reversed_populate_seeds_desert.txt");
    std::ofstream outFileNonDesert("reversed_populate_seeds_non_desert.txt");
    std::ofstream outFileAll("reversed_populate_seeds_all.txt");

    if (!outFileDesert.is_open() || !outFileNonDesert.is_open() || !outFileAll.is_open()) {
        std::cerr << "Failed to open output files!" << std::endl;
        return 1;
    }

    outFileDesert << "Reversed Populate Seeds from God Dungeons\n";
    outFileDesert << "=========================================\n";
    outFileDesert << "Desert biomes only (no water lakes)\n\n";

    outFileNonDesert << "Reversed Populate Seeds from God Dungeons\n";
    outFileNonDesert << "=========================================\n";
    outFileNonDesert << "Non-desert biomes (can have water lakes)\n\n";

    outFileAll << "Reversed Populate Seeds from God Dungeons\n";
    outFileAll << "=========================================\n";
    outFileAll << "All biomes (both desert and non-desert)\n\n";

    int totalValidSeedsDesert = 0;
    int totalValidSeedsNonDesert = 0;

    for (size_t i = 0; i < candidates.size(); i++) {
        const auto& candidate = candidates[i];

        std::cout << "Processing god dungeon " << (i + 1) << "/" << candidates.size() << std::endl;
        std::cout << "  RNG Seed: " << candidate.rngSeed << std::endl;
        std::cout << "  Size: " << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeX * 2 + 1) << "x"
                  << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeZ * 2 + 1) << std::endl;

        auto allCandidates = reverseGodDungeonWithIndex(candidate.rngSeed, candidate.sizeIdx);
        std::cout << "  Generated " << allCandidates.size() << " candidate populate seeds" << std::endl;

        // Filter for desert biomes
        auto validSeedsDesert = filterValidSeeds(allCandidates, candidate.sizeIdx, candidate.rngSeed, true);
        std::cout << "  Valid desert seeds: " << validSeedsDesert.size() << std::endl;

        // Filter for non-desert biomes
        auto validSeedsNonDesert = filterValidSeeds(allCandidates, candidate.sizeIdx, candidate.rngSeed, false);
        std::cout << "  Valid non-desert seeds: " << validSeedsNonDesert.size() << std::endl;

        // Write desert results
        if (!validSeedsDesert.empty()) {
            outFileDesert << "God Dungeon #" << (i + 1) << "\n";
            outFileDesert << "  RNG Seed: " << candidate.rngSeed << " (0x" << std::hex
                    << candidate.rngSeed << std::dec << ")\n";
            outFileDesert << "  Size: " << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeX * 2 + 1) << "x"
                    << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeZ * 2 + 1) << "\n";
            outFileDesert << "  Populate Seeds:\n";

            for (const auto& [seed, dungeonIndex] : validSeedsDesert) {
                outFileDesert << "    " << seed << " (0x" << std::hex << seed << std::dec
                             << ") [Dungeon Index: " << dungeonIndex << "]\n";
                totalValidSeedsDesert++;
            }
            outFileDesert << "\n";
        }

        // Write non-desert results
        if (!validSeedsNonDesert.empty()) {
            outFileNonDesert << "God Dungeon #" << (i + 1) << "\n";
            outFileNonDesert << "  RNG Seed: " << candidate.rngSeed << " (0x" << std::hex
                    << candidate.rngSeed << std::dec << ")\n";
            outFileNonDesert << "  Size: " << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeX * 2 + 1) << "x"
                    << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeZ * 2 + 1) << "\n";
            outFileNonDesert << "  Populate Seeds:\n";

            for (const auto& [seed, dungeonIndex] : validSeedsNonDesert) {
                outFileNonDesert << "    " << seed << " (0x" << std::hex << seed << std::dec
                                << ") [Dungeon Index: " << dungeonIndex << "]\n";
                totalValidSeedsNonDesert++;
            }
            outFileNonDesert << "\n";
        }

        // Write combined results
        if (!validSeedsDesert.empty() || !validSeedsNonDesert.empty()) {
            outFileAll << "God Dungeon #" << (i + 1) << "\n";
            outFileAll << "  RNG Seed: " << candidate.rngSeed << " (0x" << std::hex
                    << candidate.rngSeed << std::dec << ")\n";
            outFileAll << "  Size: " << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeX * 2 + 1) << "x"
                    << (DUNGEON_SIZES[candidate.sizeIdx].halfSizeZ * 2 + 1) << "\n";

            if (!validSeedsDesert.empty()) {
                outFileAll << "  Desert Populate Seeds (" << validSeedsDesert.size() << "):\n";
                for (const auto& [seed, dungeonIndex] : validSeedsDesert) {
                    outFileAll << "    " << seed << " (0x" << std::hex << seed << std::dec
                              << ") [Dungeon Index: " << dungeonIndex << "]\n";
                }
            }

            if (!validSeedsNonDesert.empty()) {
                outFileAll << "  Non-Desert Populate Seeds (" << validSeedsNonDesert.size() << "):\n";
                for (const auto& [seed, dungeonIndex] : validSeedsNonDesert) {
                    outFileAll << "    " << seed << " (0x" << std::hex << seed << std::dec
                              << ") [Dungeon Index: " << dungeonIndex << "]\n";
                }
            }
            outFileAll << "\n";
        }

        std::cout << std::endl;
    }

    outFileDesert << "=========================================\n";
    outFileDesert << "Total valid populate seeds: " << totalValidSeedsDesert << "\n";
    outFileDesert.close();

    outFileNonDesert << "=========================================\n";
    outFileNonDesert << "Total valid populate seeds: " << totalValidSeedsNonDesert << "\n";
    outFileNonDesert.close();

    outFileAll << "=========================================\n";
    outFileAll << "Total desert seeds: " << totalValidSeedsDesert << "\n";
    outFileAll << "Total non-desert seeds: " << totalValidSeedsNonDesert << "\n";
    outFileAll << "Total all seeds: " << (totalValidSeedsDesert + totalValidSeedsNonDesert) << "\n";
    outFileAll.close();

    std::cout << "========================================" << std::endl;
    std::cout << "Reversal complete!" << std::endl;
    std::cout << "Desert seeds: " << totalValidSeedsDesert << std::endl;
    std::cout << "Non-desert seeds: " << totalValidSeedsNonDesert << std::endl;
    std::cout << "Total seeds: " << (totalValidSeedsDesert + totalValidSeedsNonDesert) << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Results written to:" << std::endl;
    std::cout << "  - reversed_populate_seeds_desert.txt" << std::endl;
    std::cout << "  - reversed_populate_seeds_non_desert.txt" << std::endl;
    std::cout << "  - reversed_populate_seeds_all.txt" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}

