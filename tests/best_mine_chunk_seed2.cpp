#include <algorithm>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <vector>

#include "common/MathHelper.hpp"
#include "common/timer.hpp"
#include "include/doctest.h"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/carve/RavineGenerator.hpp"
#include "terrain/finders/LargeCaveFinder.hpp"
#include "terrain/generator.hpp"


bool testOneSeed = true;

i64 WORLDSEED = 0; //-6651998285536156346LL;
i64 SEEDS_TO_TRAVERSE = 100'000'000;
auto CONSOLE = lce::CONSOLE::WIIU;
auto VERSION = LCEVERSION::ELYTRA;
auto WORLDSIZE = lce::WORLDSIZE::CLASSIC;
auto BIOMESCALE = lce::BIOMESCALE::SMALL;

static i64 WATER_WEIGHT = -2;
static i64 LAVA_WEIGHT = -8;

static constexpr int LAVA_BUDGET   = 4'000; // max lava inside window
static constexpr int SEARCH_RAD    = 8;     // chunks around spawn
static constexpr int WINDOW_STRIDE = 1;     // offset between each x/z check


struct ColStat {
    u16 air, water, lava;
};

/// SIMD helper – counts zeros in 256 column blocks
static u16 countAirSIMD(c_u16* colBase) {
    const __m256i zero = _mm256_setzero_si256();
    const u16* p  = colBase;      // walk pointer (y-axis)
    u32 acc = 0;
    for (int y = 0; y < 256; y += 16, p += 16) {        // 16 × u16 per AVX2 vector
        const __m256i v    = _mm256_loadu_si256((const __m256i*)p);
        const __m256i cmp  = _mm256_cmpeq_epi16(v, zero);
        // movemask gives 32 bits – one per *byte* – set when that byte’s MSB is 1.
        // A 16-bit lane that is all-zero produces TWO set bits, so popcount(mask)/2
        // is the number of zero lanes in this 256-bit chunk.
        const u32 mask  = _mm256_movemask_epi8(cmp);
        acc += __builtin_popcount(mask) >> 1;           // divide by 2
    }
    return static_cast<u16>(acc);   // fits: 16 lanes × 16 vectors = 256 max
}


ColStat scanColumn(const u16* col) {
    ColStat s{0, 0, 0};

    s.air = countAirSIMD(col);
    
    for (int y = 0; y < 256; ++y) {
        const u16 id = col[y] >> 4;
        if (id == lce::blocks::STILL_WATER_ID) ++s.water;
        else if (id == lce::blocks::STILL_LAVA_ID) ++s.lava;
    }

    return s;
}


struct Score {
    Pos2D spawn;
    Pos2D pos;
    u32 air, water, lava;
};


Score getBest(World& world, std::vector<LargeCaveFinder::Results>& results) {

    Timer start;

    RNG rng;
    const Pos2D spawnChunk = world.getGenerator()->estimateSpawn(rng);
    // AreaRange COMPUTE_RANGE = {results[0].start.asPos2D().asType<int>() / 16,
    //         SEARCH_RAD, false, false};
    AreaRange COMPUTE_RANGE = {{0, 0},
                               27, false, false};


    world.createChunks(COMPUTE_RANGE);
    std::cout << "Create World Time: " << start.getSeconds() << "\n";
    start.reset();

    world.decorateCaves(COMPUTE_RANGE, false);
    std::cout << "Decorate World Time: " << start.getSeconds() << "\n";
    start.reset();

    const Pos2D lower = COMPUTE_RANGE.getLower(); // (world.worldBounds.minX, world.worldBounds.minZ);
    const Pos2D upper = COMPUTE_RANGE.getUpper(); // (world.worldBounds.maxX, world.worldBounds.maxZ);



    // scan columns into a flat grid
    c_i32 colsX = (upper.x - lower.x + 1) * 16;
    c_i32 colsZ = (upper.z - lower.z + 1) * 16;
    std::vector<ColStat> grid(colsX * colsZ, {0, 0, 0});
    auto cell = [&](c_i32 gx, c_i32 gz) -> ColStat& { return grid[gz * colsX + gx]; };

    int x = -1;
    for (auto& [pos, chunk] : world.chunks) {
        x++;
        for (int lx = 0; lx < 16; ++lx) {
            for (int lz = 0; lz < 16; ++lz) {
                c_i32 gx = (pos.x - lower.x) * 16 + lx;
                c_i32 gz = (pos.z - lower.z) * 16 + lz;
                c_u16* col = chunk->blocks.data() +
                             ChunkPrimer::getStorageIndex(lx, 0, lz);
                cell(gx, gz) = scanColumn(col);
            }
        }
    }


    std::cout << "Scan Column Time: " << start.getSeconds() << "\n";
    start.reset();

    // build 2-D prefix sums for air / water / lava
    auto makePrefix = [&](auto member) -> u32_vec {
        u32_vec ps(colsX * colsZ, 0);
        for (int z = 0; z < colsZ; ++z)
            for (int x = 0; x < colsX; ++x) {
                c_u32 val = (grid[z * colsX + x].*member);
                c_u32 left = x ? ps[z * colsX + x - 1] : 0;
                c_u32 up = z ? ps[(z - 1) * colsX + x] : 0;
                c_u32 diag = (x && z) ? ps[(z - 1) * colsX + x - 1] : 0;
                ps[z * colsX + x] = val + left + up - diag;
            }
        return ps;
    };
    const auto psAir = makePrefix(&ColStat::air);
    const auto psWater = makePrefix(&ColStat::water);
    const auto psLava = makePrefix(&ColStat::lava);

    auto sum = [&](const u32_vec& ps, c_i32 x1, c_i32 z1, c_i32 x2, c_i32 z2) {
        c_u32 A = ps[z2 * colsX + x2];
        c_u32 B = (x1 ? ps[z2 * colsX + x1 - 1] : 0);
        c_u32 C = (z1 ? ps[(z1 - 1) * colsX + x2] : 0);
        c_u32 D = (x1 && z1) ? ps[(z1 - 1) * colsX + x1 - 1] : 0;
        return A - B - C + D;
    };

    // slide 16×16 window
    Score best{{0, 0}, {0, 0}, 0, 0, UINT32_MAX};

    auto scoreVal = [](c_u32 air, c_u32 water, c_u32 lava) {
        return air - WATER_WEIGHT * water - LAVA_WEIGHT * lava;
    };

    for (int sx = 0; sx + 15 < colsX; sx += WINDOW_STRIDE) {
        for (int sz = 0; sz + 15 < colsZ; sz += WINDOW_STRIDE) {
            c_u32 air = sum(psAir, sx, sz, sx + 15, sz + 15);
            c_u32 water = sum(psWater, sx, sz, sx + 15, sz + 15);
            c_u32 lava = sum(psLava, sx, sz, sx + 15, sz + 15);

            if (lava > LAVA_BUDGET) continue;

            if (scoreVal(air, water, lava) > scoreVal(best.air, best.water, best.lava)) {
                best = {{}, {sx, sz}, air, water, lava};
            }
        }
    }

    std::cout << "Scoring Time: " << start.getSeconds() << "\n";
    start.reset();

    best.spawn = spawnChunk;
    best.pos = best.pos + lower * 16;

    return best;
}


int main(int argc, char* argv[]) {

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();  // Runs all tests

    if (context.shouldExit()) return res;





    if (testOneSeed) {
        std::int64_t a, b, c;
        std::string d, e, f;

        std::cout << "Enter Console: ";
        if (!(std::cin >> e)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        CONSOLE = lce::strToConsole(e);

        std::cout << "Enter Version(aquatic | elytra): ";
        if (!(std::cin >> f)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        VERSION = strToLCEVERSION(f);

        std::cout << "Enter Seed: ";
        if (!(std::cin >> a)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        WORLDSEED = a;

        std::cout << "Enter Biome Scale: ";
        if (!(std::cin >> d)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        BIOMESCALE = lce::strToBiomeScale(d);

        std::cout << "Enter Water Weight: ";
        if (!(std::cin >> b)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        WATER_WEIGHT = b;

        std::cout << "Enter Lava Weight: ";
        if (!(std::cin >> c)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        LAVA_WEIGHT = c;

        SEEDS_TO_TRAVERSE = 1;
    }





    int RADIUS = 27;
    Pos2D WIDTH(2 * RADIUS, 2 * RADIUS);
    Pos2D CENTER(0, 0);

    AreaRange CAVE_RANGE = {CENTER, RADIUS, false, false};


    Biome::registerBiomes();
    Generator g(CONSOLE, VERSION, WORLDSEED, WORLDSIZE, BIOMESCALE);
    World world(&g);

    for (i64 worldSeed = WORLDSEED; worldSeed > WORLDSEED - SEEDS_TO_TRAVERSE; worldSeed--) {
        world.getGenerator()->applyWorldSeed(worldSeed);

        auto results = LargeCaveFinder::findAllLargeCaves(&g, CAVE_RANGE);

        if (!testOneSeed) {
            if (results.empty())
                continue;
        }

        const Score best = getBest(world, results);

        // std::cout << "Time : " << start.getSeconds() << "\n";
        std::cout << "Seed   : " << worldSeed << "\n";
        std::cout << "Spawn:  C" << best.spawn / 16 << " / B" << best.spawn << "\n";
        std::cout << "Chunk:  C" << best.pos / 16 << " / B" << best.pos << "\n";
        std::cout << "Air=" << best.air << "  Water=" << best.water << "  Lava=" << best.lava << "\n\n";
        std::cout << "Segment: ";
        for (auto& res : results) {
            std::cout << res.start << ", ";
        }
        std::cout << "\n";
        std::cout << std::endl;

        results.clear();
        world.deleteWorld();
    }

    int x;
    std::cin >> x;
    return 0;
}
