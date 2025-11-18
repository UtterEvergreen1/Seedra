#pragma once


#include "World.hpp"

class ChunkQuadAccessor {
public:
    struct ChunkPtr {
        int m_cx = 0, m_cz = 0;       // chunk coords
        ChunkPrimer* m_ptr = nullptr; // may be null at edges/out of bounds
    };


    explicit ChunkQuadAccessor() = default;

    // Construct a quad that covers [baseChunkX..baseChunkX+dx] × [baseChunkZ..baseChunkZ+dz]
    // where dx,dz are 0 or 1 depending on whether the vein might cross boundaries.
    ChunkQuadAccessor(World* world, int baseChunkX, int baseChunkZ, bool needXPlus, bool needZPlus)
        : m_world(world), m_baseCX(baseChunkX), m_baseCZ(baseChunkZ) {
        // Always fetch c00
        m_c00 = fetch(m_baseCX, m_baseCZ);
        // Conditionally fetch the neighbors (max 3 more)
        m_c10 = needXPlus ? fetch(m_baseCX + 1, m_baseCZ) : ChunkPtr{};
        m_c01 = needZPlus ? fetch(m_baseCX, m_baseCZ + 1) : ChunkPtr{};
        m_c11 = (needXPlus && needZPlus) ? fetch(m_baseCX + 1, m_baseCZ + 1) : ChunkPtr{};
    }

    // Decide which of the 4 prefetched chunks a block (bx,bz) belongs to; return nullptr if outside.
    ND inline ChunkPrimer* pick(const int bx, const int bz) const {
        const int cx = bx >> 4;
        const int cz = bz >> 4;

        if (cx == m_baseCX) {
            if (cz == m_baseCZ) return m_c00.m_ptr;
            if (cz == m_baseCZ + 1) return m_c01.m_ptr;
        } else if (cx == m_baseCX + 1) {
            if (cz == m_baseCZ) return m_c10.m_ptr;
            if (cz == m_baseCZ + 1) return m_c11.m_ptr;
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
        return m_world->getBlockId(Pos3D(bx, by, bz));
    }

    inline void setBlock(int bx, int by, int bz, const lce::BlockState& st) const {
        if (ChunkPrimer* c = pick(bx, bz)) {
            setBlockLocal(c, bx, by, bz, st);
            return;
        }
        m_world->setBlock(Pos3D(bx, by, bz), st);
    }

private:
    World* m_world{};
    int m_baseCX{}, m_baseCZ{};
    ChunkPtr m_c00{}, m_c10{}, m_c01{}, m_c11{};

    ND inline ChunkPtr fetch(const int cx, const int cz) const {
        ChunkPtr out;
        out.m_cx = cx;
        out.m_cz = cz;
        out.m_ptr = m_world->getChunk(Pos2D{cx, cz}); // uses World’s own locking once per chunk
        return out;
    }
};
