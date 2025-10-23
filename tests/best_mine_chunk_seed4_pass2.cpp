#include <algorithm>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <vector>

#include "common/MathHelper.hpp"
#include "common/timer.hpp"
#include "include/json.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"

using nlohmann::json;
namespace fs = std::filesystem;


const std::vector<lce::BIOMESCALE> BIOME_SCALES = {
        lce::BIOMESCALE::SMALL, lce::BIOMESCALE::MEDIUM, lce::BIOMESCALE::LARGE};

static i64 AIR_WEIGHT = 1;
static i64 WATER_WEIGHT = 0;
static i64 LAVA_WEIGHT = 64;

static constexpr int LAVA_BUDGET   = 4'000; // max lava inside window
static constexpr int WINDOW_STRIDE = 1;     // offset between each x/z check




struct BestResult {
    i64 seed;
    int chunkX;
    int chunkZ;
    int startX;
    int startY;
    int startZ;
    double score;
    double coverY;
    double marginMax;
    double maxR;
    double avgVert;
    double spanY;
};


std::vector<BestResult> loadBestJson(const fs::path& path) {
    std::vector<BestResult> results;

    std::ifstream f(path);
    if (!f) {
        std::cerr << "[error] Could not open " << path << "\n";
        return results;
    }

    json j;
    try {
        f >> j;
    } catch (const std::exception& e) {
        std::cerr << "[error] Failed to parse " << path << ": " << e.what() << "\n";
        return results;
    }

    if (!j.contains("top") || !j["top"].is_array()) {
        std::cerr << "[error] Missing or invalid 'top' array in " << path << "\n";
        return results;
    }

    for (const auto& item : j["top"]) {
        BestResult r{};
        r.seed      = item.value("seed", 0LL);
        r.score     = item.value("score", 0.0);
        r.coverY    = item.value("coverY", 0.0);
        r.marginMax = item.value("marginMax", 0.0);
        r.maxR      = item.value("maxR", 0.0);
        r.avgVert   = item.value("avgVert", 0.0);
        r.spanY     = item.value("spanY", 0.0);

        if (item.contains("chunk")) {
            r.chunkX = item["chunk"].value("x", 0);
            r.chunkZ = item["chunk"].value("z", 0);
        }

        if (item.contains("start")) {
            r.startX = item["start"].value("x", 0);
            r.startY = item["start"].value("y", 0);
            r.startZ = item["start"].value("z", 0);
        }

        results.push_back(std::move(r));
    }

    std::cout << "[info] Loaded " << results.size() << " results from " << path << "\n";
    return results;
}



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
    double time_chunks;
    double time_decorate;
    double time_column;
    double time_score;
    i64 score;
    Pos2D spawn;
    Pos2D pos;
    u32 air, water, lava;
};


i64 scoreVal(c_u32 air, c_u32 water, c_u32 lava) {
    return static_cast<i64>(AIR_WEIGHT * air - WATER_WEIGHT * water - LAVA_WEIGHT * lava);
};


Score getScore(World& world, const AreaRange& caveRange) {

    const Pos2D lower = caveRange.getLower();
    const Pos2D upper = caveRange.getUpper();


    Timer start;



    RNG rng;
    const Pos2D spawnChunk = world.getGenerator()->estimateSpawn(rng);

    world.createChunks(caveRange);
    const double time_chunks = start.getSeconds();
    start.reset();

    world.decorateCaves(caveRange, false);
    const double time_decorate = start.getSeconds();
    start.reset();


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

    const double time_column = start.getSeconds();
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
    Score best{0, 0, 0, 0, 0, {0, 0}, {0, 0}, 0, 0, UINT32_MAX};

    for (int sx = 0; sx + 15 < colsX; sx += WINDOW_STRIDE) {
        for (int sz = 0; sz + 15 < colsZ; sz += WINDOW_STRIDE) {
            c_u32 air = sum(psAir, sx, sz, sx + 15, sz + 15);
            c_u32 water = sum(psWater, sx, sz, sx + 15, sz + 15);
            c_u32 lava = sum(psLava, sx, sz, sx + 15, sz + 15);

            if (lava > LAVA_BUDGET) continue;

            i64 score = scoreVal(air, water, lava);
            if (score > scoreVal(best.air, best.water, best.lava)) {
                best = {0, 0, 0, 0, score, {}, {sx, sz}, air, water, lava};
            }
        }
    }

    best.spawn = spawnChunk;
    best.pos = best.pos + lower * 16;
    best.time_score = start.getSeconds();
    best.time_column = time_column;
    best.time_chunks = time_chunks;
    best.time_decorate = time_decorate;

    return best;
}


static inline bool writeJsonPretty(const fs::path& path, const json& j) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out << j.dump(2);
    out.flush();
    return static_cast<bool>(out);
}

static inline json make_record_from_score(const Score& s,
                                          i64 seed,
                                          lce::BIOMESCALE scale,
                                          int rChunkX, int rChunkZ)
{
    json rec;
    rec["seed"]         = seed;
    rec["biome_scale"]  = lce::biomeScaleToString(scale);

    // Driving location (from best.json candidate)
    rec["candidate_chunk"] = { {"x", rChunkX}, {"z", rChunkZ} };

    // Spawn + scored window location (blocks)
    rec["spawn_block"] = { {"x", s.spawn.x}, {"z", s.spawn.z} };
    rec["best_window_block"] = { {"x", s.pos.x}, {"z", s.pos.z} };

    // Also include chunk coords of that 16×16 window anchor (block >> 4)
    rec["best_window_chunk"] = { {"x", s.pos.x / 16}, {"z", s.pos.z / 16} };

    // Counts over the 16×16 window
    rec["air"]    = s.air;
    rec["water"]  = s.water;
    rec["lava"]   = s.lava;

    // Timing diagnostics
    rec["timing"] = {
            { "create_chunks_sec",  s.time_chunks   },
            { "decorate_caves_sec", s.time_decorate },
            { "scan_columns_sec",   s.time_column   },
            { "score_window_sec",   s.time_score    }
    };

    // Constants that affected scoring (useful for later analysis)
    rec["window_size"]   = 16;
    rec["lava_budget"]   = LAVA_BUDGET;
    rec["water_weight"]  = WATER_WEIGHT;
    rec["lava_weight"]   = LAVA_WEIGHT;

    return rec;
}


struct EvalRow {
    i64 seed;
    lce::BIOMESCALE scale;
    int candidateChunkX;
    int candidateChunkZ;
    Score s;
};


static inline nlohmann::json make_record_with_score(const EvalRow& row, long long score) {
    nlohmann::json rec = make_record_from_score(
            row.s, row.seed, row.scale, row.candidateChunkX, row.candidateChunkZ
    );
    rec["score"] = score;
    return rec;
}


int main(int argc, char* argv[]) {
    Biome::registerBiomes();

    // vars
    auto CONSOLE = lce::CONSOLE::WIIU;
    auto VERSION = LCEVERSION::ELYTRA;
    auto WORLDSIZE = lce::WORLDSIZE::CLASSIC;

    // instantiate world object we will be using
    Generator g(CONSOLE, VERSION, 0, WORLDSIZE, lce::BIOMESCALE::SMALL);
    World world(&g);

    // load results
    auto bestResults = loadBestJson("best2.json");
    constexpr int RADIUS_CHUNKS = 8;

    // collect all rows first
    std::vector<EvalRow> rows;
    rows.reserve(bestResults.size() * BIOME_SCALES.size());

    // iterate results
    int rc = 0;
    for (int i = 0; i < bestResults.size(); i++) {
        const auto& r = bestResults[i];

        for (auto scale : BIOME_SCALES) {
            world.getGenerator()->changeBiomeSize(scale);
            world.getGenerator()->applyWorldSeed(r.seed);

            AreaRange caveRange(
                    Pos2D(r.chunkX, r.chunkZ),  // center chunk
                    RADIUS_CHUNKS,              // radius in chunks
                    /*reverseX=*/false,
                    /*reverseZ=*/false
            );

            const Score best = getScore(world, caveRange);

            std::cout << "[" << rc << ", "
                      << r.seed << ", "
                      << lce::biomeScaleToString(scale) << "]"
                      << " " << r.score
                      << " | (" << r.chunkX * 16 << ", " << r.chunkZ * 16 << ") "
                      << " Score: " << best.score << " | Air: " << best.air
                      << " | Water: " << best.water
                      << " | Lava: " << best.lava << "\n";

            rows.push_back(EvalRow{
                    r.seed, scale, r.chunkX, r.chunkZ, best
            });
            world.deleteWorld();
        }
        rc++;
    }

    // sort them all
    std::sort(rows.begin(), rows.end(),
          [](const EvalRow& a, const EvalRow& b) {
              c_i64 sa = a.s.score;
              c_i64 sb = b.s.score;
              if (sa != sb)              return sa > sb;             // score (desc)
              if (a.s.air != b.s.air)    return a.s.air > b.s.air;   // air (desc)
              if (a.s.lava != b.s.lava)  return a.s.lava < b.s.lava; // lava (asc)
              return a.s.water < b.s.water;                          // water (asc)
          }
    );

    // output json scaffold
    json out;
    out["source"]       = "best.json";
    out["generated_at"] = static_cast<long long>(std::time(nullptr));
    out["records"]      = json::array();

    for (size_t i = 0; i < rows.size(); ++i) {
        const auto& row = rows[i];
        c_i64 sc = scoreVal(row.s.air, row.s.water, row.s.lava);

        nlohmann::json rec = make_record_with_score(row, sc);
        rec["rank"] = static_cast<unsigned>(i + 1); // 1-based rank after sorting
        out["records"].push_back(std::move(rec));
    }

    const std::filesystem::path outPath = "best_eval.json";
    if (!writeJsonPretty(outPath, out)) {
        std::cerr << "[error] failed to write " << outPath << "\n";
        return 1;
    }

    std::cout << "[ok] wrote " << outPath << " with "
              << out["records"].size() << " records (sorted)\n";


    int x;
    std::cin >> x;
    return 0;
}
