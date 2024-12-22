#include "World.hpp"

#include "Chunk.hpp"

void World::addChunk(const Pos2D &pos, ChunkPrimer *chunk) {
    chunks[pos] = chunk;
}

ChunkPrimer *World::getChunk(const Pos2D &pos) const {
    auto it = chunks.find(pos);
    return it != chunks.end() ? it->second : nullptr;
}

ChunkPrimer *World::getOrCreateChunk(const Pos2D &pos) {
    auto it = chunks.find(pos);
    if (it != chunks.end()) {
        return it->second;
    }
    auto *chunk = Chunk::provideChunk(*this->g, pos.x, pos.z);
    addChunk(pos, chunk);
    return chunk;
}

void World::decorateChunks(const Pos2D &pos, int radius) {
    std::cout << "Decorating chunks around " << pos << " with radius " << radius << std::endl;
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dz = -radius; dz <= radius; ++dz) {
            Pos2D chunkPos = pos + Pos2D(dx, dz);
            this->getOrCreateChunk(chunkPos);
            Chunk::populateChunk(*g, chunkPos.x, chunkPos.z, this);
        }
    }
}

int World::getBlockId(int x, int y, int z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlockId(x & 15, y, z & 15);
    }
    return 0;
}

int World::getBlockId(const Pos3D &pos) {
    return this->getBlockId(pos.getX(), pos.getY(), pos.getZ());
}

const lce::blocks::Block *World::getBlock(int x, int y, int z) {
    if (ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlock(x & 15, y, z & 15);
    }
    return nullptr;
}

const lce::blocks::Block *World::getBlock(const Pos3D &pos) {
    return this->getBlock(pos.getX(), pos.getY(), pos.getZ());
}

void World::notifyNeighbors(int x, int y, int z) {
    //mimic notify neighbors to load chunks
    for (auto faces : FACING_HORIZONTAL) {
        (void*)this->getBlock(Pos3D(x, y, z).offset(faces));
    }
}

void World::setBlock(int x, int y, int z, int blockId) {
    ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4});
    chunk->setBlockId(x & 15, y, z & 15, blockId);
    notifyNeighbors(x, y, z);
}

void World::setBlock(const Pos3D &pos, int blockId) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), blockId);
}

void World::setBlock(int x, int y, int z, int blockId, int meta) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    chunk->setBlockAndData(x & 15, y, z & 15, blockId, meta);
    notifyNeighbors(x, y, z);
}

void World::setBlock(const Pos3D &pos, int blockId, int meta) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), blockId, meta);
}

void World::setBlock(int x, int y, int z, const lce::blocks::Block *block) {
    this->setBlock(x, y, z, block->getID(), block->getDataTag());
}

void World::setBlock(const Pos3D &pos, const lce::blocks::Block *block) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), block->getID(), block->getDataTag());
}

bool World::isAirBlock(int x, int y, int z) {
    if (ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->isAirBlock(x & 15, y, z & 15);
    }
    return false;
}

bool World::isAirBlock(const Pos3D &pos) {
    return this->isAirBlock(pos.getX(), pos.getY(), pos.getZ());
}

int World::getHeight(int x, int z) {
    Pos2D chunkPos = {x >> 4, z >> 4};
    ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getHeight(x & 15, z & 15);
    }
    return 0;
}

Pos3D World::getHeight(const Pos3D &pos) {
    return {pos.getX(), this->getHeight(pos.getX(), pos.getZ()), pos.getZ()};
}

int World::getTopSolidOrLiquidBlock(int x, int z) {
    Pos2D chunkPos = {x >> 4, z >> 4};
    ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getTopSolidOrLiquidBlock(x & 15, z & 15);
    }
    return 0;
}

Pos3D World::getTopSolidOrLiquidBlock(const Pos3D &pos) {
    return {pos.getX(), this->getTopSolidOrLiquidBlock(pos.getX(), pos.getZ()), pos.getZ()};
}

int World::getPrecipitationHeight(c_int x, c_int z) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    if (chunk) {
        return chunk->getPrecipitationHeight(x & 15, z & 15);
    }
    return 0;
}

bool World::canBlockFreeze(const Pos3D &pos, bool noWaterAdj) {
    if (Biome::getBiomeForId(g->getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F)
        return false;

    if (pos.getY() >= 0 && pos.getY() < 256) {
        Pos2D chunkPos = {pos.getX() >> 4, pos.getZ() >> 4};
        ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
        if (chunk) {
            return chunk->canBlockFreeze(*g, pos.convertToChunkCords(), noWaterAdj);
        }
    }
    return false;
}

ND bool World::canBlockFreezeWater(const Pos3D &pos) {
    return this->canBlockFreeze(pos, false);
}

bool World::canSnowAt(const Pos3D &pos, bool checkLight) {
    if (Biome::getBiomeForId(g->getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F) {
        return false;
    }

    if (!checkLight) { return true; }

    if (pos.getY() >= 0 && pos.getY() < 256) {
        Pos2D chunkPos = {pos.getX() >> 4, pos.getZ() >> 4};
        ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
        if (chunk) {
            return chunk->canSnowAt(*g, pos.convertToChunkCords(), checkLight);
        }
    }
    return false;
}