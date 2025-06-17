#include <algorithm>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <vector>

#include "common/MathHelper.hpp"
#include "common/timer.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/carve/RavineGenerator.hpp"
#include "terrain/generator.hpp"



static constexpr i64 WORLDSEED = 1526726; // -6651998285536156346LL;
static constexpr auto CONSOLE = lce::CONSOLE::WIIU;
static constexpr auto VERSION = LCEVERSION::ELYTRA;
static constexpr auto WORLDSIZE = lce::WORLDSIZE::CLASSIC;
static constexpr auto BIOMESCALE = lce::BIOMESCALE::SMALL;

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


Score getBest(World& world) {
    Timer start;

    RNG rng;
    const Pos2D spawnChunk = world.getGenerator()->estimateSpawn(rng);

    world.createChunks(spawnChunk / 16, SEARCH_RAD);
    std::cout << "Create World Time: " << start.getSeconds() << "\n";
    start.reset();

    world.decorateCaves(spawnChunk / 16, SEARCH_RAD, false);
    std::cout << "Decorate World Time: " << start.getSeconds() << "\n";
    start.reset();

    const Pos2D lower(world.worldBounds.minX, world.worldBounds.minZ);
    const Pos2D upper(world.worldBounds.maxX, world.worldBounds.maxZ);



    // scan columns into a flat grid
    c_i32 colsX = (upper.x - lower.x + 1) * 16;
    c_i32 colsZ = (upper.z - lower.z + 1) * 16;
    std::vector<ColStat> grid(colsX * colsZ, {0, 0, 0});
    auto cell = [&](c_i32 gx, c_i32 gz) -> ColStat& { return grid[gz * colsX + gx]; };

    for (auto& [pos, chunk] : world.chunks) {
        for (int lx = 0; lx < 16; ++lx) {
            for (int lz = 0; lz < 16; ++lz) {
                c_i32 gx = (pos.x - lower.x) * 16 + lx;
                c_i32 gz = (pos.z - lower.z) * 16 + lz;
                c_u16* col = chunk->blocks + ChunkPrimer::getStorageIndex(lx, 0, lz);
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
        return air - 2 * water - 8 * lava; //  - 8 * water - 16 * lava;
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


int main() {
    Biome::registerBiomes();
    Generator g(CONSOLE, VERSION, WORLDSEED, WORLDSIZE, BIOMESCALE);
    World world(&g);

    for (int worldSeed = WORLDSEED; worldSeed < WORLDSEED + 10000; worldSeed++) {
        world.deleteWorld();
        world.getGenerator()->applyWorldSeed(worldSeed);

        const Score best = getBest(world);

        // std::cout << "Time : " << start.getSeconds() << "\n";
        std::cout << "Spawn: C" << best.spawn / 16 << " / B" << best.spawn << "\n";
        std::cout << "Chunk: C" << best.pos / 16 << " / B" << best.pos << "\n";
        std::cout << "Air=" << best.air << "  Water=" << best.water << "  Lava=" << best.lava << "\n\n";
        std::cout << std::endl;
    }


    return 0;
}
