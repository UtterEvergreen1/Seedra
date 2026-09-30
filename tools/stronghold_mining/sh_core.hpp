#pragma once
// World sampling for the stronghold mining tool.
//
// Every fast path in here mirrors a Seedra function exactly. `EXE_StrongholdMining selftest` checks each one
// against the library's own (uncached) implementation, and dataset mode re-checks the stronghold placement for
// every single world.

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/constants.hpp"
#include "common/rng.hpp"
#include "structures/gen/stronghold/stronghold.hpp"
#include "structures/placement/stronghold.hpp"
#include "terrain/BalancedSeed.hpp"
#include "terrain/generator.hpp"

namespace shm {

    // ------------------------------------------------------------------------------------------------ constants

    /// Classic worlds span blocks [-432, 431] on both axes.
    inline constexpr int WORLD_HALF = 432;

    /// The player appears uniformly within +-16 blocks (x and z independently) of the world spawn point.
    inline constexpr int PLAYER_SPAWN_RADIUS = 16;

    /// Observable biome grid around the player: offsets -128..128 blocks in steps of 32 (9 x 9 points).
    inline constexpr int GRID_STEP = 32;
    inline constexpr int GRID_HALF = 4;
    inline constexpr int GRID_SIDE = 2 * GRID_HALF + 1;
    inline constexpr int GRID_POINTS = GRID_SIDE * GRID_SIDE;

    /// Radius of the stronghold biome snap (Placement::Stronghold::getWorldPositions) and of the spawn search.
    inline constexpr int SNAP_RADIUS = 112;
    inline constexpr int SPAWN_SEARCH_RADIUS = 256;

    /// Copy of Generator::SPAWN_BIOMES (private there). Only used for the diagnostic `spawn_valid_cells` column;
    /// the spawn itself always comes from Generator::getSpawnBlock().
    inline constexpr u64 SPAWN_BIOMES =
            makeBiomeBitmask<biome_t::forest, biome_t::plains, biome_t::taiga, biome_t::taiga_hills,
                             biome_t::wooded_hills, biome_t::jungle, biome_t::jungle_hills>();

    // ------------------------------------------------------------------------------------------------- sampling

    inline u64 splitmix64(u64 x) {
        x += 0x9E3779B97F4A7C15ULL;
        x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
        x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
        return x ^ (x >> 31);
    }

    inline u64 hash2(const u64 a, const u64 b) { return splitmix64(splitmix64(a) ^ b); }

    /// World seed of sample `sample`, created the way the game creates a random seed: a java.util.Random seeded
    /// from the performance counter (any 48-bit state is possible, so a hash stands in for the counter), then
    /// nextLong(). "Find Balanced Seed" keeps calling nextLong() until a seed passes the balance test, so keeping
    /// only the samples with balanced == 1 gives exactly the distribution of balanced worlds.
    inline i64 sampleWorldSeed(const u64 runSeed, const u64 sample) {
        RNG rng;
        rng.setSeed(hash2(runSeed, sample));
        return rng.nextLongI();
    }

    /// Where the player appears relative to the world spawn: uniform in [-16, 16] on each axis. In game this comes
    /// from the player's own RNG, which has nothing to do with world generation, so a hash is fine.
    inline Pos2D samplePlayerOffset(const u64 runSeed, const u64 sample) {
        const u64 h = hash2(runSeed ^ 0x504C41594552ULL, sample);
        constexpr u64 SPAN = 2 * PLAYER_SPAWN_RADIUS + 1;
        return {static_cast<int>(h % SPAN) - PLAYER_SPAWN_RADIUS,
                static_cast<int>((h >> 32) % SPAN) - PLAYER_SPAWN_RADIUS};
    }

    // ---------------------------------------------------------------------------------------------- biome view

    /// Bounds-checked view of the generator's 1:4 world cache (Generator::generateCache(4)).
    /// Generator::getCacheAtBlock accepts the coordinate 108 on classic worlds and then reads one cell past the
    /// row (or the buffer), so this view is used for all direct lookups. Re-create it after every
    /// applyWorldSeed(): the cache buffer is reallocated for each seed.
    class Biomes4 {
        const biome_t* m_data = nullptr;
        int m_half = 0; ///< the cache covers 1:4 cells [-m_half, m_half) on both axes

    public:
        Biomes4() = default;

        explicit Biomes4(const Generator& g) : m_data(g.getWorldBiomes(4)), m_half(g.getWorldCoordinateBounds() >> 2) {
            if (m_data == nullptr) throw std::runtime_error("Biomes4: call generateCache(4) first");
        }

        /// Biome at 1:4 cell (x4, z4), or biome_t::none outside the world.
        ND biome_t at(c_int x4, c_int z4) const {
            c_int side = 2 * m_half;
            c_int ix = x4 + m_half;
            c_int iz = z4 + m_half;
            if (ix < 0 || iz < 0 || ix >= side || iz >= side) return biome_t::none;
            return m_data[static_cast<size_t>(iz) * static_cast<size_t>(side) + static_cast<size_t>(ix)];
        }

        /// Biome of the 1:4 cell containing block (x, z).
        ND biome_t atBlock(c_int x, c_int z) const { return at(x >> 2, z >> 2); }
    };

    /// Number of cells that locateBiome(x, z, radius, mask) treats as valid (same square as the library scan).
    inline int countValidCells(const Biomes4& b, c_int x, c_int z, c_int radius, const u64 mask) {
        c_int x1 = (x - radius) >> 2;
        c_int z1 = (z - radius) >> 2;
        c_int x2 = (x + radius) >> 2;
        c_int z2 = (z + radius) >> 2;
        int n = 0;
        for (int zz = z1; zz <= z2; ++zz)
            for (int xx = x1; xx <= x2; ++xx)
                if (Generator::id_matches(b.at(xx, zz), mask)) ++n;
        return n;
    }

    // ---------------------------------------------------------------------------------------- stronghold placement

    struct PlacementTrace {
        double angle = 0.0;    ///< radians, of the attempt that produced the result
        int multiplier = 0;    ///< 3..6 on classic worlds
        double distance = 0.0; ///< chunks: (nextDouble() + 1.25) * multiplier
        Pos2D target{};        ///< block position handed to locateBiome, i.e. before the biome snap
        int attempts = 0;      ///< 1 = the first attempt found a valid biome
        int found = 0;         ///< locateBiome "passes" of the last attempt (0 = no valid biome in the square)
        Pos2D result{};        ///< final position, identical to Placement::Stronghold::getWorldPositions(g)[0]
    };

    /// Line-by-line copy of Placement::Stronghold::getWorldPositions() for one stronghold, except that the caller
    /// supplies the RNG. Seeded with the world seed it reproduces the library exactly (checked for every world in
    /// dataset mode); seeded with anything else it answers "where could the stronghold be on this biome map?".
    inline Pos2D placeStronghold(const Generator& g, RNG& rng, PlacementTrace* trace) {
        Pos2D p{};
        Pos2D result{};
        int out = 0;
        int multiplier = 0;
        c_bool xboxStronghold = lce::isXbox(g.getConsole());
        const int maxAttempts = g.getConsole() == lce::CONSOLE::XBOX1 ? 30 : 10;

        double angle = static_cast<double>(TWO_PI_FLOAT) * rng.nextDouble();

        for (int attempts = 0; attempts < maxAttempts && !out; ++attempts) {
            if (Placement::Stronghold::useFarStronghold) multiplier = 32;
            else if (xboxStronghold)
                multiplier = rng.nextInt(4) + 3;
            double dist = rng.nextDouble() + 1.25;
            if (!xboxStronghold && !Placement::Stronghold::useFarStronghold) multiplier = rng.nextInt(4) + 3;

            dist = dist * static_cast<double>(multiplier);

            p.x = (static_cast<int>(std::roundf(cos(angle) * dist)) << 4) + 8;
            p.z = (static_cast<int>(std::roundf(sin(angle) * dist)) << 4) + 8;

            if (trace != nullptr) {
                trace->angle = angle;
                trace->multiplier = multiplier;
                trace->distance = dist;
                trace->target = p;
                trace->attempts = attempts + 1;
            }

            p = g.locateBiome(p.x, p.z, SNAP_RADIUS, Placement::Stronghold::stronghold_biomes, rng, &out);
            if (out || attempts == maxAttempts - 1) result = p.toChunkPos().toBlockPos() + 4;
            angle += static_cast<double>(TWO_PI_FLOAT) / g.getStrongholdCount();

            if (g.getConsole() == lce::CONSOLE::XBOX1) {
                if (g.getWorldGenerator() == WORLDGENERATOR::DEFAULT)
                    angle = static_cast<double>(TWO_THIRDS_PI_FLOAT) * rng.nextDouble();
                else
                    angle = static_cast<double>(TWO_PI_FLOAT) * rng.nextDouble();
            }
        }

        if (trace != nullptr) {
            trace->found = out;
            trace->result = result;
        }
        return result;
    }

    // --------------------------------------------------------------------------------------------- balanced seed

    /// The game's "Find Balanced Seed" test: BalancedSeed::getIsMatch on the 1:4 biomes of blocks [-400, 400),
    /// always generated at SMALL biome scale. This variant reads a SMALL-scale world cache.
    inline bool isBalancedFromCache(const Biomes4& smallScaleBiomes) {
        constexpr int SIDE = 200;
        thread_local std::vector<biome_t> buffer(static_cast<size_t>(SIDE) * SIDE);
        for (int z = 0; z < SIDE; ++z)
            for (int x = 0; x < SIDE; ++x)
                buffer[static_cast<size_t>(z) * SIDE + static_cast<size_t>(x)] =
                        smallScaleBiomes.at(x - SIDE / 2, z - SIDE / 2);
        return BalancedSeed::getIsMatch(buffer.data());
    }

    /// Same test, generating the biomes like BalancedSeed::findBalancedSeed does (for non-small worlds).
    inline bool isBalancedFromGenerator(const Generator& smallScaleGenerator) {
        biome_t* ids = smallScaleGenerator.getBiomeRange(4, -100, -100, 200, 200);
        c_bool match = BalancedSeed::getIsMatch(ids);
        free(ids);
        return match;
    }

    /// Cheap necessary condition for the balanced test on a SMALL-scale generator. The test needs mushroom
    /// biomes, and on SMALL scale every 1:4 mushroom cell descends from a 1:16 mushroom cell (the layers after
    /// the 1:16 shore layer only zoom, smooth and add rivers). So no mushroom at 1:16 over the test area plus a
    /// 3 cell margin means the full test fails. `selftest` verifies there are no false rejections.
    inline bool mayBeBalanced(const Generator& smallScaleGenerator) {
        constexpr int LO = -28, SIDE = 56; // 1:16 cells covering 1:4 cells [-100, 100) with margin
        biome_t* ids = smallScaleGenerator.getBiomeRange(16, LO, LO, SIDE, SIDE);
        bool any = false;
        for (int i = 0; i < SIDE * SIDE && !any; ++i)
            any = ids[i] == biome_t::mushroom_island || ids[i] == biome_t::mushroom_island_shore;
        free(ids);
        return any;
    }

    inline bool isBalancedFast(const Generator& smallScaleGenerator) {
        return mayBeBalanced(smallScaleGenerator) && isBalancedFromGenerator(smallScaleGenerator);
    }

    // ------------------------------------------------------------------------------------------------ one world

    struct Settings {
        lce::CONSOLE console = lce::CONSOLE::XBOX1;
        LCEVERSION version = LCEVERSION::ELYTRA;
        lce::BIOMESCALE scale = lce::BIOMESCALE::SMALL;
        u64 runSeed = 1;
        bool balancedCheck = true;
        /// Emulate "Find Balanced Seed": keep calling nextLong() on the sample's Random until a seed passes.
        bool balancedOnly = false;
    };

    struct WorldSample {
        u64 sample = 0;
        i64 seed = 0;
        int balanced = -1;                  ///< 1 / 0, or -1 when the check was skipped
        u32 candidates = 1;                 ///< nextLong() calls needed (> 1 only with balancedOnly)
        Pos2D spawn{};                      ///< world spawn point (Generator::getSpawnBlock)
        Pos2D player{};                     ///< where the player appears: spawn + uniform [-16, 16] per axis
        biome_t spawnBiome = biome_t::none; ///< 1:4 biome at the world spawn
        biome_t playerBiome = biome_t::none;
        Pos2D stronghold{}; ///< final placement, block position (chunk << 4) + 4
        PlacementTrace trace{};
        int spawnValidCells = 0; ///< spawn-biome cells in the 129 x 129 spawn search square
        int snapValidCells = 0;  ///< stronghold-biome cells in the 57 x 57 snap square of the last attempt
        Pos2D portal{};          ///< portal room centre (x, z)
        int portalY = 0;         ///< portal room floor (min y)
        int topY = 0;            ///< stronghold bounding box max y
        int bottomY = 0;         ///< stronghold bounding box min y
        int pieces = 0;
        int footprint = 0; ///< distinct x/z columns covered by any piece (filled by the coverage accumulator)
        /// biome at player + (dx, dz); row index = dz (north -> south), column index = dx (west -> east)
        std::array<biome_t, GRID_POINTS> grid{};
    };

    /// Owns one generator (+ stronghold object) per thread and turns a sample index into a WorldSample.
    class WorldSampler {
        Settings m_settings;
        Generator m_g;
        std::unique_ptr<Generator> m_small; ///< SMALL-scale generator for the balanced test of non-small worlds
        std::unique_ptr<gen::Stronghold> m_sh;
        Biomes4 m_biomes;

    public:
        explicit WorldSampler(const Settings& settings)
            : m_settings(settings), m_g(settings.console, settings.version, lce::WORLDSIZE::CLASSIC, settings.scale),
              m_sh(std::make_unique<gen::Stronghold>()) {
            if (m_g.getStrongholdCount() != 1) throw std::runtime_error("expected exactly one stronghold");
            m_g.generateCache(4);
            m_biomes = Biomes4(m_g);
            if (settings.balancedOnly || (settings.balancedCheck && settings.scale != lce::BIOMESCALE::SMALL))
                m_small = std::make_unique<Generator>(settings.console, settings.version, lce::WORLDSIZE::CLASSIC,
                                                      lce::BIOMESCALE::SMALL);
            m_sh->m_generationStep = GS_Stronghold_Full; // full layout including the final y offset
        }

        /// World seed for a sample index: the first nextLong() of the sample's Random, or with balancedOnly the
        /// first of its nextLong() values that passes the balanced test (like the game's "Find Balanced Seed").
        i64 seedFor(const u64 sampleIndex, u32* candidates = nullptr) {
            RNG rng;
            rng.setSeed(hash2(m_settings.runSeed, sampleIndex));
            i64 seed = rng.nextLongI();
            u32 n = 1;
            if (m_settings.balancedOnly) {
                for (;;) {
                    m_small->applyWorldSeed(seed);
                    if (isBalancedFast(*m_small)) break;
                    seed = rng.nextLongI();
                    ++n;
                }
            }
            if (candidates != nullptr) *candidates = n;
            return seed;
        }

        ND const Generator& generator() const { return m_g; }
        ND const Biomes4& biomes() const { return m_biomes; }
        ND const gen::Stronghold& stronghold() const { return *m_sh; }

        /// Switch to `seed`; regenerates the 1:4 biome cache.
        void loadSeed(const i64 seed) {
            m_g.applyWorldSeed(seed);
            m_biomes = Biomes4(m_g);
        }

        ND bool isBalanced(const i64 seed) {
            if (m_settings.scale == lce::BIOMESCALE::SMALL) return isBalancedFromCache(m_biomes);
            m_small->applyWorldSeed(seed);
            return isBalancedFast(*m_small);
        }

        /// Generate the stronghold layout for the loaded world at `strongholdPos` (a getWorldPositions result).
        void generateLayout(const i64 seed, const Pos2D strongholdPos) {
            m_sh->generate(seed, strongholdPos.toChunkPos());
        }

        /// Everything for one world: placement (checked against the library), layout, spawn and the observable grid.
        WorldSample sample(const u64 sampleIndex) {
            WorldSample w;
            w.sample = sampleIndex;
            w.seed = seedFor(sampleIndex, &w.candidates);
            loadSeed(w.seed);

            if (m_settings.balancedOnly) w.balanced = 1;
            else if (m_settings.balancedCheck)
                w.balanced = isBalanced(w.seed) ? 1 : 0;

            RNG rng;
            rng.setSeed(static_cast<u64>(w.seed));
            w.stronghold = placeStronghold(m_g, rng, &w.trace);
            const Pos2D library = Placement::Stronghold::getWorldPositions(m_g)[0];
            if (!(library == w.stronghold))
                throw std::runtime_error("stronghold placement copy diverged from the library for seed " +
                                         std::to_string(w.seed) + " -- update placeStronghold() in sh_core.hpp");
            w.snapValidCells = countValidCells(m_biomes, w.trace.target.x, w.trace.target.z, SNAP_RADIUS,
                                               Placement::Stronghold::stronghold_biomes);

            generateLayout(w.seed, w.stronghold);
            const gen::Stronghold& sh = *m_sh;
            if (sh.m_portalRoomPiece == nullptr) throw std::runtime_error("stronghold without portal room");
            const StructureComponent& portal = *sh.m_portalRoomPiece;
            w.portal = {(portal.m_minX + portal.m_maxX) >> 1, (portal.m_minZ + portal.m_maxZ) >> 1};
            w.portalY = portal.m_minY;
            w.topY = sh.getStructureBB().m_maxY;
            w.bottomY = sh.getStructureBB().m_minY;
            w.pieces = sh.getPieceCount();

            w.spawn = m_g.getSpawnBlock();
            w.spawnValidCells = countValidCells(m_biomes, 0, 0, SPAWN_SEARCH_RADIUS, SPAWN_BIOMES);
            w.player = w.spawn + samplePlayerOffset(m_settings.runSeed, sampleIndex);
            w.spawnBiome = m_biomes.atBlock(w.spawn.x, w.spawn.z);
            w.playerBiome = m_biomes.atBlock(w.player.x, w.player.z);

            for (int gz = 0; gz < GRID_SIDE; ++gz) {
                for (int gx = 0; gx < GRID_SIDE; ++gx) {
                    c_int x = w.player.x + (gx - GRID_HALF) * GRID_STEP;
                    c_int z = w.player.z + (gz - GRID_HALF) * GRID_STEP;
                    w.grid[static_cast<size_t>(gz * GRID_SIDE + gx)] = m_biomes.atBlock(x, z);
                }
            }
            return w;
        }
    };

} // namespace shm
