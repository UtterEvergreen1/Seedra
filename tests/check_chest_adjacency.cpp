// filepath: c:\Users\Daniel\CLionProjects\LegacyChunkViewer\Seedra\tests\check_chest_adjacency.cpp
// Check if the two chests in verified god dungeons are beside each other
// Takes the output from verified_world_seeds.txt and validates chest positions

#include "terrain/World.hpp"
#include "terrain/generator.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/decorators/WorldGenDungeons.hpp"
#include "loot/tables/simple_dungeon.hpp"
#include "lce/items/itemsInit.hpp"
#include "common/rng.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>

#include "terrain/carve/ChunkGenerator.hpp"

struct VerifiedSeed {
    int64_t worldSeed;
    int chunkX;
    int chunkZ;
    int dungeonIndex;
    int dungeonX;
    int dungeonY;
    int dungeonZ;
    bool isDesert;

    // Results
    bool chestsAdjacent;
    Pos3D chest1Pos;
    Pos3D chest2Pos;
    int distanceBetweenChests;
};

// Custom dungeon generator that tracks chest positions
class ChestTrackingDungeonGen : public WorldGenDungeons {
public:
    std::vector<Pos3D> chestPositions;

    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override {
        using namespace lce::blocks;

        const_cast<ChestTrackingDungeonGen*>(this)->chestPositions.clear();

        // Random dungeon dimensions
        const int halfSizeX = rng.nextInt(2) + 2;
        const int minX = -halfSizeX - 1;
        const int maxX = halfSizeX + 1;
        constexpr int minY = -1;
        constexpr int maxY = 4;
        const int halfSizeZ = rng.nextInt(2) + 2;
        const int minZ = -halfSizeZ - 1;
        const int maxZ = halfSizeZ + 1;

        int airCount = 0;

        // First pass: validate location
        for (int offsetX = minX; offsetX <= maxX; ++offsetX) {
            for (int offsetY = minY; offsetY <= maxY; ++offsetY) {
                for (int offsetZ = minZ; offsetZ <= maxZ; ++offsetZ) {
                    Pos3D blockPos = position.add(offsetX, offsetY, offsetZ);
                    const bool isSolid = lce::blocks::isSolidBlock(worldIn->getBlockId(blockPos));

                    if (offsetY == minY && !isSolid) {
                        return false;
                    }
                    if (offsetY == maxY && !isSolid) {
                        return false;
                    }

                    if ((offsetX == minX || offsetX == maxX || offsetZ == minZ || offsetZ == maxZ) &&
                        offsetY == 0 && worldIn->isAirBlock(blockPos) && worldIn->isAirBlock(blockPos.up())) {
                        ++airCount;
                    }
                }
            }
        }

        if (airCount < 1 || airCount > 5) {
            return false;
        }

        // Second pass: build the dungeon structure
        for (int offsetX = minX; offsetX <= maxX; ++offsetX) {
            for (int offsetY = maxY - 1; offsetY >= minY; --offsetY) {
                for (int offsetZ = minZ; offsetZ <= maxZ; ++offsetZ) {
                    Pos3D blockPos = position.add(offsetX, offsetY, offsetZ);

                    if (offsetX != minX && offsetY != minY && offsetZ != minZ &&
                        offsetX != maxX && offsetY != maxY && offsetZ != maxZ) {
                        if (worldIn->getBlockId(blockPos) != CHEST_ID) {
                            worldIn->setBlockId(blockPos, AIR_ID);
                        }
                    } else if (blockPos.getY() >= 0 && !isSolidBlock(worldIn->getBlockId(blockPos.down()))) {
                        worldIn->setBlockId(blockPos, AIR_ID);
                    } else if (isSolidBlock(worldIn->getBlockId(blockPos)) &&
                               worldIn->getBlockId(blockPos) != CHEST_ID) {
                        if (offsetY == minY && rng.nextInt(4) != 0) {
                            worldIn->setBlockId(blockPos, MOSS_STONE_ID);
                        } else {
                            worldIn->setBlockId(blockPos, COBBLESTONE_ID);
                        }
                    }
                }
            }
        }

        // Try to place up to 2 chests
        for (int chestAttempt = 0; chestAttempt < 2; ++chestAttempt) {
            for (int positionAttempt = 0; positionAttempt < 3; ++positionAttempt) {
                const int chestX = position.getX() + rng.nextInt(halfSizeX * 2 + 1) - halfSizeX;
                const int chestY = position.getY();
                const int chestZ = position.getZ() + rng.nextInt(halfSizeZ * 2 + 1) - halfSizeZ;
                Pos3D chestPos(chestX, chestY, chestZ);

                if (worldIn->isAirBlock(chestPos)) {
                    int adjacentSolidWalls = 0;

                    for (const auto facing: FACING_HORIZONTAL) {
                        if (isSolidBlock(worldIn->getBlockId(chestPos.offset(facing)))) {
                            ++adjacentSolidWalls;
                        }
                    }

                    if (adjacentSolidWalls == 1) {
                        worldIn->setBlockId(chestPos, CHEST_ID);
                        rng.nextLongI(); // Consume loot seed

                        // Track chest position
                        const_cast<ChestTrackingDungeonGen*>(this)->chestPositions.emplace_back(chestX, chestY, chestZ);
                        break;
                    }
                }
            }
        }

        // Place mob spawner
        worldIn->setBlockId(position, MONSTER_SPAWNER_ID);

        return true;
    }
};

// Parse verified_world_seeds.txt
std::vector<VerifiedSeed> parseVerifiedSeeds(const std::string& filename) {
    std::vector<VerifiedSeed> seeds;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return seeds;
    }

    std::string line;

    // Skip header lines
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '=' || line.find("Format:") != std::string::npos ||
            line.find("Verified World Seeds") != std::string::npos) {
            continue;
        }

        // Parse line: WorldSeed ChunkX ChunkZ DungeonIndex DungeonX DungeonY DungeonZ Biome
        std::istringstream iss(line);
        VerifiedSeed seed{};
        std::string biomeStr;

        if (iss >> seed.worldSeed >> seed.chunkX >> seed.chunkZ
            >> seed.dungeonIndex >> seed.dungeonX
            >> seed.dungeonY >> seed.dungeonZ >> biomeStr) {

            seed.isDesert = (biomeStr == "Desert");
            seed.chestsAdjacent = false;
            seed.chest1Pos = Pos3D(0, 0, 0);
            seed.chest2Pos = Pos3D(0, 0, 0);
            seed.distanceBetweenChests = 0;

            seeds.push_back(seed);
        }
    }

    inFile.close();
    return seeds;
}

// Check chest adjacency for a single seed
bool checkChestAdjacency(VerifiedSeed& seed) {
    // Create generator with the world seed
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, seed.worldSeed,
                       lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    World world(&generator);

    // Generate chunks in a 3x3 grid centered on the target chunk
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            int cx = seed.chunkX + dx;
            int cz = seed.chunkZ + dz;
            Pos2D chunkPos(cx, cz);
            auto* caveChunk = new ChunkPrimer();
            std::fill_n(caveChunk->blocks.data(), caveChunk->blocks.size(), lce::blocks::BlockID::STONE_ID << 4);
            world.addChunk(chunkPos, caveChunk);
            caveChunk->stage = Stage::STAGE_CAVES;
            Chunk::populateCaves(world, chunkPos);
        }
    }

    // Get populate seed for this chunk
    RNG rng = RNG::getPopulationSeed(seed.worldSeed, seed.chunkX, seed.chunkZ);

    // Simulate water lake check
    if (seed.isDesert) {
        // Desert: no RNG consumption
    } else {
        int waterCheck = rng.nextInt(4);
        if (waterCheck == 0) {
            rng.nextInt(16);
            rng.nextInt(128);
            rng.nextInt(16);
        }
    }

    // Simulate lava lake check
    if (rng.nextInt(8) == 0) {
        rng.nextInt(16);
        int y = rng.nextInt(rng.nextInt(120) + 8);
        rng.nextInt(16);
        if (y >= 63 && rng.nextInt(10) == 0) {
            return false;
        }
    }

    // Try dungeon generation with chest tracking
    ChestTrackingDungeonGen chestTracker;

    for (int i = 0; i < 8; i++) {
        int xPos = rng.nextInt(16) + 8 + seed.chunkX * 16;
        int yPos = rng.nextInt(128);
        int zPos = rng.nextInt(16) + 8 + seed.chunkZ * 16;

        Pos3D dungeonPos(xPos, yPos, zPos);

        if (i == seed.dungeonIndex) {
            chestTracker.generate(&world, rng, dungeonPos);

            // Check if we got exactly 2 chests
            if (chestTracker.chestPositions.size() == 2) {
                seed.chest1Pos = chestTracker.chestPositions[0];
                seed.chest2Pos = chestTracker.chestPositions[1];

                // Calculate Manhattan distance
                int dx = std::abs(seed.chest2Pos.x - seed.chest1Pos.x);
                int dy = std::abs(seed.chest2Pos.y - seed.chest1Pos.y);
                int dz = std::abs(seed.chest2Pos.z - seed.chest1Pos.z);
                seed.distanceBetweenChests = dx + dy + dz;

                // Chests are adjacent if they are exactly 1 block apart (Manhattan distance = 1)
                seed.chestsAdjacent = (seed.distanceBetweenChests == 1);

                return true;
            }
            return false;
        } else {
            chestTracker.generate(&world, rng, dungeonPos);
        }
    }

    return false;
}

// Check chest adjacency from CUDA god dungeon output (no chunk generation needed)
struct CudaGodDungeonResult {
    int64_t rngSeed;
    int sizeIdx;
    int halfSizeX;
    int halfSizeZ;
    bool chestsAdjacent;
    Pos2D chest1Offset;
    Pos2D chest2Offset;
    int distanceBetweenChests;
};

// Parse god_dungeons.txt from CUDA output
std::vector<CudaGodDungeonResult> parseCudaGodDungeons(const std::string& filename) {
    std::vector<CudaGodDungeonResult> results;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return results;
    }

    std::string line;
    CudaGodDungeonResult current;
    bool readingEntry = false;

    while (std::getline(inFile, line)) {
        // Skip empty lines and header
        if (line.empty() || line.find("==") != std::string::npos ||
            line.find("God Dungeon Search Results") != std::string::npos) {
            continue;
        }

        if (line.find("FOUND GOD DUNGEON #") != std::string::npos) {
            if (readingEntry) {
                results.push_back(current);
            }
            current = CudaGodDungeonResult{};
            readingEntry = true;
        } else if (line.find("RNG Seed:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string seedStr = line.substr(colonPos + 1);
                std::istringstream iss(seedStr);
                iss >> current.rngSeed;
            }
        } else if (line.find("Dungeon size:") != std::string::npos) {
            size_t halfSizePos = line.find("halfSize:");
            if (halfSizePos != std::string::npos) {
                std::string halfSizeStr = line.substr(halfSizePos + 9);
                char separator;
                std::istringstream iss(halfSizeStr);
                iss >> current.halfSizeX >> separator >> current.halfSizeZ;
            }
        }
    }

    // Add the last entry
    if (readingEntry) {
        results.push_back(current);
    }

    inFile.close();
    return results;
}

// Counts how many enchanted golden apples are in a container
int countEnchantedGoldenApples(const loot::Container<27>& container) {
    using namespace lce::items;
    int count = 0;

    for (size_t i = 0; i < container.slotCount(); i++) {
        const lce::ItemState& itemState = container.getSlotAt(i);
        // Enchanted golden apple has ID 322 and data 1
        if (itemState.getID() == ENCHANTED_GOLDEN_APPLE.getState().getID() &&
            itemState.getDataTag() == ENCHANTED_GOLDEN_APPLE.getState().getDataTag()) {
            count += itemState.getCount();
            }
    }

    return count;
}

// Check chest adjacency using CUDA-style logic (no chunk generation)
bool checkCudaChestAdjacency(CudaGodDungeonResult& result) {
    RNG rng(result.rngSeed);

    const int minX = -result.halfSizeX;
    const int maxX = result.halfSizeX;
    const int minZ = -result.halfSizeZ;
    const int maxZ = result.halfSizeZ;

    std::vector<std::pair<Pos2D, uint64_t>> chestOffsets;

    // Simulate chest placement (same logic as CUDA kernel)
    for (int chestAttempt = 0; chestAttempt < 2; ++chestAttempt) {
        for (int positionAttempt = 0; positionAttempt < 3; ++positionAttempt) {
            const int chestOffsetX = rng.nextInt(result.halfSizeX * 2 + 1) - result.halfSizeX;
            const int chestOffsetZ = rng.nextInt(result.halfSizeZ * 2 + 1) - result.halfSizeZ;

            const bool atMinX = (chestOffsetX == minX);
            const bool atMaxX = (chestOffsetX == maxX);
            const bool atMinZ = (chestOffsetZ == minZ);
            const bool atMaxZ = (chestOffsetZ == maxZ);

            const bool nextToXWall = atMinX || atMaxX;
            const bool nextToZWall = atMinZ || atMaxZ;

            // Must be next to exactly one wall (XOR condition)
            if (nextToXWall != nextToZWall) {
                chestOffsets.emplace_back(Pos2D(chestOffsetX, chestOffsetZ), rng.nextLong());
                break;
            }
        }
    }

    // Must have exactly 2 chests
    if (chestOffsets.size() != 2) {
        return false;
    }

    result.chest1Offset = chestOffsets[0].first;
    result.chest2Offset = chestOffsets[1].first;

    // Calculate Manhattan distance (only X and Z, Y is always 0)
    int dx = std::abs(result.chest2Offset.x - result.chest1Offset.x);
    int dz = std::abs(result.chest2Offset.z - result.chest1Offset.z);
    result.distanceBetweenChests = dx + dz;

    // Chests are adjacent if they are exactly 1 block apart
    result.chestsAdjacent = (result.distanceBetweenChests == 1);

    loot::Container<27> container;
    // Verify with actual loot generation
    for (size_t i = 0; i < 2; i++) {
        container.clear();
        loot::simple_dungeon.getLootFromLootTableSeed<loot::GenMode::MODERN>(container, (int64_t)chestOffsets[i].second);

        int appleCount = countEnchantedGoldenApples(container);
        if (appleCount != 3) {
            return false;
        }
    }

    return true;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Chest Adjacency Checker for God Dungeons" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    std::cout << "Select mode:" << std::endl;
    std::cout << "  1. Check verified_world_seeds.txt (full chunk generation)" << std::endl;
    std::cout << "  2. Check god_dungeons.txt (CUDA output, no chunk generation)" << std::endl;
    std::cout << "Enter choice (1 or 2): ";

    int choice;
    std::cin >> choice;
    std::cout << std::endl;

    if (choice == 2) {
        // CUDA mode - check god_dungeons.txt
        std::cout << "Reading god_dungeons.txt..." << std::endl;
        std::vector<CudaGodDungeonResult> cudaResults = parseCudaGodDungeons("god_dungeons.txt");


        if (cudaResults.empty()) {
            std::cerr << "No CUDA god dungeons found!" << std::endl;
            std::cerr << "Run find_god_dungeon_cuda first." << std::endl;
            return 1;
        }

        std::cout << "Found " << cudaResults.size() << " CUDA god dungeon(s)." << std::endl << std::endl;

        // Open output files
        std::ofstream adjacentFile("cuda_adjacent_chests.txt");
        std::ofstream nonAdjacentFile("cuda_non_adjacent_chests.txt");
        std::ofstream summaryFile("cuda_chest_adjacency_summary.txt");

        if (!adjacentFile.is_open() || !nonAdjacentFile.is_open() || !summaryFile.is_open()) {
            std::cerr << "Failed to open output files!" << std::endl;
            return 1;
        }

        adjacentFile << "CUDA God Dungeons with Adjacent Chests\n";
        adjacentFile << "=======================================\n";
        adjacentFile << "Format: RNGSeed HalfSizeX HalfSizeZ Chest1Offset(x,z) Chest2Offset(x,z)\n\n";

        nonAdjacentFile << "CUDA God Dungeons with Non-Adjacent Chests\n";
        nonAdjacentFile << "===========================================\n";
        nonAdjacentFile << "Format: RNGSeed HalfSizeX HalfSizeZ Chest1Offset(x,z) Chest2Offset(x,z) Distance\n\n";

        summaryFile << "CUDA Chest Adjacency Summary\n";
        summaryFile << "============================\n\n";

        int adjacentCount = 0;
        int nonAdjacentCount = 0;
        int failedCount = 0;

        auto startTime = std::chrono::steady_clock::now();

        for (size_t i = 0; i < cudaResults.size(); i++) {
            auto& result = cudaResults[i];

            std::cout << "Checking CUDA result " << (i + 1) << "/" << cudaResults.size()
                      << " (RNG Seed: " << result.rngSeed << ")..." << std::flush;

            bool success = checkCudaChestAdjacency(result);

            if (success) {
                if (result.chestsAdjacent) {
                    adjacentCount++;
                    std::cout << " ✅ ADJACENT (distance=" << result.distanceBetweenChests << ")" << std::endl;

                    adjacentFile << result.rngSeed << " "
                               << result.halfSizeX << " " << result.halfSizeZ << " "
                               << "(" << result.chest1Offset.x << "," << result.chest1Offset.z << ") "
                               << "(" << result.chest2Offset.x << "," << result.chest2Offset.z << ")\n";
                    adjacentFile.flush();

                    summaryFile << "✅ RNG Seed: " << result.rngSeed << "\n";
                    summaryFile << "   Half Size: " << result.halfSizeX << "x" << result.halfSizeZ << "\n";
                    summaryFile << "   Chest 1 Offset: (" << result.chest1Offset.x << ", " << result.chest1Offset.z << ")\n";
                    summaryFile << "   Chest 2 Offset: (" << result.chest2Offset.x << ", " << result.chest2Offset.z << ")\n";
                    summaryFile << "   Distance: " << result.distanceBetweenChests << " (ADJACENT)\n\n";

                } else {
                    nonAdjacentCount++;
                    std::cout << " ⚠️  NON-ADJACENT (distance=" << result.distanceBetweenChests << ")" << std::endl;

                    nonAdjacentFile << result.rngSeed << " "
                                  << result.halfSizeX << " " << result.halfSizeZ << " "
                                  << "(" << result.chest1Offset.x << "," << result.chest1Offset.z << ") "
                                  << "(" << result.chest2Offset.x << "," << result.chest2Offset.z << ") "
                                  << result.distanceBetweenChests << "\n";
                    nonAdjacentFile.flush();

                    summaryFile << "⚠️  RNG Seed: " << result.rngSeed << "\n";
                    summaryFile << "   Half Size: " << result.halfSizeX << "x" << result.halfSizeZ << "\n";
                    summaryFile << "   Chest 1 Offset: (" << result.chest1Offset.x << ", " << result.chest1Offset.z << ")\n";
                    summaryFile << "   Chest 2 Offset: (" << result.chest2Offset.x << ", " << result.chest2Offset.z << ")\n";
                    summaryFile << "   Distance: " << result.distanceBetweenChests << " (NON-ADJACENT)\n\n";
                }
            } else {
                failedCount++;
                std::cout << " ❌ FAILED TO VERIFY" << std::endl;
            }
        }

        auto endTime = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();

        adjacentFile.close();
        nonAdjacentFile.close();

        summaryFile << "============================\n";
        summaryFile << "Total CUDA god dungeons: " << cudaResults.size() << "\n";
        summaryFile << "✅ Adjacent chests: " << adjacentCount << "\n";
        summaryFile << "⚠️  Non-adjacent chests: " << nonAdjacentCount << "\n";
        summaryFile << "❌ Failed to verify: " << failedCount << "\n";
        if (adjacentCount + nonAdjacentCount > 0) {
            summaryFile << "Adjacent percentage: " << std::fixed << std::setprecision(2)
                       << (100.0 * adjacentCount / (adjacentCount + nonAdjacentCount)) << "%\n";
        }
        summaryFile << "Time elapsed: " << elapsed << " seconds\n";
        summaryFile.close();

        std::cout << "\n========================================" << std::endl;
        std::cout << "CUDA Chest Adjacency Summary" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total CUDA god dungeons: " << cudaResults.size() << std::endl;
        std::cout << "✅ Adjacent chests: " << adjacentCount << std::endl;
        std::cout << "⚠️  Non-adjacent chests: " << nonAdjacentCount << std::endl;
        std::cout << "❌ Failed to verify: " << failedCount << std::endl;
        if (adjacentCount + nonAdjacentCount > 0) {
            std::cout << "Adjacent percentage: " << std::fixed << std::setprecision(2)
                      << (100.0 * adjacentCount / (adjacentCount + nonAdjacentCount)) << "%" << std::endl;
        }
        std::cout << "Time elapsed: " << elapsed << " seconds" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Results written to:" << std::endl;
        std::cout << "  - cuda_adjacent_chests.txt" << std::endl;
        std::cout << "  - cuda_non_adjacent_chests.txt" << std::endl;
        std::cout << "  - cuda_chest_adjacency_summary.txt" << std::endl;

        return 0;
    }

    // Original mode - check verified_world_seeds.txt

    std::cout << "Reading verified_world_seeds.txt..." << std::endl;
    std::vector<VerifiedSeed> seeds = parseVerifiedSeeds("verified_world_seeds.txt");

    if (seeds.empty()) {
        std::cerr << "No verified seeds found!" << std::endl;
        std::cerr << "Run validate_world_seeds first." << std::endl;
        return 1;
    }

    std::cout << "Found " << seeds.size() << " verified seed(s)." << std::endl << std::endl;

    // Open output files
    std::ofstream adjacentFile("adjacent_chests.txt");
    std::ofstream nonAdjacentFile("non_adjacent_chests.txt");
    std::ofstream summaryFile("chest_adjacency_summary.txt");

    if (!adjacentFile.is_open() || !nonAdjacentFile.is_open() || !summaryFile.is_open()) {
        std::cerr << "Failed to open output files!" << std::endl;
        return 1;
    }

    adjacentFile << "God Dungeons with Adjacent Chests\n";
    adjacentFile << "==================================\n";
    adjacentFile << "Format: WorldSeed ChunkX ChunkZ DungeonIndex DungeonX DungeonY DungeonZ Chest1(x,y,z) Chest2(x,y,z) Distance Biome\n\n";

    nonAdjacentFile << "God Dungeons with Non-Adjacent Chests\n";
    nonAdjacentFile << "======================================\n";
    nonAdjacentFile << "Format: WorldSeed ChunkX ChunkZ DungeonIndex DungeonX DungeonY DungeonZ Chest1(x,y,z) Chest2(x,y,z) Distance Biome\n\n";

    summaryFile << "Chest Adjacency Summary\n";
    summaryFile << "=======================\n\n";

    int adjacentCount = 0;
    int nonAdjacentCount = 0;
    int failedCount = 0;

    auto startTime = std::chrono::steady_clock::now();

    for (size_t i = 0; i < seeds.size(); i++) {
        auto& seed = seeds[i];

        std::cout << "Checking seed " << (i + 1) << "/" << seeds.size()
                  << " (World: " << seed.worldSeed << ")..." << std::flush;

        bool result = checkChestAdjacency(seed);

        if (result) {
            if (seed.chestsAdjacent) {
                adjacentCount++;
                std::cout << " ✅ ADJACENT (distance=" << seed.distanceBetweenChests << ")" << std::endl;

                adjacentFile << seed.worldSeed << " "
                           << seed.chunkX << " " << seed.chunkZ << " "
                           << seed.dungeonIndex << " "
                           << seed.dungeonX << " " << seed.dungeonY << " " << seed.dungeonZ << " "
                           << seed.chest1Pos << " " << seed.chest2Pos << " "
                           << seed.distanceBetweenChests << " "
                           << (seed.isDesert ? "Desert" : "Non-Desert") << "\n";
                adjacentFile.flush();

                summaryFile << "✅ World Seed: " << seed.worldSeed << "\n";
                summaryFile << "   Dungeon: (" << seed.dungeonX << ", " << seed.dungeonY << ", " << seed.dungeonZ << ")\n";
                summaryFile << "   Chest 1: " << seed.chest1Pos << " " << seed.chest1Pos << "\n";
                summaryFile << "   Chest 2: " << seed.chest2Pos << " " << seed.chest2Pos << "\n";
                summaryFile << "   Distance: " << seed.distanceBetweenChests << " (ADJACENT)\n\n";

            } else {
                nonAdjacentCount++;
                std::cout << " ⚠️  NON-ADJACENT (distance=" << seed.distanceBetweenChests << ")" << std::endl;

                nonAdjacentFile << seed.worldSeed << " "
                              << seed.chunkX << " " << seed.chunkZ << " "
                              << seed.dungeonIndex << " "
                              << seed.dungeonX << " " << seed.dungeonY << " " << seed.dungeonZ << " "
                              << seed.chest1Pos << " " << seed.chest2Pos << " "
                              << seed.distanceBetweenChests << " "
                              << (seed.isDesert ? "Desert" : "Non-Desert") << "\n";
                nonAdjacentFile.flush();

                summaryFile << "⚠️  World Seed: " << seed.worldSeed << "\n";
                summaryFile << "   Dungeon: (" << seed.dungeonX << ", " << seed.dungeonY << ", " << seed.dungeonZ << ")\n";
                summaryFile << "   Chest 1: " << seed.chest1Pos << " " << seed.chest1Pos << "\n";
                summaryFile << "   Chest 2: " << seed.chest2Pos << " " << seed.chest2Pos << "\n";
                summaryFile << "   Distance: " << seed.distanceBetweenChests << " (NON-ADJACENT)\n\n";
            }
        } else {
            failedCount++;
            std::cout << " ❌ FAILED TO VERIFY" << std::endl;
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();

    adjacentFile.close();
    nonAdjacentFile.close();

    summaryFile << "=======================\n";
    summaryFile << "Total verified seeds: " << seeds.size() << "\n";
    summaryFile << "✅ Adjacent chests: " << adjacentCount << "\n";
    summaryFile << "⚠️  Non-adjacent chests: " << nonAdjacentCount << "\n";
    summaryFile << "❌ Failed to verify: " << failedCount << "\n";
    summaryFile << "Adjacent percentage: " << std::fixed << std::setprecision(2)
               << (100.0 * adjacentCount / (adjacentCount + nonAdjacentCount)) << "%\n";
    summaryFile << "Time elapsed: " << elapsed << " seconds\n";
    summaryFile.close();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Chest Adjacency Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total verified seeds: " << seeds.size() << std::endl;
    std::cout << "✅ Adjacent chests: " << adjacentCount << std::endl;
    std::cout << "⚠️  Non-adjacent chests: " << nonAdjacentCount << std::endl;
    std::cout << "❌ Failed to verify: " << failedCount << std::endl;
    if (adjacentCount + nonAdjacentCount > 0) {
        std::cout << "Adjacent percentage: " << std::fixed << std::setprecision(2)
                  << (100.0 * adjacentCount / (adjacentCount + nonAdjacentCount)) << "%" << std::endl;
    }
    std::cout << "Time elapsed: " << elapsed << " seconds" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Results written to:" << std::endl;
    std::cout << "  - adjacent_chests.txt" << std::endl;
    std::cout << "  - non_adjacent_chests.txt" << std::endl;
    std::cout << "  - chest_adjacency_summary.txt" << std::endl;

    return 0;
}

