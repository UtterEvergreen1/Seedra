#pragma once
// Stronghold footprint rasterisation, exact distance transforms and the per-thread accumulators behind the
// kernel and heat-map outputs.
//
// "Kernel" = P(something is within R blocks of the column at offset (dx, dz) from the stronghold chunk origin),
// over random worlds. The stronghold layout only depends on (world seed, chunk), never on biomes, so any
// conditional heat map is (start-chunk histogram) convolved with a kernel. The analysis relies on that and checks
// it against the directly accumulated heat maps.

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <mutex>
#include <vector>

#include "sh_core.hpp"

namespace shm {

    /// Radii (blocks) at which the kernels are tabulated. 0 = the dug column itself touches the structure.
    inline constexpr std::array<int, 13> KERNEL_RADII = {0, 4, 8, 12, 16, 20, 24, 30, 40, 50, 64, 80, 100};
    inline constexpr int NUM_RADII = static_cast<int>(KERNEL_RADII.size());
    inline constexpr int MAX_RADIUS = KERNEL_RADII.back();

    /// Kernel window: offsets [-200, 200] from the stronghold chunk origin (chunkX * 16, chunkZ * 16).
    inline constexpr int KERNEL_HALF = 200;
    inline constexpr int KERNEL_SIDE = 2 * KERNEL_HALF + 1;
    inline constexpr size_t KERNEL_CELLS = static_cast<size_t>(KERNEL_SIDE) * KERNEL_SIDE;

    /// Kernels: 0 = horizontal distance to any piece, 1 = 3D distance from an eye at eyeY to any piece,
    /// 2 = horizontal distance to the portal room, 3..6 = directional look north/east/south/west: depth (blocks
    /// along the facing direction) to the nearest piece inside a 90 degree wedge (+-45 degrees) around it.
    enum KernelKind : int {
        K_ANY_H = 0,
        K_ANY_3D = 1,
        K_PORTAL_H = 2,
        K_DIR_N = 3,
        K_DIR_E = 4,
        K_DIR_S = 5,
        K_DIR_W = 6,
        K_COUNT = 7
    };
    inline constexpr std::array<const char*, K_COUNT> KERNEL_NAMES = {"any_h", "any_3d", "portal_h", "dir_n",
                                                                      "dir_e", "dir_s",  "dir_w"};
    inline constexpr bool isDirectional(const int kind) { return kind >= K_DIR_N; }

    /// Classic world grid for the directly accumulated heat maps: blocks [-432, 431].
    inline constexpr int WORLD_SIDE = 2 * WORLD_HALF;
    inline constexpr size_t WORLD_CELLS = static_cast<size_t>(WORLD_SIDE) * WORLD_SIDE;

    // ------------------------------------------------------------------------------------ distance transform

    inline constexpr double DT_INF = 1e30;

    /// Lower envelope of parabolas (Felzenszwalb & Huttenlocher): out[q] = min_i (q - pos[i])^2 + val[i] for q in
    /// [qLo, qHi]. `pos` must be strictly increasing. Scratch buffers need m + 1 entries.
    inline void lowerEnvelope(const int* pos, const double* val, const int m, const int qLo, const int qHi, double* out,
                              std::vector<int>& v, std::vector<double>& zb) {
        if (m == 0) {
            for (int q = qLo; q <= qHi; ++q) out[q - qLo] = DT_INF;
            return;
        }
        v.resize(static_cast<size_t>(m) + 1);
        zb.resize(static_cast<size_t>(m) + 2);
        int k = 0;
        v[0] = 0;
        zb[0] = -DT_INF;
        zb[1] = DT_INF;
        for (int i = 1; i < m; ++i) {
            const double fi = val[i] + static_cast<double>(pos[i]) * pos[i];
            double s;
            for (;;) {
                const int j = v[static_cast<size_t>(k)];
                s = (fi - (val[j] + static_cast<double>(pos[j]) * pos[j])) / (2.0 * (pos[i] - pos[j]));
                if (s <= zb[static_cast<size_t>(k)]) --k; // zb[0] = -inf, so k never drops below 0
                else
                    break;
            }
            ++k;
            v[static_cast<size_t>(k)] = i;
            zb[static_cast<size_t>(k)] = s;
            zb[static_cast<size_t>(k) + 1] = DT_INF;
        }
        int j = 0;
        for (int q = qLo; q <= qHi; ++q) {
            while (zb[static_cast<size_t>(j) + 1] < q) ++j;
            const int p = pos[v[static_cast<size_t>(j)]];
            out[q - qLo] = static_cast<double>(q - p) * (q - p) + val[v[static_cast<size_t>(j)]];
        }
    }

    /// Exact 2D transform D(x, z) = min over marked cells c of |(x, z) - c|^2 + f(c) on a square grid, evaluated
    /// on [lo, hi]^2. `f` holds DT_INF for unmarked cells; only rows/cols [mLo, mHi] may contain marked cells.
    class DistanceTransform {
        std::vector<double> m_rowPass; ///< (hi - lo + 1) columns x (mHi - mLo + 1) rows
        std::vector<int> m_pos, m_v;
        std::vector<double> m_val, m_zb, m_col;

    public:
        /// Result is written to `out` (side x side, row-major), only inside [lo, hi]^2.
        void run(const std::vector<double>& f, const int side, const int mLo, const int mHi, const int lo, const int hi,
                 std::vector<double>& out) {
            const int nEval = hi - lo + 1;
            const int nRows = mHi - mLo + 1;
            m_rowPass.assign(static_cast<size_t>(nEval) * static_cast<size_t>(nRows), DT_INF);
            m_pos.resize(static_cast<size_t>(std::max(side, 1)));
            m_val.resize(static_cast<size_t>(std::max(side, 1)));
            m_col.resize(static_cast<size_t>(nEval));

            // pass 1: along x, one row at a time
            for (int z = mLo; z <= mHi; ++z) {
                int m = 0;
                const double* row = &f[static_cast<size_t>(z) * static_cast<size_t>(side)];
                for (int x = mLo; x <= mHi; ++x) {
                    if (row[x] >= DT_INF) continue;
                    m_pos[static_cast<size_t>(m)] = x;
                    m_val[static_cast<size_t>(m)] = row[x];
                    ++m;
                }
                if (m == 0) continue;
                lowerEnvelope(m_pos.data(), m_val.data(), m, lo, hi, m_col.data(), m_v, m_zb);
                for (int i = 0; i < nEval; ++i)
                    m_rowPass[static_cast<size_t>(z - mLo) * static_cast<size_t>(nEval) + static_cast<size_t>(i)] =
                            m_col[static_cast<size_t>(i)];
            }

            // pass 2: along z, one column at a time
            for (int i = 0; i < nEval; ++i) {
                int m = 0;
                for (int z = mLo; z <= mHi; ++z) {
                    const double g = m_rowPass[static_cast<size_t>(z - mLo) * static_cast<size_t>(nEval) +
                                               static_cast<size_t>(i)];
                    if (g >= DT_INF) continue;
                    m_pos[static_cast<size_t>(m)] = z;
                    m_val[static_cast<size_t>(m)] = g;
                    ++m;
                }
                lowerEnvelope(m_pos.data(), m_val.data(), m, lo, hi, m_col.data(), m_v, m_zb);
                const int x = lo + i;
                for (int z = lo; z <= hi; ++z)
                    out[static_cast<size_t>(z) * static_cast<size_t>(side) + static_cast<size_t>(x)] =
                            m_col[static_cast<size_t>(z - lo)];
            }
        }
    };

    // ------------------------------------------------------------------------------------------ direct distances

    /// Squared distance from the column (x, z) to a piece: horizontal part, plus the vertical gap to an eye at
    /// eyeY when `use3d` is set (pieces are always below the eye; the gap is clamped at 0).
    inline long long squaredDistanceToPiece(const BoundingBox& bb, c_int x, c_int z, c_bool use3d, c_int eyeY) {
        const long long dx = x < bb.m_minX ? bb.m_minX - x : (x > bb.m_maxX ? x - bb.m_maxX : 0);
        const long long dz = z < bb.m_minZ ? bb.m_minZ - z : (z > bb.m_maxZ ? z - bb.m_maxZ : 0);
        long long d2 = dx * dx + dz * dz;
        if (use3d) {
            const long long dy = std::max(0, eyeY - static_cast<int>(bb.m_maxY));
            d2 += dy * dy;
        }
        return d2;
    }

    /// Depth of the nearest column of `bb` inside the wedge seen from (x, z) facing `kind` (K_DIR_*): depth k =
    /// blocks along the facing direction, the wedge holds the columns with |sideways offset| <= k. Brute force.
    inline long long directionalDepthToPiece(const BoundingBox& bb, const int kind, c_int x, c_int z) {
        long long best = std::numeric_limits<long long>::max();
        for (int cz = bb.m_minZ; cz <= bb.m_maxZ; ++cz) {
            for (int cx = bb.m_minX; cx <= bb.m_maxX; ++cx) {
                int depth = 0, side = 0;
                switch (kind) {
                    case K_DIR_N:
                        depth = z - cz, side = cx - x;
                        break;
                    case K_DIR_S:
                        depth = cz - z, side = cx - x;
                        break;
                    case K_DIR_E:
                        depth = cx - x, side = cz - z;
                        break;
                    default:
                        depth = x - cx, side = cz - z;
                        break;
                }
                if (depth >= 0 && std::abs(side) <= depth) best = std::min<long long>(best, depth);
            }
        }
        return best;
    }

    /// Distance from column (x, z) to the stronghold for the given kernel kind (brute force; used for the oracle's
    /// "did it really hit" check and for validating the transforms). Squared for the distance kinds, plain depth for
    /// the directional kinds; compare with R^2 or R respectively (see withinRadius).
    inline long long distanceToStronghold(const gen::Stronghold& sh, const KernelKind kind, c_int x, c_int z,
                                          c_int eyeY) {
        if (kind == K_PORTAL_H) return squaredDistanceToPiece(*sh.m_portalRoomPiece, x, z, false, eyeY);
        long long best = std::numeric_limits<long long>::max();
        for (int i = 0; i < sh.getPieceCount(); ++i) {
            const BoundingBox& bb = sh.getPieceConst(i);
            best = std::min(best, isDirectional(kind) ? directionalDepthToPiece(bb, kind, x, z)
                                                      : squaredDistanceToPiece(bb, x, z, kind == K_ANY_3D, eyeY));
        }
        return best;
    }

    inline bool withinRadius(const KernelKind kind, const long long distance, c_int radius) {
        return isDirectional(kind) ? distance <= radius : distance <= static_cast<long long>(radius) * radius;
    }

    // --------------------------------------------------------------------------------------------- kernels

    /// Kernel counts shared by all threads: counts[kind][bin][cell], bin r = distance in (R[r-1], R[r]].
    struct KernelTotals {
        std::mutex mutex;
        std::vector<u32> hist = std::vector<u32>(static_cast<size_t>(K_COUNT) * NUM_RADII * KERNEL_CELLS, 0);
        u64 worlds = 0;
        u64 piecesOutsideWindow = 0; ///< must stay 0: pieces further than 100 blocks from the chunk origin

        /// Cumulative counts [kind][radius][KERNEL_SIDE][KERNEL_SIDE]: worlds with distance <= KERNEL_RADII[r].
        ND std::vector<u32> cumulative() const {
            std::vector<u32> c(hist.size());
            for (int kind = 0; kind < K_COUNT; ++kind) {
                for (size_t cell = 0; cell < KERNEL_CELLS; ++cell) {
                    u32 run = 0;
                    for (int r = 0; r < NUM_RADII; ++r) {
                        const size_t i =
                                (static_cast<size_t>(kind) * NUM_RADII + static_cast<size_t>(r)) * KERNEL_CELLS + cell;
                        run += hist[i];
                        c[i] = run;
                    }
                }
            }
            return c;
        }
    };

    /// Per-thread kernel accumulator: 16-bit counts, merged into the shared KernelTotals every 60000 worlds.
    class KernelAccumulator {
        static constexpr u32 FLUSH_EVERY = 60000; // < 65535, and each world adds at most 1 per kind and cell

        int m_eyeY;
        KernelTotals* m_totals;
        std::vector<uint16_t> m_hist;
        u32 m_pending = 0;
        u64 m_outside = 0;
        std::array<int, MAX_RADIUS * MAX_RADIUS + 1> m_binOfSquared{};
        std::array<int, MAX_RADIUS + 1> m_binOfDepth{};
        std::vector<double> m_f, m_out;
        DistanceTransform m_dt;

    public:
        KernelAccumulator(c_int eyeY, KernelTotals* totals)
            : m_eyeY(eyeY), m_totals(totals), m_hist(static_cast<size_t>(K_COUNT) * NUM_RADII * KERNEL_CELLS, 0),
              m_f(KERNEL_CELLS, DT_INF), m_out(KERNEL_CELLS, DT_INF) {
            int r = 0;
            for (int d2 = 0; d2 <= MAX_RADIUS * MAX_RADIUS; ++d2) {
                while (KERNEL_RADII[static_cast<size_t>(r)] * KERNEL_RADII[static_cast<size_t>(r)] < d2) ++r;
                m_binOfSquared[static_cast<size_t>(d2)] = r;
            }
            r = 0;
            for (int d = 0; d <= MAX_RADIUS; ++d) {
                while (KERNEL_RADII[static_cast<size_t>(r)] < d) ++r;
                m_binOfDepth[static_cast<size_t>(d)] = r;
            }
        }

        KernelAccumulator(const KernelAccumulator&) = delete;
        KernelAccumulator& operator=(const KernelAccumulator&) = delete;
        ~KernelAccumulator() { flush(); }

        void flush() {
            if (m_pending == 0 || m_totals == nullptr) return;
            std::lock_guard lock(m_totals->mutex);
            for (size_t i = 0; i < m_hist.size(); ++i) m_totals->hist[i] += m_hist[i];
            m_totals->worlds += m_pending;
            m_totals->piecesOutsideWindow += m_outside;
            std::fill(m_hist.begin(), m_hist.end(), uint16_t{0});
            m_pending = 0;
            m_outside = 0;
        }

        /// Distances for one kernel kind into distances() (window cells, row-major), valid inside the returned
        /// [lo, hi]^2 square: squared distance for the distance kinds, depth for the directional kinds (DT_INF =
        /// nothing in the wedge). Returns false if nothing was marked.
        bool transform(const gen::Stronghold& sh, const Pos2D chunk, const KernelKind kind, int& lo, int& hi) {
            c_int ox = chunk.x * 16 - KERNEL_HALF; // block x of window cell 0
            c_int oz = chunk.z * 16 - KERNEL_HALF;
            c_int limitLo = MAX_RADIUS;                   // pieces must stay within +-(KERNEL_HALF - MAX_RADIUS)
            c_int limitHi = KERNEL_SIDE - 1 - MAX_RADIUS; // of the chunk origin so every radius fits the window

            std::fill(m_f.begin(), m_f.end(), DT_INF);
            int mLo = KERNEL_SIDE, mHi = -1;
            auto mark = [&](const BoundingBox& bb) {
                int x0 = bb.m_minX - ox, x1 = bb.m_maxX - ox, z0 = bb.m_minZ - oz, z1 = bb.m_maxZ - oz;
                if (kind == K_ANY_H && (x0 < limitLo || z0 < limitLo || x1 > limitHi || z1 > limitHi)) ++m_outside;
                x0 = std::max(x0, 0);
                z0 = std::max(z0, 0);
                x1 = std::min(x1, KERNEL_SIDE - 1);
                z1 = std::min(z1, KERNEL_SIDE - 1);
                if (x0 > x1 || z0 > z1) return;
                double value = 0.0;
                if (kind == K_ANY_3D) {
                    const double dy = std::max(0, m_eyeY - static_cast<int>(bb.m_maxY));
                    value = dy * dy;
                }
                for (int z = z0; z <= z1; ++z) {
                    double* row = &m_f[static_cast<size_t>(z) * KERNEL_SIDE];
                    for (int x = x0; x <= x1; ++x) row[x] = std::min(row[x], value);
                }
                mLo = std::min({mLo, x0, z0});
                mHi = std::max({mHi, x1, z1});
            };
            if (kind == K_PORTAL_H) mark(*sh.m_portalRoomPiece);
            else
                for (int i = 0; i < sh.getPieceCount(); ++i) mark(sh.getPieceConst(i));
            if (mHi < 0) return false;

            lo = std::max(0, mLo - MAX_RADIUS);
            hi = std::min(KERNEL_SIDE - 1, mHi + MAX_RADIUS);
            if (isDirectional(kind)) directionalDepth(kind, lo, hi);
            else
                m_dt.run(m_f, KERNEL_SIDE, mLo, mHi, lo, hi, m_out);
            return true;
        }

        ND const std::vector<double>& distances() const { return m_out; }

        void add(const gen::Stronghold& sh, const Pos2D chunk) {
            for (int kind = 0; kind < K_COUNT; ++kind) {
                int lo = 0, hi = -1;
                if (!transform(sh, chunk, static_cast<KernelKind>(kind), lo, hi)) continue;

                uint16_t* hist = &m_hist[static_cast<size_t>(kind) * NUM_RADII * KERNEL_CELLS];
                const bool depth = isDirectional(kind);
                const double limit = depth ? MAX_RADIUS + 0.5 : MAX_RADIUS * MAX_RADIUS + 0.5;
                for (int z = lo; z <= hi; ++z) {
                    for (int x = lo; x <= hi; ++x) {
                        const size_t cell = static_cast<size_t>(z) * KERNEL_SIDE + static_cast<size_t>(x);
                        const double d = m_out[cell];
                        if (d > limit) continue;
                        const auto id = static_cast<size_t>(d + 0.5);
                        const int bin = depth ? m_binOfDepth[id] : m_binOfSquared[id];
                        ++hist[static_cast<size_t>(bin) * KERNEL_CELLS + cell];
                    }
                }
            }
            if (++m_pending == FLUSH_EVERY) flush();
        }

    private:
        /// Depth to the nearest marked cell in the 90 degree wedge facing `kind`, by dynamic programming:
        /// D(cell) = 0 if marked, else 1 + min of the three cells one step further in the facing direction
        /// (the wedge of a cell is itself plus the wedges of those three). Cells outside [lo, hi]^2 hold nothing.
        void directionalDepth(const KernelKind kind, c_int lo, c_int hi) {
            auto at = [&](c_int x, c_int z) -> double {
                if (x < lo || x > hi || z < lo || z > hi) return DT_INF;
                return m_out[static_cast<size_t>(z) * KERNEL_SIDE + static_cast<size_t>(x)];
            };
            auto set = [&](c_int x, c_int z) {
                const size_t cell = static_cast<size_t>(z) * KERNEL_SIDE + static_cast<size_t>(x);
                if (m_f[cell] < DT_INF) {
                    m_out[cell] = 0.0;
                    return;
                }
                double best = DT_INF;
                switch (kind) {
                    case K_DIR_S:
                        best = std::min({at(x - 1, z + 1), at(x, z + 1), at(x + 1, z + 1)});
                        break;
                    case K_DIR_N:
                        best = std::min({at(x - 1, z - 1), at(x, z - 1), at(x + 1, z - 1)});
                        break;
                    case K_DIR_E:
                        best = std::min({at(x + 1, z - 1), at(x + 1, z), at(x + 1, z + 1)});
                        break;
                    default:
                        best = std::min({at(x - 1, z - 1), at(x - 1, z), at(x - 1, z + 1)});
                        break;
                }
                m_out[cell] = best >= DT_INF ? DT_INF : best + 1.0;
            };
            // sweep against the facing direction so the three "further" cells are always done first
            switch (kind) {
                case K_DIR_S:
                    for (int z = hi; z >= lo; --z)
                        for (int x = lo; x <= hi; ++x) set(x, z);
                    break;
                case K_DIR_N:
                    for (int z = lo; z <= hi; ++z)
                        for (int x = lo; x <= hi; ++x) set(x, z);
                    break;
                case K_DIR_E:
                    for (int x = hi; x >= lo; --x)
                        for (int z = lo; z <= hi; ++z) set(x, z);
                    break;
                default:
                    for (int x = lo; x <= hi; ++x)
                        for (int z = lo; z <= hi; ++z) set(x, z);
                    break;
            }
        }
    };

    // ------------------------------------------------------------------------------------- direct heat maps

    /// Per-thread world-grid heat maps, accumulated directly from each world's real layout (no kernel).
    struct CoverageAccumulator {
        std::vector<u32> any, anyBalanced, portal, start; ///< WORLD_SIDE x WORLD_SIDE each, row = z, col = x
        std::vector<u32> stamp;
        u32 token = 0;
        u64 worlds = 0, balancedWorlds = 0, clipped = 0;

        CoverageAccumulator()
            : any(WORLD_CELLS, 0), anyBalanced(WORLD_CELLS, 0), portal(WORLD_CELLS, 0), start(WORLD_CELLS, 0),
              stamp(WORLD_CELLS, 0) {}

        static bool inWorld(c_int x, c_int z) {
            return x >= -WORLD_HALF && x < WORLD_HALF && z >= -WORLD_HALF && z < WORLD_HALF;
        }
        static size_t cellOf(c_int x, c_int z) {
            return static_cast<size_t>(z + WORLD_HALF) * WORLD_SIDE + static_cast<size_t>(x + WORLD_HALF);
        }

        /// Adds one world; returns the number of distinct columns covered by any piece.
        int add(const gen::Stronghold& sh, const Pos2D strongholdPos, c_bool balanced) {
            ++token;
            int covered = 0;
            for (int i = 0; i < sh.getPieceCount(); ++i) {
                const BoundingBox& bb = sh.getPieceConst(i);
                if (!inWorld(bb.m_minX, bb.m_minZ) || !inWorld(bb.m_maxX, bb.m_maxZ)) ++clipped;
                c_int x0 = std::max<int>(bb.m_minX, -WORLD_HALF), x1 = std::min<int>(bb.m_maxX, WORLD_HALF - 1);
                c_int z0 = std::max<int>(bb.m_minZ, -WORLD_HALF), z1 = std::min<int>(bb.m_maxZ, WORLD_HALF - 1);
                for (int z = z0; z <= z1; ++z) {
                    for (int x = x0; x <= x1; ++x) {
                        const size_t c = cellOf(x, z);
                        if (stamp[c] == token) continue;
                        stamp[c] = token;
                        ++any[c];
                        if (balanced) ++anyBalanced[c];
                        ++covered;
                    }
                }
            }
            const BoundingBox& p = *sh.m_portalRoomPiece;
            for (int z = std::max<int>(p.m_minZ, -WORLD_HALF); z <= std::min<int>(p.m_maxZ, WORLD_HALF - 1); ++z)
                for (int x = std::max<int>(p.m_minX, -WORLD_HALF); x <= std::min<int>(p.m_maxX, WORLD_HALF - 1); ++x)
                    ++portal[cellOf(x, z)];
            if (inWorld(strongholdPos.x, strongholdPos.z)) ++start[cellOf(strongholdPos.x, strongholdPos.z)];
            ++worlds;
            if (balanced) ++balancedWorlds;
            return covered;
        }

        void merge(const CoverageAccumulator& o) {
            for (size_t i = 0; i < WORLD_CELLS; ++i) {
                any[i] += o.any[i];
                anyBalanced[i] += o.anyBalanced[i];
                portal[i] += o.portal[i];
                start[i] += o.start[i];
            }
            worlds += o.worlds;
            balancedWorlds += o.balancedWorlds;
            clipped += o.clipped;
        }
    };

} // namespace shm
