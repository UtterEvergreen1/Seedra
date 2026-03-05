#include "include/doctest.h"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/WorldConfig.hpp"
#include "terrain/PopulationReverser.hpp"
#include "common/rng.hpp"

// ============================================================
//  ChunkPrimer – storage index
// ============================================================
TEST_SUITE("ChunkPrimer::getStorageIndex") {

    TEST_CASE("origin maps to 0") {
        CHECK(ChunkPrimer::getStorageIndex(0, 0, 0) == 0);
    }

    TEST_CASE("XZY layout: x stride is 4096, z stride is 256, y stride is 1") {
        // index = x<<12 | z<<8 | y
        CHECK(ChunkPrimer::getStorageIndex(1, 0, 0) == (1 << 12));
        CHECK(ChunkPrimer::getStorageIndex(0, 0, 1) == (1 << 8));
        CHECK(ChunkPrimer::getStorageIndex(0, 1, 0) == 1);
    }

    TEST_CASE("corner (15,255,15) is within storage") {
        const size_t idx = ChunkPrimer::getStorageIndex(15, 255, 15);
        CHECK(idx < 16u * 256u * 16u);
    }
}

// ============================================================
//  ChunkPrimer – isInvalidIndex
// ============================================================
TEST_SUITE("ChunkPrimer::isInvalidIndex") {

    TEST_CASE("valid coords are not invalid") {
        CHECK_FALSE(ChunkPrimer::isInvalidIndex(0,   0,   0));
        CHECK_FALSE(ChunkPrimer::isInvalidIndex(15,  255, 15));
        CHECK_FALSE(ChunkPrimer::isInvalidIndex(7,   128, 7));
    }

    TEST_CASE("x out of [0,15] is invalid") {
        CHECK(ChunkPrimer::isInvalidIndex(-1,  0, 0));
        CHECK(ChunkPrimer::isInvalidIndex(16,  0, 0));
    }

    TEST_CASE("z out of [0,15] is invalid") {
        CHECK(ChunkPrimer::isInvalidIndex(0,  0, -1));
        CHECK(ChunkPrimer::isInvalidIndex(0,  0, 16));
    }

    TEST_CASE("y out of [0,255] is invalid") {
        CHECK(ChunkPrimer::isInvalidIndex(0, -1,  0));
        CHECK(ChunkPrimer::isInvalidIndex(0, 256, 0));
    }
}

// ============================================================
//  ChunkPrimer – block get/set round-trip
// ============================================================
TEST_SUITE("ChunkPrimer block access") {

    TEST_CASE("fresh chunk is all air") {
        ChunkPrimer cp;
        for (int x = 0; x < 16; ++x)
            for (int z = 0; z < 16; ++z)
                for (int y = 0; y < 8; ++y)
                    CHECK(cp.getBlockId(x, y, z) == lce::blocks::AIR_ID);
    }

    TEST_CASE("setBlockId / getBlockId round-trip") {
        ChunkPrimer cp;
        cp.setBlockId(3, 64, 7, 1);   // stone = 1
        CHECK(cp.getBlockId(3, 64, 7) == 1);
        // neighbours untouched
        CHECK(cp.getBlockId(2, 64, 7) == lce::blocks::AIR_ID);
        CHECK(cp.getBlockId(3, 63, 7) == lce::blocks::AIR_ID);
        CHECK(cp.getBlockId(3, 64, 6) == lce::blocks::AIR_ID);
    }

    TEST_CASE("setBlockAndData encodes id and data correctly") {
        ChunkPrimer cp;
        cp.setBlockAndData(1, 1, 1, /*id=*/5, /*data=*/3);
        const lce::BlockState bs = cp.getBlock(1, 1, 1);
        CHECK(bs.getID()      == 5);
        CHECK(bs.getDataTag() == 3);
    }

    TEST_CASE("setBlock(Pos3D) and getBlock(Pos3D) round-trip") {
        ChunkPrimer cp;
        const lce::BlockState stone = lce::BlockState(1, 0);
        const Pos3D pos{5, 100, 12};
        cp.setBlock(pos, stone);
        CHECK(cp.getBlock(pos).getID() == 1);
    }

    TEST_CASE("out-of-bounds setBlockId is silently ignored") {
        ChunkPrimer cp;
        cp.setBlockId(16, 0, 0, 1);   // x=16 is invalid
        // storage should be untouched
        for (int y = 0; y < 4; ++y)
            CHECK(cp.getBlockId(0, y, 0) == lce::blocks::AIR_ID);
    }

    TEST_CASE("out-of-bounds getBlockId returns 0") {
        ChunkPrimer cp;
        CHECK(cp.getBlockId(-1, 0, 0)  == 0);
        CHECK(cp.getBlockId(0, 256, 0) == 0);
        CHECK(cp.getBlockId(0, 0, 16)  == 0);
    }

    TEST_CASE("isAirBlock reflects block state") {
        ChunkPrimer cp;
        CHECK(cp.isAirBlock(0, 0, 0));
        cp.setBlockId(0, 0, 0, 1);
        CHECK_FALSE(cp.isAirBlock(0, 0, 0));
    }

    TEST_CASE("mutBlockPtr returns nullptr for out-of-bounds") {
        ChunkPrimer cp;
        CHECK(cp.mutBlockPtr(-1, 0, 0)   == nullptr);
        CHECK(cp.mutBlockPtr(0, 256, 0)  == nullptr);
    }

    TEST_CASE("mutBlockPtr allows direct write") {
        ChunkPrimer cp;
        u16* ptr = cp.mutBlockPtr(2, 10, 3);
        REQUIRE(ptr != nullptr);
        *ptr = static_cast<u16>(7 << 4); // id=7, data=0
        CHECK(cp.getBlockId(2, 10, 3) == 7);
    }

    TEST_CASE("reset clears all blocks back to air") {
        ChunkPrimer cp;
        cp.setBlockId(0, 0, 0, 1);
        cp.setBlockId(15, 255, 15, 2);
        cp.reset();
        CHECK(cp.getBlockId(0,   0,   0)   == lce::blocks::AIR_ID);
        CHECK(cp.getBlockId(15,  255, 15)  == lce::blocks::AIR_ID);
    }
}

// ============================================================
//  ChunkPrimer – getHighestYBlock
// ============================================================
TEST_SUITE("ChunkPrimer::getHighestYBlock") {

    TEST_CASE("empty chunk returns 0") {
        ChunkPrimer cp;
        CHECK(cp.getHighestYBlock() == 0);
    }

    TEST_CASE("single block at y=64 returns 64") {
        ChunkPrimer cp;
        cp.setBlockId(0, 64, 0, 1);
        CHECK(cp.getHighestYBlock() == 64);
    }

    TEST_CASE("highest of two blocks is returned") {
        ChunkPrimer cp;
        cp.setBlockId(0, 64,  0, 1);
        cp.setBlockId(5, 128, 5, 2);
        CHECK(cp.getHighestYBlock() == 128);
    }
}

// ============================================================
//  ChunkPrimer – getFootprintSize
// ============================================================
TEST_SUITE("ChunkPrimer::getFootprintSize") {

    TEST_CASE("footprint is at least the block storage size") {
        CHECK(ChunkPrimer::getFootprintSize() >= 16u * 256u * 16u * sizeof(u16));
    }
}

// ============================================================
//  WorldConfig – construction and getters
// ============================================================
TEST_SUITE("WorldConfig") {

    TEST_CASE("constructor stores seed and getters match") {
        WorldConfig cfg(123456789LL,
                        lce::CONSOLE::PS4,
                        LCEVERSION::AQUATIC,
                        lce::WORLDSIZE::LARGE,
                        lce::BIOMESCALE::MEDIUM);
        CHECK(cfg.getWorldSeed()    == 123456789LL);
        CHECK(cfg.getConsole()      == lce::CONSOLE::PS4);
        CHECK(cfg.getLCEVersion()   == LCEVERSION::AQUATIC);
        CHECK(cfg.getWorldSize()    == lce::WORLDSIZE::LARGE);
        CHECK(cfg.getBiomeScale()   == lce::BIOMESCALE::MEDIUM);
        CHECK(cfg.getWorldGenerator() == WORLDGENERATOR::DEFAULT);
    }

    TEST_CASE("setWorldSeed updates seed") {
        WorldConfig cfg(0LL, lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA,
                        lce::WORLDSIZE::MEDIUM, lce::BIOMESCALE::SMALL);
        cfg.setWorldSeed(-9999LL);
        CHECK(cfg.getWorldSeed() == -9999LL);
    }

    TEST_CASE("world coordinate bounds are positive and non-zero") {
        WorldConfig cfg(1LL, lce::CONSOLE::SWITCH, LCEVERSION::AQUATIC,
                        lce::WORLDSIZE::MEDIUM, lce::BIOMESCALE::MEDIUM);
        CHECK(cfg.getWorldCoordinateBounds() > 0);
    }

    TEST_CASE("chunk bounds equal coordinate bounds >> 4") {
        WorldConfig cfg(1LL, lce::CONSOLE::SWITCH, LCEVERSION::AQUATIC,
                        lce::WORLDSIZE::MEDIUM, lce::BIOMESCALE::MEDIUM);
        CHECK(cfg.getWorldChunkBounds() == cfg.getWorldCoordinateBounds() >> 4);
    }

    TEST_CASE("world bounds box reflects coordinate bounds") {
        WorldConfig cfg(1LL, lce::CONSOLE::SWITCH, LCEVERSION::AQUATIC,
                        lce::WORLDSIZE::MEDIUM, lce::BIOMESCALE::MEDIUM);
        const BoundingBox bb = cfg.getWorldBounds();
        const int b = cfg.getWorldCoordinateBounds();
        CHECK(bb.m_minX == -b);
        CHECK(bb.m_minZ == -b);
        CHECK(bb.m_maxX ==  b);
        CHECK(bb.m_maxZ ==  b);
    }

    TEST_CASE("setWorldSize updates bounds consistently") {
        WorldConfig cfg(1LL, lce::CONSOLE::PS3, LCEVERSION::AQUATIC,
                        lce::WORLDSIZE::SMALL, lce::BIOMESCALE::SMALL);
        cfg.setWorldSize(lce::WORLDSIZE::LARGE);
        CHECK(cfg.getWorldSize() == lce::WORLDSIZE::LARGE);
        CHECK(cfg.getWorldCoordinateBounds() > 0);
        CHECK(cfg.getWorldChunkBounds() == cfg.getWorldCoordinateBounds() >> 4);
    }

    TEST_CASE("setLCEVersion and setConsole round-trip") {
        WorldConfig cfg(0LL, lce::CONSOLE::NONE, LCEVERSION::NONE,
                        lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        cfg.setLCEVersion(LCEVERSION::BOUNTIFUL);
        cfg.setConsole(lce::CONSOLE::WIIU);
        CHECK(cfg.getLCEVersion() == LCEVERSION::BOUNTIFUL);
        CHECK(cfg.getConsole()    == lce::CONSOLE::WIIU);
    }

    TEST_CASE("fixed biome default is plains and can be changed") {
        WorldConfig cfg(0LL, lce::CONSOLE::NONE, LCEVERSION::NONE,
                        lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        CHECK(cfg.getFixedBiome() == biome_t::plains);
        cfg.setFixedBiome(biome_t::desert);
        CHECK(cfg.getFixedBiome() == biome_t::desert);
    }

    TEST_CASE("stronghold count default is 1 and can be changed") {
        WorldConfig cfg(0LL, lce::CONSOLE::NONE, LCEVERSION::NONE,
                        lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        CHECK(cfg.getStrongholdCount() == 1);
        cfg.setStrongholdCount(3);
        CHECK(cfg.getStrongholdCount() == 3);
    }

    TEST_CASE("setBiomeScale round-trip") {
        WorldConfig cfg(0LL, lce::CONSOLE::NONE, LCEVERSION::NONE,
                        lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
        cfg.setBiomeScale(lce::BIOMESCALE::LARGE);
        CHECK(cfg.getBiomeScale() == lce::BIOMESCALE::LARGE);
    }
}

// ============================================================
//  RNG::getChunkSeed / getPopulationSeed
// ============================================================
TEST_SUITE("RNG seed helpers") {

    TEST_CASE("getChunkSeed is deterministic") {
        const u64 s1 = RNG::getChunkSeed(123456LL, 4, -2);
        const u64 s2 = RNG::getChunkSeed(123456LL, 4, -2);
        CHECK(s1 == s2);
    }

    TEST_CASE("getChunkSeed changes with world seed") {
        CHECK(RNG::getChunkSeed(1LL,  0, 0) != RNG::getChunkSeed(2LL,  0, 0));
    }

    TEST_CASE("getChunkSeed changes with chunk coords") {
        const i64 w = 42LL;
        CHECK(RNG::getChunkSeed(w, 0, 0) != RNG::getChunkSeed(w, 1, 0));
        CHECK(RNG::getChunkSeed(w, 0, 0) != RNG::getChunkSeed(w, 0, 1));
    }

    TEST_CASE("getPopulationSeed(w,0,0) seed matches getChunkSeed(w,0,0)") {
        const i64 w = 999LL;
        const RNG pop = RNG::getPopulationSeed(w, 0, 0);
        // getPopulationSeed calls RNG(getChunkSeed(...)) which calls setSeed
        const u64 expected = (RNG::getChunkSeed(w, 0, 0) ^ 0x5DEECE66DULL) & ((1ULL << 48) - 1);
        CHECK(pop.getSeed() == expected);
    }

    TEST_CASE("getLargeFeatureSeed is deterministic") {
        const RNG r1 = RNG::getLargeFeatureSeed(777LL, 5, -3);
        const RNG r2 = RNG::getLargeFeatureSeed(777LL, 5, -3);
        CHECK(r1 == r2);
    }

    TEST_CASE("getLargeFeatureSeed differs across chunk coords") {
        CHECK_FALSE(RNG::getLargeFeatureSeed(1LL, 0, 0) == RNG::getLargeFeatureSeed(1LL, 1, 0));
    }
}

// ============================================================
//  LCG::combine
// ============================================================
TEST_SUITE("LCG::combine") {

    TEST_CASE("combine(0) is identity (mul=1, add=0)") {
        constexpr LCG base{0x5DEECE66DULL, 0xBULL};
        constexpr LCG id = base.combine(0);
        CHECK(id.multiplier == 1);
        CHECK(id.addend     == 0);
    }

    TEST_CASE("combine(1) equals base LCG") {
        constexpr LCG base{0x5DEECE66DULL, 0xBULL};
        constexpr LCG c1 = base.combine(1);
        CHECK(c1.multiplier == base.multiplier);
        CHECK(c1.addend     == base.addend);
    }

    TEST_CASE("JAVA_LCG::java2 matches combine(2)") {
        constexpr LCG c2 = JAVA_LCG::base.combine(2);
        CHECK(JAVA_LCG::M2 == c2.multiplier);
        CHECK(JAVA_LCG::A2 == c2.addend);
    }

    TEST_CASE("JAVA_LCG::java4 matches combine(4)") {
        constexpr LCG c4 = JAVA_LCG::base.combine(4);
        CHECK(JAVA_LCG::M4 == c4.multiplier);
        CHECK(JAVA_LCG::A4 == c4.addend);
    }
}

// ============================================================
//  PopulationReverser::getSeedsFromChunkSeed
// ============================================================
TEST_SUITE("PopulationReverser") {

    // Helper: verify every returned candidate actually reproduces the chunk seed
    static void verifyCandidates(u64 chunkSeed, int x, int z) {
        const auto results = PopulationReverser::getSeedsFromChunkSeed(chunkSeed, x, z);
        for (i64 ws : results) {
            const u64 got = RNG::getChunkSeed(ws, x, z) & MASK_48;
            CHECK(got == (chunkSeed & MASK_48));
        }
    }

    TEST_CASE("x=0, z=0 returns the chunk seed directly") {
        const u64 cs = 0xDEADBEEFULL;
        const auto res = PopulationReverser::getSeedsFromChunkSeed(cs, 0, 0);
        REQUIRE(res.size() == 1);
        CHECK(static_cast<u64>(res[0]) == cs);
    }

    TEST_CASE("all candidates reproduce the chunk seed at (1,0)") {
        verifyCandidates(RNG::getChunkSeed(12345LL, 1, 0), 1, 0);
    }

    TEST_CASE("all candidates reproduce the chunk seed at (0,1)") {
        verifyCandidates(RNG::getChunkSeed(12345LL, 0, 1), 0, 1);
    }

    TEST_CASE("all candidates reproduce the chunk seed at (4,-2)") {
        verifyCandidates(RNG::getChunkSeed(987654321LL, 4, -2), 4, -2);
    }

    TEST_CASE("known world seed is recovered at (1,1)") {
        constexpr i64 worldSeed = 42LL;
        const u64 cs = RNG::getChunkSeed(worldSeed, 1, 1);
        const auto res = PopulationReverser::getSeedsFromChunkSeed(cs, 1, 1);
        bool found = false;
        for (i64 ws : res) {
            if ((ws & MASK_48) == (worldSeed & MASK_48)) { found = true; break; }
        }
        CHECK(found);
    }

    TEST_CASE("result list is non-empty for typical coords") {
        const u64 cs = RNG::getChunkSeed(99999LL, 3, 5);
        const auto res = PopulationReverser::getSeedsFromChunkSeed(cs, 3, 5);
        CHECK_FALSE(res.empty());
    }
}

