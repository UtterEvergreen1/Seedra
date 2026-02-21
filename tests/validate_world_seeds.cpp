// Validate world seeds from god dungeon reversal
// Takes the output from world_seeds_from_god_dungeons.txt and validates each one
// by generating chunks and checking if the god dungeon actually appears

#include "terrain/World.hpp"
#include "terrain/Generator.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/decorators/WorldGenDungeons.hpp"
#include "loot/tables/simple_dungeon.hpp"
#include "lce/items/itemsInit.hpp"
#include "common/rng.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>

#include "terrain/carve/ChunkGenerator.hpp"

struct WorldSeedCandidate {
    int64_t worldSeed;
    int chunkX;
    int chunkZ;
    int dungeonIndex;
    int expectedDungeonX;
    int expectedDungeonY;
    int expectedDungeonZ;
    bool isDesert;
    int godDungeonNum;

    // Validation results
    bool validated;
    int actualDungeonX;
    int actualDungeonY;
    int actualDungeonZ;
    int actualDungeonAttempt;
    bool dungeonGenerated;
};

struct DungeonSize {
    int halfSizeX;
    int halfSizeZ;
};

const DungeonSize DUNGEON_SIZES[4] = {
    {2, 2}, // 5x5
    {2, 3}, // 5x7
    {3, 2}, // 7x5
    {3, 3}  // 7x7
};

// Counts enchanted golden apples in a container
int countEnchantedGoldenApples(const loot::Container<27>& container) {
    using namespace lce::items;
    int count = 0;

    for (size_t i = 0; i < container.slotCount(); i++) {
        const lce::ItemState& itemState = container.getSlotAt(i);
        if (itemState.getID() == ENCHANTED_GOLDEN_APPLE.getState().getID() &&
            itemState.getDataTag() == ENCHANTED_GOLDEN_APPLE.getState().getDataTag()) {
            count += itemState.getCount();
        }
    }

    return count;
}

// Custom dungeon generator that tracks all dungeon attempts
class TrackingDungeonGen : public WorldGenDungeons {
public:
    struct DungeonAttempt {
        int attemptNumber;
        int x, y, z;
        int halfSizeX, halfSizeZ;
        bool generated;
        std::vector<int64_t> chestSeeds;
    };

    std::vector<DungeonAttempt> allAttempts;
    int currentAttempt = -1;

    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override {
        using namespace lce::blocks;

        DungeonAttempt attempt{};
        const_cast<TrackingDungeonGen*>(this)->currentAttempt++;
        attempt.attemptNumber = currentAttempt;
        attempt.x = position.getX();
        attempt.y = position.getY();
        attempt.z = position.getZ();

        // Random dungeon dimensions
        const int halfSizeX = rng.nextInt(2) + 2;
        const int minX = -halfSizeX - 1;
        const int maxX = halfSizeX + 1;
        constexpr int minY = -1;
        constexpr int maxY = 4;
        const int halfSizeZ = rng.nextInt(2) + 2;
        const int minZ = -halfSizeZ - 1;
        const int maxZ = halfSizeZ + 1;

        attempt.halfSizeX = halfSizeX;
        attempt.halfSizeZ = halfSizeZ;

        int airCount = 0;

        // First pass: validate location
        for (int offsetX = minX; offsetX <= maxX; ++offsetX) {
            for (int offsetY = minY; offsetY <= maxY; ++offsetY) {
                for (int offsetZ = minZ; offsetZ <= maxZ; ++offsetZ) {
                    Pos3D blockPos = position.add(offsetX, offsetY, offsetZ);
                    const bool isSolid = lce::blocks::isSolidBlock(worldIn->getBlockId(blockPos));

                    if (offsetY == minY && !isSolid) {
                        attempt.generated = false;
                        const_cast<TrackingDungeonGen*>(this)->allAttempts.push_back(attempt);
                        return false;
                    }
                    if (offsetY == maxY && !isSolid) {
                        attempt.generated = false;
                        const_cast<TrackingDungeonGen*>(this)->allAttempts.push_back(attempt);
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
            attempt.generated = false;
            const_cast<TrackingDungeonGen*>(this)->allAttempts.push_back(attempt);
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
                        int64_t lootSeed = rng.nextLongI();
                        attempt.chestSeeds.push_back(lootSeed);
                        break;
                    }
                }
            }
        }

        // Place mob spawner
        worldIn->setBlockId(position, MONSTER_SPAWNER_ID);
        // Note: pickMobSpawner is private, but we don't need it for validation

        attempt.generated = true;
        const_cast<TrackingDungeonGen*>(this)->allAttempts.push_back(attempt);
        return true;
    }
};
// #define DEBUG
// Parse world_seeds_from_god_dungeons.txt
std::vector<WorldSeedCandidate> parseWorldSeeds(const std::string& filename) {
    std::vector<WorldSeedCandidate> candidates;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return candidates;
    }

    std::string line;
    int lineNum = 0;

    // Skip header lines
    while (std::getline(inFile, line)) {
        lineNum++;
        if (line.empty() || line[0] == '=' || line.find("Format:") != std::string::npos ||
            line.find("WorldSeed:") != std::string::npos) {
            continue;
        }

        //check if debug
#ifdef DEBUG
        if (lineNum >= 100000) {
            break;
        }
#endif

        // Parse line: WorldSeed ChunkX ChunkZ DungeonIndex DungeonX DungeonY DungeonZ Biome GodDungeonNum
        std::istringstream iss(line);
        WorldSeedCandidate candidate{};
        std::string biomeStr;

        if (iss >> candidate.worldSeed >> candidate.chunkX >> candidate.chunkZ
            >> candidate.dungeonIndex >> candidate.expectedDungeonX
            >> candidate.expectedDungeonY >> candidate.expectedDungeonZ >> biomeStr) {

            // Handle optional GodDungeonNum
            if (!(iss >> candidate.godDungeonNum)) {
                candidate.godDungeonNum = 0;
            }

            candidate.expectedDungeonX += candidate.chunkX * 16;
            candidate.expectedDungeonZ += candidate.chunkZ * 16;
            candidate.isDesert = (biomeStr == "Desert");
            candidate.validated = false;
            candidate.dungeonGenerated = false;
            candidate.actualDungeonX = 0;
            candidate.actualDungeonY = 0;
            candidate.actualDungeonZ = 0;
            candidate.actualDungeonAttempt = -1;

            candidates.push_back(candidate);
        }
    }

    inFile.close();
    return candidates;
}

// Validate a single world seed candidate
bool validateWorldSeed(WorldSeedCandidate& candidate) {
    // Create generator with the world seed
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, candidate.worldSeed,
                       lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    // generator.generateCachesUpTo(4);
    World world(&generator);

    // Generate chunks in a 3x3 grid centered on the target chunk
    // This ensures surrounding chunks are generated for proper decoration
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            int cx = candidate.chunkX + dx;
            int cz = candidate.chunkZ + dz;
            Pos2D chunkPos(cx, cz);
            auto* caveChunk = new ChunkPrimer();
            std::fill_n(caveChunk->blocks.data(), caveChunk->blocks.size(), lce::blocks::BlockID::STONE_ID << 4);
            world.addChunk(chunkPos, caveChunk);
            caveChunk->stage = Stage::STAGE_CAVES;
            Chunk::populateCaves(world, chunkPos);
        }
    }

    // Get populate seed for this chunk
    RNG rng = RNG::getPopulationSeed(candidate.worldSeed, candidate.chunkX, candidate.chunkZ);

    // Simulate water lake check
    if (candidate.isDesert) {
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

    // Try dungeon generation with tracking
    TrackingDungeonGen trackingGen;

    for (int i = 0; i < 8; i++) {
        int xPos = rng.nextInt(16) + 8 + candidate.chunkX * 16;
        int yPos = rng.nextInt(128);
        int zPos = rng.nextInt(16) + 8 + candidate.chunkZ * 16;

        Pos3D dungeonPos(xPos, yPos, zPos);
        trackingGen.generate(&world, rng, dungeonPos);
    }

    // Check if any dungeon attempt matches our expectations
    for (const auto& attempt : trackingGen.allAttempts) {
        if (!attempt.generated) continue;
        if (attempt.attemptNumber == candidate.dungeonIndex) {
            // Check position matches
            if (attempt.x == candidate.expectedDungeonX &&
                attempt.z == candidate.expectedDungeonZ) {

                // Check if it's a god dungeon
                if (attempt.chestSeeds.size() == 2) {
                    constexpr auto Mode = loot::GenMode::MODERN;
                    loot::Container<27> container;

                    bool isGodDungeon = true;
                    for (int64_t chestSeed : attempt.chestSeeds) {
                        container.clear();
                        loot::simple_dungeon.getLootFromLootTableSeed<Mode>(container, chestSeed);

                        int appleCount = countEnchantedGoldenApples(container);
                        if (appleCount != 3) {
                            isGodDungeon = false;
                            break;
                        }
                    }

                    if (isGodDungeon) {
                        candidate.validated = true;
                        candidate.dungeonGenerated = true;
                        candidate.actualDungeonX = attempt.x;
                        candidate.actualDungeonY = attempt.y;
                        candidate.actualDungeonZ = attempt.z;
                        candidate.actualDungeonAttempt = attempt.attemptNumber;
                        return true;
                    }
                }
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    return false;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "World Seed Validator for God Dungeons" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    // Setup loot system
    enchants::EnchantController::setup(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC);

    std::cout << "Reading world_seeds_from_god_dungeons.txt..." << std::endl;
    std::vector<WorldSeedCandidate> candidates = parseWorldSeeds("world_seeds_from_god_dungeons.txt");

    if (candidates.empty()) {
        std::cerr << "No world seed candidates found!" << std::endl;
        std::cerr << "Run reverse_world_seeds first." << std::endl;
        return 1;
    }

    std::cout << "Found " << candidates.size() << " world seed candidate(s)." << std::endl << std::endl;

    // Open output files
    std::ofstream validFile("verified_world_seeds.txt");
    std::ofstream summaryFile("validation_summary.txt");
    std::ofstream checkpointFile("validation_checkpoint.txt");

    if (!validFile.is_open() || !summaryFile.is_open()) {
        std::cerr << "Failed to open output files!" << std::endl;
        return 1;
    }

    validFile << "Verified World Seeds for God Dungeons\n";
    validFile << "======================================\n";
    validFile << "Format: WorldSeed ChunkX ChunkZ DungeonIndex DungeonX DungeonY DungeonZ Biome\n\n";

    summaryFile << "Validation Summary\n";
    summaryFile << "==================\n\n";

    int validCount = 0;
    int invalidCount = 0;
    //start time

    // Speed/checkpointing
    constexpr size_t CHECKPOINT_INTERVAL = 100000; // checkpoint every 100k seeds
    size_t processedSeeds = 0;
    auto startTime = std::chrono::steady_clock::now();

    for (size_t i = 0; i < candidates.size(); i++) {
        auto& candidate = candidates[i];

        /*std::cout << "Validating seed " << (i + 1) << "/" << candidates.size()
                  << " (World: " << candidate.worldSeed << ", Chunk: "
                  << candidate.chunkX << "," << candidate.chunkZ << ")..." << std::flush;*/

        bool result = validateWorldSeed(candidate);

        if (result && candidate.validated) {
            validCount++;
            // std::cout << " ✅ VALID" << std::endl;

            validFile << candidate.worldSeed << " "
                     << candidate.chunkX << " " << candidate.chunkZ << " "
                     << candidate.dungeonIndex << " "
                     << candidate.actualDungeonX << " "
                     << candidate.actualDungeonY << " "
                     << candidate.actualDungeonZ << " "
                     << (candidate.isDesert ? "Desert" : "Non-Desert") << "\n";

            summaryFile << "✅ World Seed: " << candidate.worldSeed << "\n";
            summaryFile << "   Chunk: (" << candidate.chunkX << ", " << candidate.chunkZ << ")\n";
            summaryFile << "   Dungeon: (" << candidate.actualDungeonX << ", "
                       << candidate.actualDungeonY << ", " << candidate.actualDungeonZ << ")\n";
            summaryFile << "   Attempt: " << candidate.actualDungeonAttempt << "\n";

        } else {
            invalidCount++;
            // std::cout << " ❌ INVALID" << "\r" << std::flush;
        }

        // Progress bookkeeping
        processedSeeds++;

        // Checkpoint every CHECKPOINT_INTERVAL seeds
        if EXPECT_FALSE(processedSeeds % CHECKPOINT_INTERVAL == 0) {
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime).count();
            double rate = (elapsed > 0.0) ? (static_cast<double>(processedSeeds) / elapsed) : 0.0;

            // Console output
            std::cout << "[Checkpoint] Processed " << processedSeeds
                      << " seeds. Valid=" << validCount << " Invalid=" << invalidCount
                      << " Elapsed=" << elapsed << "s Rate=" << rate << " seeds/s\n";

            // Write to summary and flush files
            summaryFile << "[Checkpoint] Processed " << processedSeeds
                        << " seeds. Valid=" << validCount << " Invalid=" << invalidCount
                        << " Elapsed=" << elapsed << "s Rate=" << rate << " seeds/s\n";
            summaryFile.flush();
            validFile.flush();

            // Create a small checkpoint file for external monitoring
            checkpointFile.open("validation_checkpoint.txt");
            if (checkpointFile.is_open()) {
                checkpointFile << "Processed: " << processedSeeds << "\n";
                checkpointFile << "Valid: " << validCount << "\n";
                checkpointFile << "Invalid: " << invalidCount << "\n";
                checkpointFile << "Elapsed: " << elapsed << "\n";
                checkpointFile << "Rate: " << rate << "\n";
                checkpointFile << "Timestamp: " << std::time(nullptr) << "\n";
                checkpointFile.flush();
                checkpointFile.close();
            }
        }
    }

    validFile.close();

    summaryFile << "==================\n";
    summaryFile << "Total candidates: " << candidates.size() << "\n";
    summaryFile << "✅ Valid: " << validCount << "\n";
    summaryFile << "❌ Invalid: " << invalidCount << "\n";
    summaryFile << "Success rate: " << std::fixed << std::setprecision(2)
               << (100.0 * validCount / candidates.size()) << "%\n";
    summaryFile.close();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Validation Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total candidates: " << candidates.size() << std::endl;
    std::cout << "✅ Valid: " << validCount << std::endl;
    std::cout << "❌ Invalid: " << invalidCount << std::endl;
    std::cout << "Success rate: " << std::fixed << std::setprecision(2)
              << (100.0 * validCount / candidates.size()) << "%" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Results written to:" << std::endl;
    std::cout << "  - verified_world_seeds.txt" << std::endl;
    std::cout << "  - invalid_world_seeds.txt" << std::endl;
    std::cout << "  - validation_summary.txt" << std::endl;

    return (invalidCount > 0) ? 1 : 0;
}

