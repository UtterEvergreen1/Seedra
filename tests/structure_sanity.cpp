#include "structures/placement/DynamicStructures.hpp"   // adjust include path if needed
#include <cstdio>
#include <vector>
#include <unordered_set>
#include <utility>

using namespace Placement;

static inline int blockToChunk(int blockCoord) {
    // positions from getAllPossiblePositions are centers: (chunk<<4)+8
    return (blockCoord - 8) >> 4;
}

template<typename D>
void print_constants(const char* name) {
    // runtime view of constants from both places
    std::printf("\n[%s] constants:\n", name);
    std::printf("  SALT               : D=%d  DS=%d\n", D::SALT, DynamicStructure<D>::SALT);
    std::printf("  MAIN_RADIUS        : D=%d  DS=%d\n", D::MAIN_RADIUS, DynamicStructure<D>::MAIN_RADIUS);
    std::printf("  SECOND_RADIUS      : D=%d  DS=%d\n", D::SECOND_RADIUS, DynamicStructure<D>::SECOND_RADIUS);
    std::printf("  HAS_SECOND_CHECK   : D=%d  DS=%d\n",
                int(D::HAS_SECOND_BIOME_CHECK), int(DynamicStructure<D>::HAS_SECOND_BIOME_CHECK));
    std::printf("  REGION_SIZE        : %d\n", DynamicStructure<D>::REGION_SIZE);
    std::printf("  CHUNK_RANGE        : %d\n", DynamicStructure<D>::CHUNK_RANGE);
    std::printf("  CHUNK_BOUNDS       : %d\n", DynamicStructure<D>::CHUNK_BOUNDS);
    std::printf("  ATTEMPTS           : %d\n", DynamicStructure<D>::ATTEMPTS);
}

template<typename D>
bool check_positions(const char* name, long long worldSeed, int regionX, int regionZ, int maxPrint = 10) {
    auto posBlocks = DynamicStructure<D>::getAllPossiblePositions(worldSeed, regionX, regionZ);

    std::printf("\n[%s] region=(%d,%d) worldSeed=%lld\n", name, regionX, regionZ, worldSeed);
    std::printf("  candidates (block coords): count=%zu\n", posBlocks.size());
    int shown = 0;

    bool allOk = true;

    for (const auto& p : posBlocks) {
        const int cx = blockToChunk(p.x);
        const int cz = blockToChunk(p.z);

        const bool ok1 = DynamicStructure<D>::isPossibleChunkPos(worldSeed, regionX, regionZ, Pos2D{cx, cz});
        const bool ok2 = DynamicStructure<D>::canSpawnAtChunk(worldSeed, cx, cz, regionX, regionZ);

        if (shown < maxPrint) {
            std::printf("    block=(%d,%d)  chunk=(%d,%d)  isPossible=%d  canSpawn=%d\n",
                        p.x, p.z, cx, cz, int(ok1), int(ok2));
            ++shown;
        }

        if (!(ok1 && ok2)) {
            allOk = false;
            // Show the first failure in detail
            std::printf("    !! mismatch for chunk=(%d,%d): isPossible=%d canSpawn=%d\n", cx, cz, int(ok1), int(ok2));
            break;
        }
    }

    // Negative test: pick a chunk that is very unlikely to be one of the sampled ones.
    // (Deterministic but arbitrary offsets)
    if (!posBlocks.empty()) {
        const int cx_bad = blockToChunk(posBlocks[0].x) + 1234;
        const int cz_bad = blockToChunk(posBlocks[0].z) - 987;
        const bool neg1 = DynamicStructure<D>::isPossibleChunkPos(worldSeed, regionX, regionZ, Pos2D{cx_bad, cz_bad});
        const bool neg2 = DynamicStructure<D>::canSpawnAtChunk(worldSeed, cx_bad, cz_bad, regionX, regionZ);
        std::printf("  negative check at chunk=(%d,%d): isPossible=%d canSpawn=%d (expected both 0)\n",
                    cx_bad, cz_bad, int(neg1), int(neg2));
        if (neg1 || neg2) allOk = false;
    }

    std::printf("  verdict: %s\n", allOk ? "OK" : "FAIL");
    return allOk;
}

template<typename D>
bool run_suite(const char* name, lce::WORLDSIZE ws, long long worldSeed, int regionX, int regionZ) {
    // Configure spacing knobs deterministically for this test
    D::setWorldSize(ws);

    print_constants<D>(name);
    return check_positions<D>(name, worldSeed, regionX, regionZ);
}

// Optional: turn on a compile-time guard that the two views of the constants match.
// Define this macro (e.g. via CMake add_compile_definitions(STRICT_DERIVED_CONSTS))
// **only** if you implemented Option A (specializing the base members).
#ifdef STRICT_DERIVED_CONSTS
template<typename D>
constexpr bool kConstsAgree =
        (D::SALT == DynamicStructure<D>::SALT) &&
        (D::MAIN_RADIUS == DynamicStructure<D>::MAIN_RADIUS) &&
        (D::SECOND_RADIUS == DynamicStructure<D>::SECOND_RADIUS) &&
        (D::HAS_SECOND_BIOME_CHECK == DynamicStructure<D>::HAS_SECOND_BIOME_CHECK);

static_assert(kConstsAgree<Mansion>,        "Mansion constants disagreement");
static_assert(kConstsAgree<Monument>,       "Monument constants disagreement");
static_assert(kConstsAgree<BuriedTreasure>, "BuriedTreasure constants disagreement");
static_assert(kConstsAgree<Shipwreck>,      "Shipwreck constants disagreement");
static_assert(kConstsAgree<Outpost>,        "Outpost constants disagreement");
#endif

int main() {
    const long long worldSeed = 123456789ll;
    const int regionX = 7;
    const int regionZ = -3;

    // Use a fixed world size to make results stable across runs.
    // MEDIUM disables reduced spacing in your code (so you exercise that path).
    bool ok = true;
    ok &= run_suite<Mansion>       ("Mansion",        lce::WORLDSIZE::MEDIUM, worldSeed, regionX, regionZ);
    ok &= run_suite<Monument>      ("Monument",       lce::WORLDSIZE::MEDIUM, worldSeed, regionX, regionZ);
    ok &= run_suite<BuriedTreasure>("BuriedTreasure", lce::WORLDSIZE::MEDIUM, worldSeed, regionX, regionZ);
    ok &= run_suite<Shipwreck>     ("Shipwreck",      lce::WORLDSIZE::MEDIUM, worldSeed, regionX, regionZ);
    ok &= run_suite<Outpost>       ("Outpost",        lce::WORLDSIZE::MEDIUM, worldSeed, regionX, regionZ);

    std::puts(ok ? "\nALL TESTS OK" : "\nSOME TESTS FAILED");
    return ok ? 0 : 1;
}
