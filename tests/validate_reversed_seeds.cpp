// Validate reversed populate seeds by actually generating chunks and dungeons
// Creates a minimal world with chunks around (0,0) and tests if the reversed
// populate seeds actually produce the expected god dungeons

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

#include "terrain/biomes/biome.hpp"

struct ValidatedSeed {
    uint64_t populateSeed;
    int64_t expectedRngSeed;
    int expectedSizeIdx;
    int godDungeonNum;
    bool validated;
    int64_t actualRngSeed;
    int actualSizeIdx;
    std::vector<int64_t> chestSeeds;
    bool isDesert;
    int dungeonIndex;
    int actualDungeonX; // The actual position used (for the dungeonIndex)
    int actualDungeonY;
    int actualDungeonZ;
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

// Parse reversed_populate_seeds files to get all populate seeds to test
std::vector<ValidatedSeed> parseReversedSeeds(const std::string& filename, bool isDesert) {
    std::vector<ValidatedSeed> seeds;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return seeds;
    }

    std::string line;
    int currentGodDungeonNum = 0;
    int64_t currentExpectedRngSeed = 0;
    int currentExpectedSizeIdx = 0;

    std::regex godDungeonRegex(R"(God Dungeon #(\d+))");
    std::regex rngSeedRegex(R"(RNG Seed:\s*(\d+))");
    std::regex sizeRegex(R"(Size:\s*(\d+)x(\d+))");
    std::regex populateSeedRegex(R"(^\s+(\d+)\s+\(0x[0-9a-f]+\)\s+\[Dungeon Index:\s*(\d+)\])");

    while (std::getline(inFile, line)) {
        std::smatch match;

        if (std::regex_search(line, match, godDungeonRegex)) {
            currentGodDungeonNum = std::stoi(match[1].str());
        } else if (std::regex_search(line, match, rngSeedRegex)) {
            currentExpectedRngSeed = std::stoll(match[1].str());
        } else if (std::regex_search(line, match, sizeRegex)) {
            int width = std::stoi(match[1].str());
            int length = std::stoi(match[2].str());

            // Map to size index
            for (int i = 0; i < 4; i++) {
                int expectedWidth = DUNGEON_SIZES[i].halfSizeX * 2 + 1;
                int expectedLength = DUNGEON_SIZES[i].halfSizeZ * 2 + 1;
                if (expectedWidth == width && expectedLength == length) {
                    currentExpectedSizeIdx = i;
                    break;
                }
            }
        } else if (std::regex_search(line, match, populateSeedRegex)) {
            ValidatedSeed seed;
            seed.populateSeed = std::stoull(match[1].str());
            seed.dungeonIndex = std::stoi(match[2].str());
            seed.expectedRngSeed = currentExpectedRngSeed;
            seed.expectedSizeIdx = currentExpectedSizeIdx;
            seed.godDungeonNum = currentGodDungeonNum;
            seed.validated = false;
            seed.actualRngSeed = 0;
            seed.actualSizeIdx = 0;
            seed.isDesert = isDesert;
            seeds.push_back(seed);
        }
    }

    inFile.close();
    return seeds;
}

// Custom dungeon generator that tracks RNG seed before chest placement
class TrackingDungeonGen : public WorldGenDungeons {
public:
    int64_t capturedRngSeed = 0;
    int capturedSizeIdx = 0;
    std::vector<int64_t> chestLootSeeds;
    bool dungeonGenerated = false;

    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override {
        using namespace lce::blocks;

        // Random dungeon dimensions
        const int halfSizeX = rng.nextInt(2) + 2;
        const int minX = -halfSizeX - 1;
        const int maxX = halfSizeX + 1;
        constexpr int minY = -1;
        constexpr int maxY = 4;
        const int halfSizeZ = rng.nextInt(2) + 2;
        const int minZ = -halfSizeZ - 1;
        const int maxZ = halfSizeZ + 1;

        // Determine size index
        for (int i = 0; i < 4; i++) {
            if (DUNGEON_SIZES[i].halfSizeX == halfSizeX &&
                DUNGEON_SIZES[i].halfSizeZ == halfSizeZ) {
                const_cast<TrackingDungeonGen*>(this)->capturedSizeIdx = i;
                break;
            }
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
        // CAPTURE RNG SEED HERE - right before chest placement
        const_cast<TrackingDungeonGen*>(this)->capturedRngSeed = rng.getSeed();
        const_cast<TrackingDungeonGen*>(this)->chestLootSeeds.clear();

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
                        int64_t lootSeed = rng.nextLong();
                        const_cast<TrackingDungeonGen*>(this)->chestLootSeeds.push_back(lootSeed);
                        break;
                    }
                }
            }
        }

        // Place mob spawner
        worldIn->setBlockId(position, MONSTER_SPAWNER_ID);
        // pickMobSpawner(rng);
        rng.nextInt(4);

        const_cast<TrackingDungeonGen*>(this)->dungeonGenerated = true;
        return true;
    }
};

// Validate a single populate seed
bool validatePopulateSeed(ValidatedSeed& seed, int64_t worldSeed) {
    // Create a minimal generator
    Generator generator(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC, worldSeed, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    World world(&generator);

    // Generate chunks in a 3x3 grid around (0,0)
    for (int cx = -1; cx <= 1; cx++) {
        for (int cz = -1; cz <= 1; cz++) {
            Pos2D chunkPos(cx, cz);
            auto* caveChunk = new ChunkPrimer();
            std::fill_n(caveChunk->blocks.data(), caveChunk->blocks.size(), lce::blocks::BlockID::STONE_ID << 4);
            world.addChunk(chunkPos, caveChunk);
            caveChunk->stage = Stage::STAGE_WATER_CAVES;
        }
    }

    // Now manually run the decoration sequence for chunk (0,0)
    // We need to simulate the exact RNG sequence
    RNG rng;
    rng.setSeed(seed.populateSeed);

    // Water lake check
    // Code: if (biomeAt == desert || biomeAt == desert_hills || rng.nextInt(4) != 0) return {};
    if (seed.isDesert) {
        // Desert: Short-circuit evaluation means nextInt(4) is NEVER called!
        // No RNG consumption at all for desert biomes
    } else {
        // Non-desert biome: nextInt(4) IS called
        int waterCheck = rng.nextInt(4);
        if (waterCheck == 0) {
            // Position calculated - generation may have been attempted but failed
            rng.nextInt(16); // x
            rng.nextInt(128); // y
            rng.nextInt(16); // z
            // Water lake generation would happen here, we assume it failed
        }
    }

    // Lava lake check
    if (rng.nextInt(8) == 0) {
        rng.nextInt(16);
        int y = rng.nextInt(rng.nextInt(120) + 8);
        rng.nextInt(16);
        if (y >= 63 && rng.nextInt(10) == 0) {
            // Lava lake would generate - skip dungeons
            world.deleteWorld(); // Clean up before returning
            return false;
        }
    }

    // Try up to 8 dungeon attempts, but only generate at the expected index
    // We need to simulate the RNG progression through all attempts
    TrackingDungeonGen trackingGen;

    for (int i = 0; i < 8; i++) {
        int xPos = rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = rng.nextInt(16) + 8;

        if (i == seed.dungeonIndex) {
            // Store the position for this dungeon index
            seed.actualDungeonX = xPos;
            seed.actualDungeonY = yPos;
            seed.actualDungeonZ = zPos;

            Pos3D dungeonPos(xPos, 16, zPos); // Keep Y=16 for solid checks as you requested

            if (trackingGen.generate(&world, rng, dungeonPos)) {
                // Dungeon was generated!
                seed.actualRngSeed = trackingGen.capturedRngSeed;
                seed.actualSizeIdx = trackingGen.capturedSizeIdx;
                seed.chestSeeds = trackingGen.chestLootSeeds;

                // Check if this matches our expectations
                if (seed.actualRngSeed == seed.expectedRngSeed &&
                    seed.actualSizeIdx == seed.expectedSizeIdx) {

                    // Verify it's actually a god dungeon
                    if (seed.chestSeeds.size() == 2) {
                        constexpr auto Mode = loot::GenMode::MODERN;
                        loot::Container<27> container;

                        bool isGodDungeon = true;
                        for (size_t j = 0; j < seed.chestSeeds.size(); j++) {
                            container.clear();
                            loot::simple_dungeon.getLootFromLootTableSeed<Mode>(container, seed.chestSeeds[j]);

                            int appleCount = countEnchantedGoldenApples(container);
                            if (appleCount != 3) {
                                isGodDungeon = false;
                                break;
                            }
                        }

                        seed.validated = isGodDungeon;
                        world.deleteWorld(); // Clean up before returning
                        return isGodDungeon;
                    }
                }

                // Dungeon generated but doesn't match expectations
                world.deleteWorld(); // Clean up before returning
                return false;
            }

            // Dungeon failed to generate at expected index
            world.deleteWorld(); // Clean up before returning
            return false;
        }
        else {
            // For non-matching indices, simulate failed dungeon attempt
            // Each failed attempt consumes 2 size rolls
            rng.nextInt(2); // halfSizeX
            rng.nextInt(2); // halfSizeZ
        }
    }

    world.deleteWorld(); // Clean up before returning
    return false;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Reversed Seed Validator" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    // Setup loot system
    enchants::EnchantController::setup(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC);
    Biome::registerBiomes();

    std::cout << "Reading reversed populate seeds..." << std::endl;

    // Read desert seeds
    std::vector<ValidatedSeed> desertSeeds = parseReversedSeeds("reversed_populate_seeds_desert.txt", true);
    std::cout << "Found " << desertSeeds.size() << " desert populate seed(s)." << std::endl;

    // Read non-desert seeds
    std::vector<ValidatedSeed> nonDesertSeeds = parseReversedSeeds("reversed_populate_seeds_non_desert.txt", false);
    std::cout << "Found " << nonDesertSeeds.size() << " non-desert populate seed(s)." << std::endl;

    // Combine both lists
    std::vector<ValidatedSeed> allSeeds;
    allSeeds.insert(allSeeds.end(), desertSeeds.begin(), desertSeeds.end());
    allSeeds.insert(allSeeds.end(), nonDesertSeeds.begin(), nonDesertSeeds.end());

    if (allSeeds.empty()) {
        std::cerr << "No reversed populate seeds found!" << std::endl;
        std::cerr << "Run reverse_god_dungeon first." << std::endl;
        return 1;
    }

    std::cout << "Total seeds to validate: " << allSeeds.size() << std::endl << std::endl;

    // Open output files
    std::ofstream outFile("validation_results.txt");
    std::ofstream validFile("truly_valid_seeds.txt");

    if (!outFile.is_open() || !validFile.is_open()) {
        std::cerr << "Failed to open output files!" << std::endl;
        return 1;
    }

    outFile << "Validation Results for Reversed Populate Seeds\n";
    outFile << "==============================================\n\n";

    validFile << "Truly Valid Populate Seeds (Verified by Generation)\n";
    validFile << "====================================================\n\n";

    int validCount = 0;
    int invalidCount = 0;
    int validDesertCount = 0;
    int validNonDesertCount = 0;
    int64_t dummyWorldSeed = 12345; // We use a dummy world seed since we're testing populate seeds

    for (size_t i = 0; i < allSeeds.size(); i++) {
        auto& seed = allSeeds[i];

        std::cout << "Validating seed " << (i + 1) << "/" << allSeeds.size()
                  << " (" << (seed.isDesert ? "Desert" : "Non-Desert")
                  << ", God Dungeon #" << seed.godDungeonNum << ")..." << std::flush;

        bool result = validatePopulateSeed(seed, dummyWorldSeed);

        if (result && seed.validated) {
            validCount++;
            if (seed.isDesert) {
                validDesertCount++;
            } else {
                validNonDesertCount++;
            }
            std::cout << " ✅ VALID" << std::endl;

            outFile << "Seed #" << (i + 1) << " - God Dungeon #" << seed.godDungeonNum
                    << " (" << (seed.isDesert ? "Desert" : "Non-Desert") << ") - ✅ VALID\n";
            outFile << "  Populate Seed: " << seed.populateSeed << " (0x" << std::hex
                    << seed.populateSeed << std::dec << ")\n";
            outFile << "  Dungeon Index: " << seed.dungeonIndex << "\n";
            outFile << "  Dungeon Position: (" << seed.actualDungeonX << ", "
                    << seed.actualDungeonY << ", " << seed.actualDungeonZ << ")\n";
            outFile << "  Expected RNG Seed: " << seed.expectedRngSeed << "\n";
            outFile << "  Actual RNG Seed:   " << seed.actualRngSeed << "\n";
            outFile << "  Expected Size: " << (DUNGEON_SIZES[seed.expectedSizeIdx].halfSizeX * 2 + 1) << "x"
                    << (DUNGEON_SIZES[seed.expectedSizeIdx].halfSizeZ * 2 + 1) << "\n";
            outFile << "  Actual Size:   " << (DUNGEON_SIZES[seed.actualSizeIdx].halfSizeX * 2 + 1) << "x"
                    << (DUNGEON_SIZES[seed.actualSizeIdx].halfSizeZ * 2 + 1) << "\n";
            outFile << "  Chest Seeds: " << seed.chestSeeds[0] << ", " << seed.chestSeeds[1] << "\n";
            outFile << "  Result: Both chests have 3 enchanted golden apples!\n\n";

            validFile << "God Dungeon #" << seed.godDungeonNum
                      << " (" << (seed.isDesert ? "Desert" : "Non-Desert") << ")\n";
            validFile << "  Populate Seed: " << seed.populateSeed << " (0x" << std::hex
                      << seed.populateSeed << std::dec << ")\n";
            validFile << "  Dungeon Index: " << seed.dungeonIndex << "\n";
            validFile << "  Dungeon Position: (" << seed.actualDungeonX << ", "
                      << seed.actualDungeonY << ", " << seed.actualDungeonZ << ")\n";
            validFile << "  RNG Seed: " << seed.actualRngSeed << "\n";
            validFile << "  Size: " << (DUNGEON_SIZES[seed.actualSizeIdx].halfSizeX * 2 + 1) << "x"
                      << (DUNGEON_SIZES[seed.actualSizeIdx].halfSizeZ * 2 + 1) << "\n\n";

        } else {
            invalidCount++;
            std::cout << " ❌ INVALID" << std::endl;

            outFile << "Seed #" << (i + 1) << " - God Dungeon #" << seed.godDungeonNum
                    << " (" << (seed.isDesert ? "Desert" : "Non-Desert") << ") - ❌ INVALID\n";
            outFile << "  Populate Seed: " << seed.populateSeed << " (0x" << std::hex
                    << seed.populateSeed << std::dec << ")\n";
            outFile << "  Dungeon Index: " << seed.dungeonIndex << "\n";
            outFile << "  Expected Dungeon Position: (" << seed.actualDungeonX << ", "
                    << seed.actualDungeonY << ", " << seed.actualDungeonZ << ")\n";
            outFile << "  Expected RNG Seed: " << seed.expectedRngSeed << "\n";
            outFile << "  Actual RNG Seed:   " << seed.actualRngSeed << "\n";
            outFile << "  Reason: ";
            if (seed.actualRngSeed != seed.expectedRngSeed) {
                outFile << "RNG seed mismatch\n";
            } else if (seed.actualSizeIdx != seed.expectedSizeIdx) {
                outFile << "Size mismatch\n";
            } else if (seed.chestSeeds.size() != 2) {
                outFile << "Wrong number of chests (" << seed.chestSeeds.size() << ")\n";
            } else {
                outFile << "Not a god dungeon (wrong loot)\n";
            }
            outFile << "\n";
        }
    }

    outFile << "==============================================\n";
    outFile << "Summary:\n";
    outFile << "  Total seeds tested: " << allSeeds.size() << "\n";
    outFile << "  ✅ Valid: " << validCount << "\n";
    outFile << "    - Desert: " << validDesertCount << "\n";
    outFile << "    - Non-Desert: " << validNonDesertCount << "\n";
    outFile << "  ❌ Invalid: " << invalidCount << "\n";
    outFile << "  Success rate: " << std::fixed << std::setprecision(2)
            << (100.0 * validCount / allSeeds.size()) << "%\n";

    validFile << "====================================================\n";
    validFile << "Total truly valid seeds: " << validCount << "\n";
    validFile << "  Desert seeds: " << validDesertCount << "\n";
    validFile << "  Non-desert seeds: " << validNonDesertCount << "\n";

    outFile.close();
    validFile.close();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Validation Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total seeds tested: " << allSeeds.size() << std::endl;
    std::cout << "✅ Valid: " << validCount << std::endl;
    std::cout << "  - Desert: " << validDesertCount << std::endl;
    std::cout << "  - Non-Desert: " << validNonDesertCount << std::endl;
    std::cout << "❌ Invalid: " << invalidCount << std::endl;
    std::cout << "Success rate: " << std::fixed << std::setprecision(2)
              << (100.0 * validCount / allSeeds.size()) << "%" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Results written to:" << std::endl;
    std::cout << "  - validation_results.txt" << std::endl;
    std::cout << "  - truly_valid_seeds.txt" << std::endl;

    return (invalidCount > 0) ? 1 : 0;
}

