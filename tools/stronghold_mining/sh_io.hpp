#pragma once
// Output helpers: NumPy .npy writer and the seeds CSV row format.

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "sh_core.hpp"

namespace shm {

    template<class T>
    constexpr const char* npyDescr();
    template<>
    constexpr const char* npyDescr<uint32_t>() {
        return "<u4";
    }
    template<>
    constexpr const char* npyDescr<uint8_t>() {
        return "|u1";
    }
    template<>
    constexpr const char* npyDescr<int32_t>() {
        return "<i4";
    }
    template<>
    constexpr const char* npyDescr<double>() {
        return "<f8";
    }

    /// Write a C-ordered array as a NumPy .npy file (format 1.0, little endian).
    template<class T>
    void writeNpy(const std::string& path, const T* data, const std::vector<size_t>& shape) {
        size_t count = 1;
        std::string shapeStr = "(";
        for (size_t i = 0; i < shape.size(); ++i) {
            count *= shape[i];
            shapeStr += std::to_string(shape[i]);
            shapeStr += (shape.size() == 1 || i + 1 < shape.size()) ? "," : "";
            if (i + 1 < shape.size()) shapeStr += " ";
        }
        shapeStr += ")";
        std::string header =
                std::string("{'descr': '") + npyDescr<T>() + "', 'fortran_order': False, 'shape': " + shapeStr + ", }";
        const size_t unpadded = 10 + header.size() + 1; // magic(6) + version(2) + length(2) + header + '\n'
        header.append((64 - unpadded % 64) % 64, ' ');
        header += '\n';

        std::ofstream f(path, std::ios::binary);
        if (!f) throw std::runtime_error("cannot write " + path);
        const char magic[8] = {'\x93', 'N', 'U', 'M', 'P', 'Y', 1, 0};
        f.write(magic, 8);
        const auto len = static_cast<uint16_t>(header.size());
        const char lenBytes[2] = {static_cast<char>(len & 0xFF), static_cast<char>(len >> 8)};
        f.write(lenBytes, 2);
        f.write(header.data(), static_cast<std::streamsize>(header.size()));
        f.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(count * sizeof(T)));
        if (!f) throw std::runtime_error("write failed: " + path);
    }

    /// Grid column name for offset index (gx, gz): b_<dx>_<dz>, negative values written as m32, m64, ...
    inline std::string gridColumnName(c_int gx, c_int gz) {
        auto fmt = [](c_int v) { return v < 0 ? "m" + std::to_string(-v) : std::to_string(v); };
        return "b_" + fmt((gx - GRID_HALF) * GRID_STEP) + "_" + fmt((gz - GRID_HALF) * GRID_STEP);
    }

    inline std::vector<std::string> seedCsvColumns() {
        std::vector<std::string> cols = {"sample",
                                         "seed",
                                         "balanced",
                                         "candidates",
                                         "spawn_x",
                                         "spawn_z",
                                         "player_x",
                                         "player_z",
                                         "spawn_biome",
                                         "player_biome",
                                         "sh_x",
                                         "sh_z",
                                         "attempts",
                                         "found",
                                         "target_x",
                                         "target_z",
                                         "angle",
                                         "mult",
                                         "dist",
                                         "spawn_valid_cells",
                                         "snap_valid_cells",
                                         "portal_x",
                                         "portal_z",
                                         "portal_y",
                                         "sh_top_y",
                                         "sh_bottom_y",
                                         "pieces",
                                         "footprint"};
        for (int gz = 0; gz < GRID_SIDE; ++gz)
            for (int gx = 0; gx < GRID_SIDE; ++gx) cols.push_back(gridColumnName(gx, gz));
        return cols;
    }

    inline int biomeOut(const biome_t b) { return b == biome_t::none ? -1 : static_cast<int>(b); }

    /// Append one CSV row (same order as seedCsvColumns()).
    inline void appendSeedCsvRow(std::string& out, const WorldSample& w) {
        char buf[512];
        const int n = std::snprintf(
                buf, sizeof(buf),
                "%llu,%lld,%d,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.6f,%d,%.6f,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                static_cast<unsigned long long>(w.sample), static_cast<long long>(w.seed), w.balanced,
                static_cast<unsigned>(w.candidates), w.spawn.x, w.spawn.z, w.player.x, w.player.z,
                biomeOut(w.spawnBiome), biomeOut(w.playerBiome), w.stronghold.x, w.stronghold.z, w.trace.attempts,
                w.trace.found, w.trace.target.x, w.trace.target.z, w.trace.angle, w.trace.multiplier, w.trace.distance,
                w.spawnValidCells, w.snapValidCells, w.portal.x, w.portal.z, w.portalY, w.topY, w.bottomY, w.pieces,
                w.footprint);
        out.append(buf, static_cast<size_t>(n));
        for (const biome_t b: w.grid) {
            out += ',';
            out += std::to_string(biomeOut(b));
        }
        out += '\n';
    }

} // namespace shm
