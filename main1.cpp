
#include <fstream>
#include <iostream>

#include "LegacyCubiomes/cubiomes/include.hpp"
#include "LegacyCubiomes/enchants/include.hpp"
#include "LegacyCubiomes/loot/include.hpp"
#include "LegacyCubiomes/structures/include.hpp"
#include "LegacyCubiomes/utils/include.hpp"

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"


#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>



struct M_Cache {
    int64_t x[54] = {0};
    int64_t z[54] = {0};
};

int getWorldRavineTotalCount(int64_t worldSeed) {
    int ravineCount = 0;
    int64_t currentX, currentZ;
    M_Cache m_cache;

    RNG rng1;
    rng1.setSeed(worldSeed);
    const int64_t seedMultiplierX = (int64_t) rng1.nextLong();
    const int64_t seedMultiplierZ = (int64_t) rng1.nextLong();

    #pragma unroll
    for (int64_t index = -27; index < 1; ++index) {
        m_cache.x[index + 27] = index * seedMultiplierX;
        m_cache.z[index + 27] = index * seedMultiplierZ;
    }

    #pragma unroll
    for (int64_t index = 1; index < 27; ++index) {
        m_cache.x[27 + index] = -m_cache.x[27 - index];
        m_cache.z[27 + index] = -m_cache.z[27 - index];
    }

    for (currentX = 0; currentX < 54; ++currentX) {
        #pragma unroll
        for (currentZ = 0; currentZ < 54; ++currentZ) {
            rng1.setSeed(m_cache.x[currentX] ^ m_cache.z[currentZ] ^ worldSeed);
            if EXPECT_FALSE (rng1.nextInt(50) == 0) {
                ravineCount++;
            }
        }
    }
    return ravineCount;
}


int main(int argc, char* argv[]) {

    int64_t ravine_seed;
    int mostRavines = INT32_MIN;

    uint64_t start = getMilliseconds();

    int ravineCount;
    for (int index = 0; index < 100000; index++) {
        ravineCount = getWorldRavineTotalCount(index);
        if EXPECT_FALSE (ravineCount <= mostRavines)
            continue;

        ravine_seed = index;
        mostRavines = ravineCount;
        printf("Seed: %llu | Ravines: %d\n", ravine_seed, mostRavines);
    }
    uint64_t end = getMilliseconds();
    float diff = (float)(end - start) / 1000;
    printf("\nTime: %.3f\n", diff);

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