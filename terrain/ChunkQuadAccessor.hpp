#pragma once


#include "World.hpp"

class ChunkQuadAccessor {
public:
    struct ChunkPtr {
        int cx = 0, cz = 0;         // chunk coords
        ChunkPrimer* ptr = nullptr; // may be null at edges/out of bounds
    };


    explicit ChunkQuadAccessor() = default;

    // Construct a quad that covers [baseChunkX..baseChunkX+dx] × [baseChunkZ..baseChunkZ+dz]
    // where dx,dz are 0 or 1 depending on whether the vein might cross boundaries.
    ChunkQuadAccessor(World* world, int baseChunkX, int baseChunkZ, bool needXPlus, bool needZPlus)
        : world(world), baseCX(baseChunkX), baseCZ(baseChunkZ) {
        // Always fetch c00
        c00 = fetch(baseCX, baseCZ);
        // Conditionally fetch the neighbors (max 3 more)
        c10 = needXPlus ? fetch(baseCX + 1, baseCZ) : ChunkPtr{};
        c01 = needZPlus ? fetch(baseCX, baseCZ + 1) : ChunkPtr{};
        c11 = (needXPlus && needZPlus) ? fetch(baseCX + 1, baseCZ + 1) : ChunkPtr{};
    }

    // Decide which of the 4 prefetched chunks a block (bx,bz) belongs to; return nullptr if outside.
    ND inline ChunkPrimer* pick(int bx, int bz) const {
        const int cx = bx >> 4;
        const int cz = bz >> 4;

        if (cx == baseCX) {
            if (cz == baseCZ) return c00.ptr;
            if (cz == baseCZ + 1) return c01.ptr;
        } else if (cx == baseCX + 1) {
            if (cz == baseCZ) return c10.ptr;
            if (cz == baseCZ + 1) return c11.ptr;
        }
        return nullptr; // outside prefetched quad
    }

    static inline int getBlockIdLocal(const ChunkPrimer* c, int bx, int by, int bz) {
        return c->getBlockId(bx & 15, by, bz & 15);
    }

    static inline void setBlockLocal(ChunkPrimer* c, int bx, int by, int bz, const lce::BlockState& st) {
        c->setBlock(bx & 15, by, bz & 15, st);
    }

    ND inline int getBlockId(int bx, int by, int bz) const {
        if (ChunkPrimer* c = pick(bx, bz)) {
            return getBlockIdLocal(c, bx, by, bz);
        }
        return world->getBlockId(Pos3D(bx, by, bz));
    }

    inline void setBlock(int bx, int by, int bz, const lce::BlockState& st) const {
        if (ChunkPrimer* c = pick(bx, bz)) {
            setBlockLocal(c, bx, by, bz, st);
            return;
        }
        world->setBlock(Pos3D(bx, by, bz), st);
    }

private:
    World* world{};
    int baseCX{}, baseCZ{};
    ChunkPtr c00{}, c10{}, c01{}, c11{};

    ND inline ChunkPtr fetch(int cx, int cz) const {
        ChunkPtr out;
        out.cx = cx;
        out.cz = cz;
        out.ptr = world->getChunk(Pos2D{cx, cz}); // uses World’s own locking once per chunk
        return out;
    }
};