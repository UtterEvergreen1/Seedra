#pragma once

inline bool World::chunkExists(const Pos2D &pos) {
    if (pos.x < worldBounds.minX || pos.x > worldBounds.maxX || pos.z < worldBounds.minZ ||
        pos.z > worldBounds.maxZ) {
        return false;
    }

    std::lock_guard lock(chunkMutex);
    return chunks.contains(pos);
}

inline ChunkPrimer *World::getChunk(const Pos2D &pos) {
    std::lock_guard lock(chunkMutex);
    if (lastChunkCoords.load() == pos) {
        if (lastChunk != nullptr) {
            return lastChunk;
        }
    }

    if (pos.x < worldBounds.minX || pos.x > worldBounds.maxX ||
        pos.z < worldBounds.minZ || pos.z > worldBounds.maxZ) {
        return nullptr;
    }

    const auto it = chunks.find(pos);
    if (it != chunks.end()) {
        lastChunkCoords.store(pos);
        lastChunk.store(it->second);
        return it->second;
    }

    return nullptr;
}

inline ChunkPrimer *World::getOrCreateChunk(const Pos2D &chunkPos) {
    ChunkPrimer* chunk = getChunk(chunkPos);
    if (chunk != nullptr) {
        return chunk;
    }

    if (chunkPos.x < worldBounds.minX || chunkPos.x > worldBounds.maxX ||
        chunkPos.z < worldBounds.minZ || chunkPos.z > worldBounds.maxZ) {
        return nullptr;
    }

    if (ChunkPrimer* pooled = chunkPool.acquireFromPool()) {
        chunk = pooled; // already reset when released to pool
    } else {
        chunk = new ChunkPrimer();
    }

    addChunk(chunkPos, chunk);
    Chunk::provideChunk(chunk, *this->g, chunkPos);
    std::lock_guard lock(chunkMutex);
    lastChunkCoords.store(chunkPos);
    lastChunk.store(chunk);
    return chunk;
}

inline void World::addChunk(const Pos2D &pos, ChunkPrimer *chunk) {
    std::lock_guard lock(chunkMutex);
    chunks[pos] = chunk;
}

inline bool World::isAirBlock(c_int x, c_int y, c_int z) {
    if (const ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->isAirBlock(x & 15, y, z & 15);
    }
    return false;
}

inline bool World::isAirBlock(const Pos3D &pos) {
    return this->isAirBlock(pos.x, pos.y, pos.z);
}




inline lce::BlockState World::getBlock(const Pos3D &pos) {
    return this->getBlock(pos.x, pos.y, pos.z);
}

inline lce::BlockState World::getBlock(c_i32 x, c_i32 y, c_i32 z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlock(x & 15, y, z & 15);
    }
    return lce::BlocksInit::AIR.getState();
}

inline u16 World::getBlockId(c_i32 x, c_i32 y, c_i32 z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlockId(x & 15, y, z & 15);
    }
    return 0;
}

inline u16 World::getBlockId(const Pos3D &pos) {
    return this->getBlockId(pos.x, pos.y, pos.z);
}

inline void World::setBlock(c_int x, c_int y, c_int z, const lce::BlockState blockState) {
    this->setBlockAndData(x, y, z, blockState.getID(), blockState.getDataTag());
}

inline void World::setBlock(const Pos3D &pos, const lce::BlockState blockState) {
    this->setBlockAndData(pos.x, pos.y, pos.z, blockState.getID(), blockState.getDataTag());
}

// TODO: remove
inline void World::setBlockAndData(c_i32 x, c_i32 y, c_i32 z, c_u16 blockId, c_u8 meta) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    if (chunk == nullptr) return;
    chunk->setBlockAndData(x & 15, y, z & 15, blockId, meta);
    notifyNeighbors(x, y, z);
}

inline void World::setBlockAndData(const Pos3D &pos, c_u16 blockId, c_u8 meta) {
    this->setBlockAndData(pos.x, pos.y, pos.z, blockId, meta);
}

inline void World::setBlockId(c_i32 x, c_i32 y, c_i32 z, c_u16 blockId) {
    ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4});
    if (chunk == nullptr) return;
    chunk->setBlockId(x & 15, y, z & 15, blockId);
    notifyNeighbors(x, y, z);
}

inline void World::setBlockId(const Pos3D &pos, u16 blockId) {
    this->setBlockId(pos.x, pos.y, pos.z, blockId);
}

inline u16* World::mutBlockPtr(i32 x, i32 y, i32 z) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    if (chunk == nullptr) return nullptr;
    return chunk->mutBlockPtr(x, y, z);
}



inline void World::notifyNeighbors(MU c_int x, MU c_int y, MU c_int z) {
    //mimic notify neighbors to load chunks
    /*for (const auto faces: FACING_HORIZONTAL) {
        (void) this->getBlock(Pos3D(x, y, z).offset(faces));
    }*/
}

inline Pos3D World::getHeight(const Pos3D &pos) {
    return {pos.x, this->getHeight(pos.x, pos.z), pos.z};
}


inline int World::getHeight(c_int x, c_int z) {
    const Pos2D chunkPos = {x >> 4, z >> 4};
    const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getHeight(x & 15, z & 15);
    }
    return 0;
}

inline int World::getTopSolidOrLiquidBlock(c_int x, c_int z) {
    const Pos2D chunkPos = {x >> 4, z >> 4};
    const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getTopSolidOrLiquidBlock(x & 15, z & 15);
    }
    return 0;
}

inline Pos3D World::getTopSolidOrLiquidBlock(const Pos3D &pos) {
    return {pos.x, this->getTopSolidOrLiquidBlock(pos.x, pos.z), pos.z};
}

inline int World::getPrecipitationHeight(c_int x, c_int z) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    if (chunk) {
        return chunk->getPrecipitationHeight(x & 15, z & 15);
    }
    return 0;
}

inline bool World::canBlockFreeze(const Pos3D &pos, const bool noWaterAdj) {
    if (!this->hasIdealTemperature(pos))
        return false;

    if (pos.getY() >= 0 && pos.getY() < 256) {
        const Pos2D chunkPos = {pos.getX() >> 4, pos.getZ() >> 4};
        const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
        if (chunk) {
            const bool valid = chunk->canBlockFreeze(pos.convertToChunkCoords());
            if (!noWaterAdj) return valid;

            if (!valid) return false;
            for (const auto faces : FACING_HORIZONTAL) {
                if (chunk->canBlockFreeze(pos.offset(faces).convertToChunkCoords())) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

inline bool World::canBlockFreezeWater(const Pos3D &pos) {
    return this->canBlockFreeze(pos, false);
}

inline bool World::canSnowAt(const Pos3D &pos, const bool checkLight) {
    if (!this->hasIdealTemperature(pos))
        return false;

    if (!checkLight) { return true; }

    // TODO: needs to check block light later on to replicate a perfect chunk
    if (pos.getY() >= 0 && pos.getY() < 256) {
        const Pos2D chunkPos = {pos.getX() >> 4, pos.getZ() >> 4};
        const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
        if (chunk) {
            return chunk->canSnowAt(pos.convertToChunkCoords());
        }
    }
    return false;
}


static_assert(BlockAccess<World>, "ChunkPrimer must satisfy BlockAccess concept");