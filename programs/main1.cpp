
/*
#include "LegacyCubiomes/cubiomes/include.hpp"
#include "LegacyCubiomes/enchants/include.hpp"
#include "LegacyCubiomes/loot/include.hpp"
#include "LegacyCubiomes/structures/include.hpp"
#include "LegacyCubiomes/utils/include.hpp"

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
*/

#include "LegacyCubiomes/utils/constants.hpp"
#include "lce/processor.hpp"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>


struct M_Cache {
    int64_t x[54] = {0};
    int64_t z[54] = {0};
};

int getWorldRavineTotalCount(int64_t worldSeed) {
    int ravineCount = 0;
    int64_t currentX, currentZ;
    static M_Cache m_cache;
    uint64_t seed, ret1;
    static constexpr int RANGE = 27;

    worldSeed ^= 0x5deece66d;
    seed = worldSeed & 0xFFFFFFFFFFFF;

    // seedMultiplierX nextLong()
    ret1 = ((seed * 0x5deece66d + 0xb) & 0xFFFFFFFF0000) << 16;
    seed = seed * 0xBB20B4600A69 + 0x40942DE6BA;
    c_i64 seedMultiplierX = (int64_t) (ret1 + (int) (seed >> 16));

    // seedMultiplierZ nextLong()
    ret1 = ((seed * 0x5deece66d + 0xb) & 0xFFFFFFFF0000) << 16;
    seed = seed * 0xBB20B4600A69 + 0x40942DE6BA;
    c_i64 seedMultiplierZ = (int64_t) (ret1 + (int) (seed >> 16));

    // create multiplication cache
    #pragma unroll
    for (int64_t index = -RANGE; index < 1; ++index) {
        m_cache.x[index + RANGE] = index * seedMultiplierX;
        m_cache.z[index + RANGE] = index * seedMultiplierZ;
    }
    #pragma unroll
    for (int64_t index = 1; index < RANGE; ++index) {
        m_cache.x[RANGE + index] = -m_cache.x[RANGE - index];
        m_cache.z[RANGE + index] = -m_cache.z[RANGE - index];
    }

    // check each chunk
    for (currentX = 0; currentX < 2 * RANGE; ++currentX) {
        for (currentZ = 0; currentZ < 2 * RANGE; ++currentZ) {
            seed = ((m_cache.x[currentX] ^ m_cache.z[currentZ] ^ worldSeed)
                            * 0x5deece66d + 0xb) & 0x0000FFFFFFFFFFFF;
            if EXPECT_FALSE((int) (seed >> 17) % 50 == 0) {
                ravineCount++;
            }
        }
    }
    return ravineCount;
}

#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"


int main(int argc, char* argv[]) {

    Pos2DTemplate<int> point(5, 4);
    point = point + Pos2DTemplate<int>(4, 5);
    std::cout << point << std::endl;


    Pos2DTemplate<double> point2(1.0, 3.14);

    std::cout << point2 << std::endl;


    int xxx; std::cin >> xxx;


    int64_t ravine_seed;
    int mostRavines = 110;
    std::vector<int64_t> seeds;

    uint64_t start = getMilliseconds();

    int ravineCount;
    for (int64_t index = 100000000; index < 200000000; index++) {
        ravineCount = getWorldRavineTotalCount(index);
        if EXPECT_FALSE (ravineCount < mostRavines)
            continue;

        if EXPECT_FALSE(ravineCount == mostRavines) {
            seeds.push_back(index);
            continue;
        }

        seeds.clear();
        seeds.push_back(index);
        ravine_seed = index;
        mostRavines = ravineCount;
        float percentage = float(mostRavines) / 29.16F;
        printf("Seed: %llu | Ravines: %d (%.2f%%)\n", ravine_seed, mostRavines, percentage);
    }
    uint64_t end = getMilliseconds();
    float diff = (float)(end - start) / 1000;
    printf("\nTime: %.3f\n", diff);

    printf("\n\nOther Seeds:\n");
    for (int64_t index : seeds) {
        printf("%llu\n", index);
    }
    printf("\n\nDone! Hit enter to exit.");
    int x; std::cin >> x;
}




/*
Biome::registerBiomes();

auto console = CONSOLE::WIIU;
auto version = LCEVERSION::AQUATIC;
EnchantmentHelper::setup(console, version);
Generator g(console, version, 119, WORLDSIZE::CLASSIC, BIOMESCALE::SMALL);
    std::string path = R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\)";

int* ids;
auto start = getMilliseconds();

int ROLLS = 10000;
for (int i = 0; i < ROLLS; i++) {
    g.incrementSeed();
    ids = g.getBiomeRange(1, 0, 0, 16, 16);
}

auto diff = getMilliseconds() - start;

std::cout << ids << std::endl;
std::cout << "rolls: " << ROLLS << std::endl;
std::cout << "time:  " << diff << std::endl;
*/