// GreedyCombiner.cpp
// 3D greedy rectangular prism combiner for Minecraft structure NBT
// Uses your NBTBase API (nbt.hpp). No parsing needed here—just read tags.

#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cctype>
#include <cassert>

#include "common/nbt.hpp"

// -------------------------
// Data types
// -------------------------

struct Region {
    int state;                 // palette index
    int min_x, min_y, min_z;   // inclusive
    int max_x, max_y, max_z;   // inclusive

    inline int64_t volume() const {
        return int64_t(max_x - min_x + 1) *
               int64_t(max_y - min_y + 1) *
               int64_t(max_z - min_z + 1);
    }
};

struct StructureGrid {
    int sx = 0, sy = 0, sz = 0;           // dimensions
    int airIndex = -1;                    // palette index for air, if found
    std::vector<int32_t> grid;            // size sx*sy*sz, value = palette state index or -1

    inline size_t idx(int x, int y, int z) const {
        // X-fastest, then Y, then Z
        // (x in [0..sx), y in [0..sy), z in [0..sz))
        return size_t(x) + size_t(sx) * (size_t(y) + size_t(sy) * size_t(z));
    }
};

// -------------------------
// NBT helpers
// -------------------------

static inline bool readPosXYZ(const NBTBase& tag, int& x, int& y, int& z) {
    // Supports either INT_ARRAY [x,y,z] or LIST of 3 ints
    if (tag.is<NBTIntArray>()) {
        const auto& arr = tag.get<NBTIntArray>();
        if (arr.size() < 3) return false;
        x = arr[0]; y = arr[1]; z = arr[2];
        return true;
    }
    if (tag.is<NBTList>()) {
        const auto& li = tag.get<NBTList>();
        if (li.size() < 3) return false;
        x = li[0].get<i32>();
        y = li[1].get<i32>();
        z = li[2].get<i32>();
        return true;
    }
    return false;
}

static StructureGrid buildGridFromNBT(const NBTBase& root, bool fillUnsetAsAir = true) {
    StructureGrid sg{};

    // --- size ---
    {
        const NBTBase& sizeTag = root("size");
        int sx=0, sy=0, sz=0;
        if (!readPosXYZ(sizeTag, sx, sy, sz)) {
            throw std::runtime_error("size tag missing or invalid");
        }
        if (sx <= 0 || sy <= 0 || sz <= 0) {
            throw std::runtime_error("size has non-positive dimension");
        }
        sg.sx = sx; sg.sy = sy; sg.sz = sz;
    }

    // --- palette (detect air index if present) ---
    int airIdx = -1;
    {
        const NBTBase& paletteTag = root("palette");
        if (!paletteTag || !paletteTag.is<NBTList>()) {
            throw std::runtime_error("palette missing or not a list");
        }
        const auto& pal = paletteTag.get<NBTList>();
        for (size_t i = 0; i < pal.size(); ++i) {
            const NBTBase& entry = pal[i];
            std::string name = entry("Name").is<std::string>() ? entry("Name").get<std::string>()
                                                               : std::string{};
            if (name.empty() && entry.is<NBTCompound>()) {
                if (auto opt = entry.get<NBTCompound>().value<std::string>("Name")) name = *opt;
            }
            if (airIdx < 0 && name == "minecraft:air") airIdx = int(i);
        }
    }
    sg.airIndex = airIdx;

    // --- grid init ---
    const size_t total = size_t(sg.sx) * size_t(sg.sy) * size_t(sg.sz);
    const int32_t emptyVal = (fillUnsetAsAir && airIdx >= 0) ? airIdx : -1;
    sg.grid.assign(total, emptyVal);

    // --- blocks: each entry has "state" and "pos": [x,y,z] ---
    {
        const NBTBase& blocksTag = root("blocks");
        if (!blocksTag || !blocksTag.is<NBTList>()) {
            // Some structures omit blocks if empty; that's OK.
            return sg;
        }
        const auto& blocks = blocksTag.get<NBTList>();
        for (size_t i = 0; i < blocks.size(); ++i) {
            const NBTBase& b = blocks[i];
            if (!b.is<NBTCompound>()) continue;

            int st = b("state").is<i32>() ? b("state").get<i32>() : -1;
            if (st < 0) continue;

            int x=0, y=0, z=0;
            if (!readPosXYZ(b("pos"), x, y, z)) continue;

            if (x < 0 || y < 0 || z < 0 || x >= sg.sx || y >= sg.sy || z >= sg.sz) {
                // If you want, clamp or skip; we'll skip for safety.
                continue;
            }
            sg.grid[sg.idx(x,y,z)] = st;
        }
    }

    return sg;
}

// -------------------------
// Greedy combiner
// -------------------------

// Greedy expansion from seed (x0, y0, z0) using target state; marks visited.
// Returns the maximal axis-aligned box starting at (x0,y0,z0) with that state
// while respecting "visited" and "grid".
static Region growMaximalBox(const StructureGrid& sg,
                             int x0, int y0, int z0,
                             int target,
                             std::vector<uint8_t>& visited)
{
    const int sx = sg.sx, sy = sg.sy, sz = sg.sz;

    auto taken = [&](int x,int y,int z)->bool {
        return visited[sg.idx(x,y,z)] != 0;
    };
    auto matches = [&](int x,int y,int z)->bool {
        return sg.grid[sg.idx(x,y,z)] == target && !taken(x,y,z);
    };

    // 1) expand in +X
    int ex = 1;
    while (x0 + ex < sx) {
        bool ok = matches(x0 + ex, y0, z0);
        if (!ok) break;
        ++ex;
    }

    // 2) expand in +Y while each new row [x0..x0+ex) matches
    int ey = 1;
    while (y0 + ey < sy) {
        bool ok = true;
        const int y = y0 + ey;
        for (int x = 0; x < ex; ++x) {
            if (!matches(x0 + x, y, z0)) { ok = false; break; }
        }
        if (!ok) break;
        ++ey;
    }

    // 3) expand in +Z while each new slab (ex by ey) matches
    int ez = 1;
    while (z0 + ez < sz) {
        bool ok = true;
        const int z = z0 + ez;
        for (int y = 0; y < ey; ++y) {
            for (int x = 0; x < ex; ++x) {
                if (!matches(x0 + x, y0 + y, z)) { ok = false; break; }
            }
            if (!ok) break;
        }
        if (!ok) break;
        ++ez;
    }

    // mark visited
    for (int dz = 0; dz < ez; ++dz)
        for (int dy = 0; dy < ey; ++dy)
            for (int dx = 0; dx < ex; ++dx)
                visited[sg.idx(x0+dx, y0+dy, z0+dz)] = 1;

    Region r;
    r.state = target;
    r.min_x = x0;             r.max_x = x0 + ex - 1;
    r.min_y = y0;             r.max_y = y0 + ey - 1;
    r.min_z = z0;             r.max_z = z0 + ez - 1;
    return r;
}

// Main combiner: one pass over the volume; for each unvisited non-ignored cell, grow a maximal box.
static std::vector<Region>
greedyCombineAllStates(const StructureGrid& sg, int ignoreState /* e.g., airIndex or -1 */)
{
    std::vector<Region> out;
    if (sg.sx == 0 || sg.sy == 0 || sg.sz == 0) return out;

    std::vector<uint8_t> visited(sg.grid.size(), 0);

    for (int z = 0; z < sg.sz; ++z) {
        for (int y = 0; y < sg.sy; ++y) {
            for (int x = 0; x < sg.sx; ++x) {
                const size_t i = sg.idx(x,y,z);
                if (visited[i]) continue;
                int st = sg.grid[i];
                if (st == ignoreState) continue;
                // Seed a new region
                Region r = growMaximalBox(sg, x,y,z, st, visited);
                out.push_back(r);
            }
        }
    }

    // Sort largest first (optional)
    std::sort(out.begin(), out.end(),
              [](const Region& a, const Region& b){ return a.volume() > b.volume(); });
    return out;
}

// -------------------------
// Emission helpers (naming + comments)
// -------------------------

static std::string baseIdFromFull(const std::string& full) {
    // "minecraft:purpur_stairs" -> "purpur_stairs"
    auto pos = full.find(':');
    std::string s = (pos == std::string::npos) ? full : full.substr(pos + 1);
    // sanitize for C++ identifier just in case
    std::string out; out.reserve(s.size());
    for (char c : s) {
        if (std::isalnum((unsigned char)c) || c == '_')
            out.push_back((char)std::tolower((unsigned char)c));
        else
            out.push_back('_');
    }
    if (out.empty()) out = "unknown";
    return out;
}

static std::string toBlocksInitConst(const std::string& full) {
    // "minecraft:purpur_stairs" -> "PURPUR_STAIRS"
    auto pos = full.find(':');
    std::string s = (pos == std::string::npos) ? full : full.substr(pos + 1);
    std::string out; out.reserve(s.size());
    for (char c : s) {
        if (std::isalnum((unsigned char)c))
            out.push_back((char)std::toupper((unsigned char)c));
        else
            out.push_back('_');
    }
    if (out.empty()) out = "UNKNOWN";
    return out;
}

static std::string propertiesComment(const NBTBase& paletteEntry) {
    const NBTBase& props = paletteEntry("Properties");
    if (!props || !props.is<NBTCompound>()) return "// (no properties)";

    const auto& cmp = props.get<NBTCompound>();
    auto keys = props.getKeySet();
    std::sort(keys.begin(), keys.end());

    std::ostringstream oss;
    oss << "// ";
    bool first = true;
    for (const auto& k : keys) {
        const NBTBase* v = cmp.find(k);
        if (!v) continue;
        if (!first) oss << ", ";
        first = false;

        // pretty print value (mostly strings)
        std::string vs;
        if (v->is<std::string>()) {
            vs = v->get<std::string>(); // unquoted
        } else if (v->is<i32>()) { vs = std::to_string(v->get<i32>()); }
        else if (v->is<i16>())  { vs = std::to_string(v->get<i16>()); }
        else if (v->is<i64>())  { vs = std::to_string(v->get<i64>()); }
        else if (v->is<u8>())   { vs = std::to_string((int)v->get<u8>()); }
        else { vs = v->to_string_shallow(); }

        oss << k << "=" << vs;
    }
    if (first) return "// (no properties)"; // had a Properties tag but empty
    return oss.str();
}

// -------------------------
// Code generator (updated per requirements)
// -------------------------

static std::string generateStructureCalls(const std::vector<Region>& regions,
                                          const NBTBase& root,
                                          int airIndex)
{
    std::ostringstream oss;
    const NBTBase& paletteTag = root("palette");
    const bool hasPalette = (paletteTag && paletteTag.is<NBTList>());
    const auto& pal = hasPalette ? paletteTag.get<NBTList>() : *(new NBTList()); // only used if hasPalette==true

    // Determine which palette states are actually used
    std::vector<int> usedStates;
    {
        std::unordered_set<int> used;
        used.reserve(regions.size());
        for (const Region& r : regions) {
            if (used.insert(r.state).second) usedStates.push_back(r.state);
        }
        std::sort(usedStates.begin(), usedStates.end());
    }

    // Number variables per *block name* (minecraft:id) in order of palette index
    std::unordered_map<int, std::string> stateVar;    // palette index -> var name
    std::unordered_map<std::string, int> nameCounter; // base name -> next suffix

    oss << "// === Block state declarations (unique per palette entry) ===\n";
    for (int st : usedStates) {
        // Resolve full name
        std::string fullName;
        if (hasPalette && st >= 0 && st < (int)pal.size()) {
            const NBTBase& entry = pal[st];
            fullName = entry("Name").is<std::string>() ? entry("Name").get<std::string>() : std::string{};
            if (fullName.empty() && entry.is<NBTCompound>()) {
                if (auto opt = entry.get<NBTCompound>().value<std::string>("Name")) fullName = *opt;
            }
        }
        if (fullName.empty()) fullName = "minecraft:unknown_" + std::to_string(st);

        // Create variable name and BlocksInit constant name
        std::string base     = baseIdFromFull(fullName);        // e.g. purpur_stairs
        int suffix           = nameCounter[base]++;             // 0,1,2...
        std::string var      = base + std::to_string(suffix);   // purpur_stairs0
        std::string initName = toBlocksInitConst(fullName);     // PURPUR_STAIRS

        stateVar[st] = var;

        // Emit initializer + trailing properties comment on SAME line
        oss << "const lce::BlockState " << var
            << " = lce::BlocksInit::" << initName << ".getDefaultState(); ";

        if (hasPalette && st >= 0 && st < (int)pal.size()) {
            oss << propertiesComment(pal[st]) << "\n";
        } else {
            oss << "// (no properties)\n";
        }
    }

    oss << "\n// === Fill calls ===\n";

    // Order regions for readability
    std::vector<size_t> order(regions.size());
    for (size_t i = 0; i < regions.size(); ++i) order[i] = i;
    std::stable_sort(order.begin(), order.end(),
                     [&](size_t a, size_t b){
                         if (regions[a].min_y != regions[b].min_y) return regions[a].min_y < regions[b].min_y;
                         if (regions[a].min_z != regions[b].min_z) return regions[a].min_z < regions[b].min_z;
                         return regions[a].min_x < regions[b].min_x;
                     });

    int curY = std::numeric_limits<int>::min();
    for (size_t oi : order) {
        const Region& r = regions[oi];
        if (r.min_y != curY) {
            curY = r.min_y;
            oss << "\n// -- Layer Y = " << curY << " --\n";
        }

        const bool isAir   = (airIndex >= 0 && r.state == airIndex);
        const bool single  = (r.volume() == 1);
        const std::string& var = stateVar[r.state]; // declared above

        if (isAir && r.volume() > 1) {
            oss << "fillWithAir(worldIn, chunkBB, "
                << r.min_x << "," << r.min_y << "," << r.min_z << ", "
                << r.max_x << "," << r.max_y << "," << r.max_z << ");\n";
        } else if (single) {
            // Preferred order: (worldIn, chunkBB, x,y,z, state)
            oss << "setBlockState(worldIn, chunkBB, "
                << r.min_x << "," << r.min_y << "," << r.min_z << ", "
                << var << ");\n";
        } else {
            oss << "fillWithBlocks(worldIn, chunkBB, "
                << r.min_x << "," << r.min_y << "," << r.min_z << ", "
                << r.max_x << "," << r.max_y << "," << r.max_z << ", "
                << var << "," << var << ", false);\n";
        }
    }

    return oss.str();
}

// -------------------------
// Example usage (replace with your pipeline)
// -------------------------

#define DEMO_GREEDY_COMBINER_MAIN
#ifdef DEMO_GREEDY_COMBINER_MAIN
int main(int argc, char* argv[]) {
    const std::string path = (argc > 1) ? argv[1] : "nbt/igloo/igloo_middle";

    // Your loader pattern:
    NBTBase root;
    root = root.readFile(path)[""]; // as per your snippet

    // Build dense grid (treat unset voxels as air if palette has air)
    StructureGrid sg = buildGridFromNBT(root, /*fillUnsetAsAir=*/true);

    // Merge everything except air (if known).
    const int ignore = (sg.airIndex >= 0 ? sg.airIndex : -1);
    std::vector<Region> solids = greedyCombineAllStates(sg, ignore);

    // Stats
    int64_t placed = 0;
    for (const Region& r : solids) placed += r.volume();
    std::cout << "Greedy regions: " << solids.size()
              << " covering " << placed << " blocks\n";

    // Emit structure-generation declarations + calls
    std::string calls = generateStructureCalls(solids, root, sg.airIndex);
    std::cout << "\n" << calls << "\n";

    return 0;
}
#endif






/*
int main(int argc, char* argv[]) {
    const std::string path = "nbt/igloo/igloo_top";
    NBTBase root;
    root = root.readFile(path)[""];

    NBTBase& author = root["author"];
    NBTBase& palette = root["palette"];
    NBTBase& blocks = root["blocks"];
    NBTBase& version = root["version"];
    NBTBase& size = root["size"];

    return 0;
}
 */