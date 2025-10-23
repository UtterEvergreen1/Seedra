
#include "common/Pos3DTemplate.hpp"
#include "lce/processor.hpp"
#include "terrain/ChunkPrimer.hpp"

#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <span>

/*
 * | blocks | gridFormat[u16] | loc B | locSIZE | blkSIZ  | memory layout    | memory size    |
 * + ------ + --------------- + ----- + ------- + ------- + ---------------- + -------------- +
 * |  1    | block  .. 0000   | 0     | 0       | 0       | N/A              | N/A            |
 * |  2    | ptrOff .. 0001   | 1     | u64[1]  | u16[ 2] | u64[1] + u16[ 2] |   8 +  4 =  12 |
 * |  2- 3 | ptrOff .. 0010   | 2     | u64[2]  | u16[ 4] | u64[2] + u16[ 4] |  16 +  8 =  24 |
 * |  4- 7 | ptrOff .. 0011   | 3     | u64[3]  | u16[ 8] | u64[3] + u16[ 8] |  24 + 16 =  40 |
 * |  8-15 | ptrOff .. 0100   | 4     | u64[4]  | u16[16] | u64[4] + u16[16] |  32 + 32 =  64 |
 * | 16+   | ptrOff .. 0101   | N/A   | N/A     | u16[64] | u16[64]          | 128 +  0 = 128 |
 * + ----- + ---------------- + ------ + ------ + ------- + ---------------- + -------------- +
 */
class PalettedChunk {
public:
    static constexpr int NX =  16/4; // = 4
    static constexpr int NY = 128/4; // =32
    static constexpr int NZ =  16/4; // = 4
    static constexpr i32 START_ALLOC = 65536;
    static constexpr i32 GRID_SIZES[6] = {
            0,
            16,
            24,
            40,
            64,
            128,
    };

    u16 gridHeader[NX * NY * NZ] = {};
    u32 currentSize = START_ALLOC;
    u8* ptr = nullptr;
    u64 nextFreeOffset = 0;

    int   _lastGrid = -1;       // which grid is currently cached
    u16   _cache[64];           // decoded u16[64] for that grid

public:
    PalettedChunk() {
        ptr = new u8[START_ALLOC];
    }

    ~PalettedChunk() {
        delete[] ptr;
        ptr = nullptr;
    }

    static FORCEINLINE int getLocalIndexInGrid(int x, int y, int z) {
        return ((y & 3) << 4)
               | ((z & 3) << 2)
               |  (x & 3);
    }

    // Returns 0…(NX*NY*NZ−1) for which sub‑grid the block belongs to
    static FORCEINLINE int getSubgridIndex(int x, int y, int z) {
        int gx = x >> 2;
        int gy = y >> 2;
        int gz = z >> 2;
        return gx + gy * NX + gz * (NX * NY);
    }


    void FORCEINLINE setGridHeader(u32 gridIdx, u64 offset, u16 fmt) {
        gridHeader[gridIdx] = u16((offset << 1) | fmt);
    }

    u8* getNextGridLocation(int gridSize) {
        size_t offset = nextFreeOffset;
        if (offset + GRID_SIZES[gridSize] > START_ALLOC)
            throw std::bad_alloc();
        u8* base = ptr + offset;
        nextFreeOffset += GRID_SIZES[gridSize];
        return base;
    }

    void setBlock(int x, int y, int z, u16 newID) {
        // 1) locate grid + header
        int gridIdx = getSubgridIndex(x, y, z);
        u16 hdr     = gridHeader[gridIdx];
        int fmt     = hdr & 0xF;            // low‑4 = format code
        int loc     = getLocalIndexInGrid(x, y, z);

        // 2) UNIFORM → 1‑bit palette
        if (fmt == 0) {
            u16 oldID = hdr >> 4;
            if (oldID == newID) return;  // nothing to do

            // promote from uniform to 1‑bit palette
            u8* base    = getNextGridLocation(1);
            std::span<u64> planes(
                    reinterpret_cast<u64*>(base),     // one u64
                    1
            );
            std::span<u16> palette(
                    reinterpret_cast<u16*>(base + sizeof(u64)), // two u16s
                    2
            );

            planes[0]    = 0ULL;           // clear old bits
            palette[0]   = oldID;
            palette[1]   = newID;
            planes[0]   |= (1ULL << loc);

            setGridHeader(gridIdx, size_t(base - ptr), 1);
            return;
        }

        // 3) RAW16?
        if (fmt == 5) {
            u8* base = ptr + ((hdr & 0xFFF0) >> 1);
            std::span<u16> raw(
                    reinterpret_cast<u16*>(base),
                    64
            );
            raw[loc] = newID;
            return;
        }

        // 4) PALETTED (fmt = 1..4)
        int bits        = fmt;              // bits per block index
        int paletteSize = 1 << bits;
        u8* base        = ptr + ((hdr & 0xFFF0) >> 1);

        // wrap planes and palette in spans:
        std::span<u64> planes(
                reinterpret_cast<u64*>(base),
                bits
        );
        std::span<u16> palette(
                reinterpret_cast<u16*>(base + bits * sizeof(u64)),
                paletteSize
        );

        // (a) Already in palette or empty slot?
        {
            u64 mask = 1ULL << loc;
            for (int i = 0; i < paletteSize; ++i) {
                if (palette[i] == newID) {
                    // flip bits
                    for (int b = 0; b < bits; ++b)
                        (i & (1<<b) ? planes[b] |= mask
                                      : planes[b] &= ~mask);
                    return;
                }
                if (palette[i] == 0xFFFF) {
                    // empty: insert
                    palette[i] = newID;
                    for (int b = 0; b < bits; ++b)
                        if (i & (1<<b)) planes[b] |= mask;
                    return;
                }
            }
        }

        // 4.c) Need to grow palette → newFmt = bits+1
        if (fmt < 4) {
            int oldSize = 1 << bits;
            int newFmt  = bits + 1;

            u8* newBase = getNextGridLocation(newFmt);
            std::span<u64> oldPlanes = planes;
            std::span<u64> newPlanes(
                    reinterpret_cast<u64*>(newBase),
                    newFmt
            );
            // copy old planes
            for (int b = 0; b < bits; ++b)
                newPlanes[b] = oldPlanes[b];
            // set the new high‐bit plane only at `loc`
            newPlanes[bits] = 1ULL << loc;

            // palette spans
            std::span<u16> oldPal(
                    reinterpret_cast<u16*>(base + bits * sizeof(u64)),
                    oldSize
            );
            std::span<u16> newPal(
                    reinterpret_cast<u16*>(newBase + newFmt * sizeof(u64)),
                    1 << newFmt
            );
            // copy & extend
            for (int i = 0; i < oldSize; ++i)      newPal[i]          = oldPal[i];
            newPal[oldSize]                         = newID;
            for (int i = oldSize+1; i < (1<<newFmt); ++i) newPal[i] = 0xFFFF;

            setGridHeader(gridIdx, size_t(newBase - ptr), newFmt);
            return;
        }

        // 5) FULL → raw16 fallback
        {
            // decode full 4³ into stack
            u16 oldCache[64];
            for (int i = 0; i < 64; ++i) {
                u16 pi = 0;
                for (int b = 0; b < bits; ++b)
                    pi |= ((planes[b] >> i) & 1ULL) << b;
                oldCache[i] = palette[pi];
            }
            oldCache[loc] = newID;

            u8* newBase = getNextGridLocation(5);
            std::span<u16> rawDest(
                    reinterpret_cast<u16*>(newBase),
                    64
            );
            std::copy_n(oldCache, 64, rawDest.begin());

            setGridHeader(gridIdx, size_t(newBase - ptr), 5);
        }
    }

public:
    // ── replace your getBlock with this ──────────────────────────
    u16 getBlockCached(int x, int y, int z) {
        int gi  = getSubgridIndex(x, y, z);
        int loc = getLocalIndexInGrid(x, y, z);

        // if we’ve never decoded this grid, do it now
        if (_lastGrid != gi) {
            decodeGridToCache(gi);
            _lastGrid = gi;
        }

        // now just one L1‐resident lookup
        return _cache[loc];
    }


    u16 getBlock(i32 x, i32 y, i32 z) {
        // 1) Which sub‑grid?
        int gridIdx = getSubgridIndex(x, y, z);
        u16 hdr = gridHeader[gridIdx];

        // 2) Uniform?
        if ((hdr & 0xF) == 0b0000) {
            return (hdr & 0xFFF0) >> 4;   // low‑12 bits = block ID
        }

        // 3) Compute base pointer (must be 8‑byte aligned)
        u8* base = ptr + ((hdr & 0xFFF0) >> 1);

        // 4) Local index inside 4×4×4
        int loc = getLocalIndexInGrid(x, y, z);

        // 5) Raw16?
        if ((hdr & 0xF) == 0b0101) {
            auto raw = reinterpret_cast<c_u16*>(base);
            return raw[loc];
        }

        // 6) Paletted:
        int fmt             = hdr & 0xF;          // 0..3
        int bitsPerLocation = fmt;                // 1..4

        // 7) Bit‑planes come first:
        auto planes = std::span<u64>(
                reinterpret_cast<u64*>(base),
                bitsPerLocation
        );
        // auto planes = reinterpret_cast<c_u64*>(base);

        // 8) Extract your palette index by pulling bit 'loc' from each plane
        u16 blockIndex = 0;
        for (int b = 0; b < bitsPerLocation; ++b) {
            u16 bit = (planes[b] >> loc) & 1;
            blockIndex |= bit << b;
        }

        // 9) Palette follows immediately after the planes
        std::span<u16> palette(
                reinterpret_cast<u16*>(
                base + bitsPerLocation * sizeof(u64)),
                1 << bitsPerLocation
        );

        return palette[blockIndex];
    }

    void compact() {
        constexpr int TOTAL = NX*NY*NZ;

        // 1) Allocate new arena
        u8* newPtr = new u8[START_ALLOC];
        size_t writeOff = 0;

        // 2) Walk every grid in order
        for (int gi = 0; gi < TOTAL; ++gi) {
            u16 hdr = gridHeader[gi];
            int fmt = hdr & 0xF;
            if (fmt == 0) {
                // uniform grids have no payload
                continue;
            }
            // old offset (in bytes) and size
            size_t oldOff = (hdr >> 1);
            size_t sizeB  = GRID_SIZES[fmt];

            // 3) Copy into newPtr
            std::memcpy(newPtr + writeOff,
                        ptr    + oldOff,
                        sizeB);

            // 4) Patch header to new offset
            gridHeader[gi] = u16((writeOff << 1) | fmt);

            writeOff += sizeB;
        }

        // 5) Replace old buffer and update free pointer
        delete[] ptr;
        ptr = newPtr;
        nextFreeOffset = writeOff;
    }




private:
    // ── decode entire grid #gi into _cache[64] ────────────────────
    void decodeGridToCache(int gi) {
        u16 hdr = gridHeader[gi];
        //  uniform?
        if ((hdr & 0xF) == 0) {
            u16 id = hdr >> 4;
            for (int i = 0; i < 64; ++i) _cache[i] = id;
            return;
        }

        //  raw16?
        if ((hdr & 0xF) == 5) {
            auto base = ptr + ((hdr & 0xFFF0) >> 1);
            auto raw  = reinterpret_cast<const u16*>(base);
            for (int i = 0; i < 64; ++i) _cache[i] = raw[i];
            return;
        }

        //  paletted (1–4 bit planes)
        int fmt = hdr & 0xF;             // 1..4
        auto base     = ptr + ((hdr & 0xFFF0) >> 1);
        auto planes   = reinterpret_cast<const u64*>(base);
        auto palette  = reinterpret_cast<const u16*>(
                base + fmt*sizeof(u64));
        for (int i = 0; i < 64; ++i) {
            // extract the i’th palette bit
            u16 pi = 0;
            for (int b = 0; b < fmt; ++b)
                pi |= ((planes[b] >> i) & 1ULL) << b;
            _cache[i] = palette[pi];
        }
    }

};










using Key = std::tuple<int,int,int>;
struct KeyHash {
    size_t operator()(Key const& k) const noexcept {
        auto [x,y,z] = k;
        return ((size_t)x<<11) | ((size_t)y<<4) | (size_t)z;
    }
};
struct KeyEq {
    bool operator()(Key const& a, Key const& b) const noexcept {
        return a==b;
    }
};






int unit_test() {
    PalettedChunk chunk;

    // RNG for picking grids, locs and IDs
    std::mt19937_64 rng(123456);
    std::uniform_int_distribution<int> distGX(0, PalettedChunk::NX-1),
            distGY(0, PalettedChunk::NY-1),
            distGZ(0, PalettedChunk::NZ-1);
    std::uniform_int_distribution<int> distLoc(0,3);
    std::uniform_int_distribution<uint16_t> distID(1, 255);

    // Map from (x,y,z) to final expected ID
    std::unordered_map<Key,uint16_t,KeyHash,KeyEq> expected;
    expected.reserve(512);

    // Helper to pick a random block within a random grid
    auto pickRandomPos = [&](){
        int gx = distGX(rng), gy = distGY(rng), gz = distGZ(rng);
        int bx = gx*4, by = gy*4, bz = gz*4;
        int x = bx + distLoc(rng);
        int y = by + distLoc(rng);
        int z = bz + distLoc(rng);
        return std::make_tuple(x,y,z);
    };

    // Helper that calls setBlock *and* records expected
    auto SET = [&](int x,int y,int z, uint16_t v){
        chunk.setBlock(x,y,z,v);
        expected[{x,y,z}] = v;
    };

    // 1) Default empty → getBlock==0 (no recording here)
    for(int i=0;i<10;i++){
        auto [x,y,z] = pickRandomPos();
        assert(chunk.getBlock(x,y,z)==0);
    }

    // 2) Uniform→same ID (0)
    {
        auto [x,y,z] = pickRandomPos();
        SET(x,y,z, 0);
        assert(chunk.getBlock(x,y,z)==0);
    }

    // 3) Uniform→1‑bit palette
    uint16_t id1; int x1,y1,z1;
    {
        std::tie(x1,y1,z1) = pickRandomPos();
        id1 = distID(rng);
        SET(x1,y1,z1, id1);
        assert(chunk.getBlock(x1,y1,z1)==id1);
        // a different slot in same grid remains 0
        int x2 = (x1&~3) + ((x1&3)^1);
        assert(chunk.getBlock(x2,y1,z1)==0);
    }

    // 4) 1‑bit palette→existing entry
    {
        int x = x1, y = y1, z = z1 ^ 1; // same grid
        SET(x,y,z, id1);
        assert(chunk.getBlock(x,y,z)==id1);
    }

    // 5) 1‑bit→2‑bit promotion
    uint16_t id2; int x3,y3,z3;
    {
        std::tie(x3,y3,z3) = pickRandomPos();
        id2 = distID(rng);
        // ensure same grid as id1
        x3 = (x1&~3) + distLoc(rng);
        y3 = (y1&~3) + distLoc(rng);
        z3 = (z1&~3) + distLoc(rng);
        SET(x3,y3,z3, id2);
        assert(chunk.getBlock(x3,y3,z3)==id2);
        assert(chunk.getBlock(x1,y1,z1)==id1);
    }

    // 6) Fill 2‑bit palette (4 distinct)
    uint16_t id3,id4; int x4,y4,z4, x5,y5,z5;
    {
        std::tie(x4,y4,z4) = pickRandomPos();
        x4 = (x1&~3)+2; y4=(y1&~3); z4=(z1&~3);
        id3 = distID(rng); SET(x4,y4,z4,id3);
        assert(chunk.getBlock(x4,y4,z4)==id3);

        std::tie(x5,y5,z5) = pickRandomPos();
        x5 = (x1&~3)+3; y5=(y1&~3); z5=(z1&~3);
        id4 = distID(rng); SET(x5,y5,z5,id4);
        assert(chunk.getBlock(x5,y5,z5)==id4);
    }

    // 7) 2‑bit→3‑bit promotion
    uint16_t id5; int x6,y6,z6;
    {
        std::tie(x6,y6,z6) = pickRandomPos();
        x6=(x1&~3); y6=(y1&~3)+2; z6=(z1&~3);
        id5 = distID(rng); SET(x6,y6,z6,id5);
        assert(chunk.getBlock(x6,y6,z6)==id5);
    }

    // 8) Fill 3‑bit palette (8 slots)
    uint16_t id6,id7,id8; int x7,y7,z7,x8,y8,z8,x9,y9,z9;
    {
        std::tie(x7,y7,z7) = pickRandomPos();
        x7=(x1&~3)+1; y7=(y1&~3)+2; z7=(z1&~3); id6=distID(rng);
        SET(x7,y7,z7,id6); assert(chunk.getBlock(x7,y7,z7)==id6);

        std::tie(x8,y8,z8) = pickRandomPos();
        x8=(x1&~3)+2; y8=(y1&~3)+2; z8=(z1&~3); id7=distID(rng);
        SET(x8,y8,z8,id7); assert(chunk.getBlock(x8,y8,z8)==id7);

        std::tie(x9,y9,z9) = pickRandomPos();
        x9=(x1&~3)+3; y9=(y1&~3)+2; z9=(z1&~3); id8=distID(rng);
        SET(x9,y9,z9,id8); assert(chunk.getBlock(x9,y9,z9)==id8);
    }

    // 9) 4‑bit→raw16 fallback
    uint16_t id9; int x10,y10,z10;
    {
        std::tie(x10,y10,z10) = pickRandomPos();
        // same grid
        x10=(x1&~3)+3; y10=(y1&~3)+3; z10=(z1&~3);
        id9 = distID(rng); SET(x10,y10,z10,id9);
        assert(chunk.getBlock(x10,y10,z10)==id9);
    }

    // 10) raw16→update
    {
        uint16_t id10 = distID(rng);
        SET(x10,y10,z10, id10);
        assert(chunk.getBlock(x10,y10,z10)==id10);
    }

    // 11) boundary & multi‑grid
    {
        // grid A
        SET(3,3,3, 1234);
        // adjacent in +x
        SET(4,3,3, 5678);
        assert(chunk.getBlock(3,3,3)==1234);
        assert(chunk.getBlock(4,3,3)==5678);
    }

    // Final: verify *all* recorded sets
    for (auto const& kv : expected) {
        auto [x,y,z] = kv.first;
        uint16_t want = kv.second;
        uint16_t got  = chunk.getBlock(x,y,z);
        assert(got == want && "Final round‑trip mismatch");
    }

    // Random samples of untouched positions
    std::uniform_int_distribution<int> dx(0,15), dy(0,127), dz(0,15);
    for (int i = 0; i < 100000; ++i) {
        int x = dx(rng), y = dy(rng), z = dz(rng);
        Key k{x,y,z};
        if (!expected.count(k)) {
            assert(chunk.getBlock(x,y,z) == 0);
        }
    }

    std::cout<<"✅ All 11 cases *and* every setBlock were verified!\n";
    return 0;
}






static std::vector<std::tuple<int,int,int>> buildGridMajorCoords() {
    std::vector<std::tuple<int,int,int>> coords;
    coords.reserve(16*128*16);
    for (int gz = 0; gz < PalettedChunk::NZ; ++gz) {
        for (int gy = 0; gy < PalettedChunk::NY; ++gy) {
            for (int gx = 0; gx < PalettedChunk::NX; ++gx) {
                int bx = gx*4, by = gy*4, bz = gz*4;
                for (int dy = 0; dy < 4; ++dy) {
                    for (int dz = 0; dz < 4; ++dz) {
                        for (int dx = 0; dx < 4; ++dx) {
                            coords.emplace_back(bx+dx, by+dy, bz+dz);
                        }
                    }
                }
            }
        }
    }
    return coords;
}

void fillFlat(ChunkPrimer &flat, int uniquePerGrid) {
    for(int y=0;y<128;y++){
        for(int z=0;z<16;z++){
            for(int x=0;x<16;x++){
                int lx = x&3, ly = y&3, lz = z&3;
                int loc = (ly<<4)|(lz<<2)|lx;
                flat.blocks[(y<<8)|(z<<4)|x] = loc % uniquePerGrid;
            }
        }
    }
}

void fillCompressed(PalettedChunk &pc, int uniquePerGrid) {
    for(int y=0;y<128;y++){
        for(int z=0;z<16;z++){
            for(int x=0;x<16;x++){
                int lx = x&3, ly = y&3, lz = z&3;
                int loc = (ly<<4)|(lz<<2)|lx;
                pc.setBlock(x,y,z, uint16_t(loc % uniquePerGrid));
            }
        }
    }
}

// bench over a prebuilt coords list
template<class Fn>
double bench(Fn fn, int reps,
             const std::vector<std::tuple<int,int,int>>& coords) {
    using clk = std::chrono::high_resolution_clock;
    volatile uint16_t sink = 0;
    auto t0 = clk::now();
    for(int r = 0; r < reps; ++r) {
        for(auto [x,y,z] : coords) {
            sink ^= fn(x,y,z);
        }
    }
    auto t1 = clk::now();
    if (sink == 0xFFFF) std::cout<<"unreachable\n";
    return std::chrono::duration<double>(t1 - t0).count();
}

int benchmark(){
    constexpr std::array<int,6> uniqueCounts = {1,2,4,8,16,64};
    constexpr int REPS = 50;

    // build grid-major coords once
    auto coords = buildGridMajorCoords();

    std::mt19937_64 rng(123456);
    std::shuffle(coords.begin(), coords.end(), rng);

    for(int unique: uniqueCounts){
        ChunkPrimer flat;
        fillFlat(flat, unique);

        PalettedChunk pc;
        fillCompressed(pc, unique);
        pc.compact();

        // warm up
        bench([&](int x,int y,int z){ return flat.getBlockId(x,y,z); },
              1, coords);
        bench([&](int x,int y,int z){ return pc.getBlock(x,y,z); },
              1, coords);

        double tFlat = bench([&](int x,int y,int z){
            return flat.getBlockId(x,y,z);
        },
                             REPS, coords);

        double tComp = bench([&](int x,int y,int z){
            return pc.getBlock(x,y,z);
        },
                             REPS, coords);

        std::cout << "uniquePerGrid="<<unique
                  << "  flat="<<tFlat<<"s"
                  << "  compressed="<<tComp<<"s\n";
    }
    return 0;
}

int main(){
    unit_test();
    benchmark();
    return 0;
}














