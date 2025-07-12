#include <filesystem>
#include <thread>
#include <fstream>

#include "common/timer.hpp"
#include "loot/classes/Container.hpp"
#include "loot/tables/blacksmith.hpp"
#include "structures/gen/village/village.hpp"
#include "structures/placement/StaticStructures.hpp"
#include "terrain/generator.hpp"

int64_t previousLong(int64_t seed) {
    int64_t MULT = 0x5DEECE66D;
    int ADD = 0xB;
    int64_t p1 = 0;
    for (int i = 1; i <= 48; i++) {
        int64_t mask = ((int64_t) 1 << i) - 1;
        int64_t p2 = (p1 * MULT + ADD) & mask;
        if (p2 != (seed & mask)) {
            p1 = p1 | ((int64_t) 1 << (i - 1));
        }
    }
    seed = p1;
    return seed;
}

int64_t isNextLong(uint64_t l) {
    int64_t MULT = 0x5DEECE66D;
    int ADD = 0xB;
    int64_t m48 = ((int64_t) 1 << 48) - 1;
    int64_t MASK32 = ((int64_t) 1 << 32) - 1;
    int64_t MASK31 = ((int64_t) 1 << 31) - 1;
    int64_t TWO16_RAW = ((int64_t) 1 << 16);

    int64_t i1 = (l >> 32) & MASK32;
    int64_t i2 = l & MASK32;
    if (i2 > MASK31) {
        i1++;
    }
    int64_t front = i1 << 16;
    for (int64_t i = 0; i < TWO16_RAW; i++) {
        int64_t sed = front | i;
        int64_t i22 = ((sed * MULT + ADD) & m48) >> 16;
        if (i22 == i2) {
            int64_t ret;
            ret = previousLong(sed);
            return ret;
        }
    }
    return -1;
}

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
            if (s1.pos.size() < s2.pos.size()) return true;
            if (s2.pos.size() < s1.pos.size()) return false;
            for (size_t i = 0; i < s1.pos.size(); ++i) {
                if (s1.pos[i] < s2.pos[i]) return true;
                if (s2.pos[i] < s1.pos[i]) return false;
            }
            if (s1.seed < s2.seed) return true;
            if (s2.seed < s1.seed) return false;
            return false; // all positions are equal
        }
    };
};

void bruteForceObi() {
    i64 maxSeed = (1LL << 48); // Maximum seed value for 48-bit seeds
    const uint32_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::mutex coutMutex;
    std::ofstream outputFile("48+LootObiSeeds.txt", std::ios::app);

    auto worker = [&](i64 startSeed, i64 endSeed, int threadNum) {
        loot::Container27 container;
        Timer timer;
        int numObi;
        for (i64 currentSeed = startSeed; currentSeed < endSeed; ++currentSeed) {
            container.clear();
            numObi = 0;
            if EXPECT_FALSE (currentSeed % 1000000000 == 0) {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "Thread " << threadNum << " processing seed: " << currentSeed << std::endl;
                std::cout << "Seeds per second: "
                          << 1000000000.0f / timer.getSeconds() << std::endl;
                timer.reset();
            }

            loot::blacksmith.getLootFromLootTableSeed<loot::GenMode::MOD_NO_SHUF>(container, currentSeed);
            for (int itemAt = 0; itemAt < container.slotCount(); itemAt++) {
                lce::ItemState item = container.getSlotAt(itemAt);
                if (item.id == lce::ids::ItemID::OBSIDIAN_ID) {
                    numObi += item.myItemCount;
                }
            }

            if (numObi < 48) continue;

            numObi = 0;
            container.clear();
            loot::blacksmith.getLootFromLootTableSeed<loot::GenMode::MODERN>(container, currentSeed);
            for (int itemAt = 0; itemAt < container.slotCount(); itemAt++) {
                lce::ItemState item = container.getSlotAt(itemAt);
                if (item.id == lce::ids::ItemID::OBSIDIAN_ID) {
                    numObi += item.myItemCount;
                }
            }

            if (numObi < 48) continue;

            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Found seed with 40+ obsidian: " << currentSeed << std::endl;
            std::cout << "Container contents:" << std::endl;
            std::cout << container << std::endl;
            outputFile << currentSeed << std::endl;
            outputFile.flush();
        }
    };

    std::vector<i64> startSeeds = {
        1617000000000, 48502000000000, 95379000000000, 142292000000000, 189231000000000, 236179000000000
    };

    i64 seedsPerThread = maxSeed / numThreads;
    for (int t = 0; t < numThreads; ++t) {
        // i64 startSeed = t * seedsPerThread;
        i64 startSeed = startSeeds[t % startSeeds.size()];
        i64 endSeed = (t == numThreads - 1) ? maxSeed : (t + 1) * seedsPerThread;
        threads.emplace_back(worker, startSeed, endSeed, t);
    }
    for (auto &th : threads) {
        th.join();
    }
    outputFile.close();
}

void findMostObi() {
    // std::string inputFile = "cakeResults.txt";
    std::string inputFile = "48+LootObiSeeds.txt";
    std::ifstream file(inputFile);

    if (!file.is_open()) {
        std::cerr << "Could not open input file: " << inputFile << std::endl;
        return;
    }

    constexpr i64 maxSisterSeed = 0x7FFF000000000000;

    int highestObiCount = 0;
    i64 seed;
    while (file >> seed) {
        loot::Container27 container;

        // test if any of the seeds can generate 4+ cakes in the chunk at (x, z)
        int obiCount = 0;
        container.clear();
        loot::blacksmith.getLootFromLootTableSeed<loot::GenMode::MODERN>(container, seed);
        for (int itemAt = 0; itemAt < container.slotCount(); itemAt++) {
            lce::ItemState item = container.getSlotAt(itemAt);
            if (item.id == lce::items::ids::OBSIDIAN_ID) {
                obiCount += item.myItemCount;
            }
        }

        if (obiCount < highestObiCount) continue;
        for (seed = (int64_t) ((uint64_t) (seed & 0xFFFFFFFFFFFFLL) | 0x8000000000000000ULL);
             seed <= maxSisterSeed; seed += 0x1000000000000) {
            int64_t prevSeed = isNextLong(seed);
            if (prevSeed != -1) {
                std::cout << "Found " << obiCount << " obsidian for seed: " << seed << " Previous seed: " << (prevSeed ^ 0x5deece66dLL) << std::endl;
            }
        }
        highestObiCount = obiCount;
    }
}

void findRealBlacksmiths() {
    constexpr i64 maxSisterSeed = 0x7FFF000000000000;
    std::string inputFile = "obiPopulationSeeds2.txt";
    std::string outputFile = "realBlacksmiths.txt";
    std::ifstream file(inputFile);
    std::ofstream output(outputFile);

    if (!file.is_open()) {
        std::cerr << "Could not open input file: " << inputFile << std::endl;
        return;
    }
    if (!output.is_open()) {
        std::cerr << "Could not open output file: " << outputFile << std::endl;
        return;
    }

    i64 seed;
    int xChunk, zChunk;
    std::vector<SeedVecPos> validSeeds;
    std::vector<Pos2D> validVillagePositions;
    Generator g(lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::LARGE);
    g.generateCache(4);
    gen::Village villageGen(&g, false);
    gen::Village villageGenWithBiomes(&g);
    Placement::Village<false>::setWorldSize(lce::WORLDSIZE::MEDIUM);
    auto villageRegionSize = (float)Placement::Village<false>::REGION_SIZE;
    while (file >> seed >> xChunk >> zChunk) {
        Pos2D highObiChunk(xChunk, zChunk);
        int regionX = std::floor((float)xChunk / villageRegionSize);
        int regionZ = std::floor((float)zChunk / villageRegionSize);
        validVillagePositions.clear();
        g.applyWorldSeed(seed);
        for (int rx = regionX - 1; rx <= regionX + 1; ++rx) {
            for (int rz = regionZ - 1; rz <= regionZ + 1; ++rz) {
                Pos2D villagePos = Placement::Village<false>::getRegionChunkPosition(seed, rx, rz);
                if (villagePos - highObiChunk < 9) {
                    villageGen.generate(villagePos);
                    if (villageGen.getBlackSmithPiece() != nullptr &&
                        villageGen.getBlackSmithPiece()->getWorldPos(5, 5).insideChunk(highObiChunk)) {
                        validVillagePositions.push_back(villagePos);
                    }
                }
            }
        }

        if (validVillagePositions.empty()) continue;

        std::cout << "Finding real blacksmiths for seed: " << seed <<
                " at chunk (" << xChunk << ", " << zChunk << ") with " <<
                validVillagePositions.size() << " valid villages..." << std::endl;
        for (seed = (int64_t) ((uint64_t) (seed & 0xFFFFFFFFFFFFLL) | 0x8000000000000000ULL);
             seed <= maxSisterSeed; seed += 0x1000000000000) {
            g.applyWorldSeed(seed);
            for (const Pos2D &villagePos: validVillagePositions) {
                if (!Placement::Village<false>::verifyChunkPosition(&g, villagePos)) continue;

                villageGenWithBiomes.generate(villagePos);
                if (!villageGenWithBiomes.hasMoreThanTwoComponents() || villageGenWithBiomes.getBlackSmithPiece() ==
                    nullptr) continue;

                // check if the blacksmith is a real one
                if (villageGenWithBiomes.getBlackSmithPiece()->getWorldPos(5, 5).insideChunk(highObiChunk)) {
                    std::vector<Pos2D> vec{highObiChunk, villagePos};
                    validSeeds.emplace_back(seed, vec);
                }
            }
        }
    }
    std::sort(validSeeds.begin(), validSeeds.end(), SeedVecPos::Less());
    for (const auto &seedVecPos: validSeeds) {
        output << seedVecPos.seed;
        for (const auto &pos: seedVecPos.pos) {
            output << " " << pos.x << " " << pos.z;
        }
        output << "\n";
    }
    output.close();
    file.close();
}

int main() {
    bruteForceObi();
    // findMostObi();
    // findRealBlacksmiths();
    /*int obiCount = 0;
    loot::Container27 container;
    loot::blacksmith.getLootFromChunk<loot::GenMode::MODERN>(container, 9390135920093555, -22, 5);
    for (int itemAt = 0; itemAt < container.slotCount(); itemAt++) {
        lce::ItemState item = container.getSlotAt(itemAt);
        if (item.id == lce::items::ids::OBSIDIAN_ID) {
            obiCount += item.myItemCount;
        }
    }
    std::cout << "Blacksmith loot: " << container << std::endl;
    std::cout << "Obsidian count: " << obiCount << std::endl;*/
    return 0;
}
