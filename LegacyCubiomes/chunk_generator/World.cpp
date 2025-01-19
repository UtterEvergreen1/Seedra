#include "World.hpp"

#include "Chunk.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/mineshaft.hpp"
#include "LegacyCubiomes/structures/placement/stronghold.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "LegacyCubiomes/structures/gen/mineshaft/mineshaft.hpp"
#include "LegacyCubiomes/structures/gen/stronghold/stronghold.hpp"

World::~World() {
    this->deleteWorld();
}

void World::deleteWorld() {
    for (auto &chunk : chunks) {
        delete chunk.second;
    }
    chunks.clear();
    villages.clear();
    strongholds.clear();
    mineshafts.clear();
}

void World::addChunk(const Pos2D &pos, ChunkPrimer *chunk) {
    chunks[pos] = chunk;
}

ChunkPrimer *World::getChunk(const Pos2D &pos) const {
    const auto it = chunks.find(pos);
    return it != chunks.end() ? it->second : nullptr;
}

ChunkPrimer *World::getOrCreateChunk(const Pos2D &pos) {
    const auto it = chunks.find(pos);
    if (it != chunks.end()) {
        return it->second;
    }
    auto *chunk = Chunk::provideChunk(*this->g, pos.x, pos.z);
    addChunk(pos, chunk);
    return chunk;
}

void World::decorateChunks(const Pos2D &pos, const int radius) {
    std::cout << "Decorating chunks around " << pos << " with radius " << radius << std::endl;
    for (int dx = -radius; dx <= radius - 1; ++dx) {
        for (int dz = -radius; dz <= radius - 1; ++dz) {
            Pos2D chunkPos = pos + Pos2D(dx, dz);
            this->getOrCreateChunk(chunkPos);
            Chunk::populateChunk(*this, *g, chunkPos.x, chunkPos.z);
        }
    }
}

int World::getBlockId(const int x, const int y, const int z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlockId(x & 15, y, z & 15);
    }
    return 0;
}

int World::getBlockId(const Pos3D &pos) {
    return this->getBlockId(pos.getX(), pos.getY(), pos.getZ());
}

const lce::blocks::Block *World::getBlock(const int x, const int y, const int z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlock(x & 15, y, z & 15);
    }
    return nullptr;
}

const lce::blocks::Block *World::getBlock(const Pos3D &pos) {
    return this->getBlock(pos.getX(), pos.getY(), pos.getZ());
}

void World::notifyNeighbors(const int x, const int y, const int z) {
    //mimic notify neighbors to load chunks
    for (const auto faces : FACING_HORIZONTAL) {
        (void*)this->getBlock(Pos3D(x, y, z).offset(faces));
    }
}

void World::setBlock(const int x, const int y, const int z, const int blockId) {
    ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4});
    chunk->setBlockId(x & 15, y, z & 15, blockId);
    notifyNeighbors(x, y, z);
}

void World::setBlock(const Pos3D &pos, const int blockId) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), blockId);
}

void World::setBlock(const int x, const int y, const int z, const int blockId, const int meta) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    chunk->setBlockAndData(x & 15, y, z & 15, blockId, meta);
    notifyNeighbors(x, y, z);
}

void World::setBlock(const Pos3D &pos, const int blockId, const int meta) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), blockId, meta);
}

void World::setBlock(const int x, const int y, const int z, const lce::blocks::Block *block) {
    this->setBlock(x, y, z, block->getID(), block->getDataTag());
}

void World::setBlock(const Pos3D &pos, const lce::blocks::Block *block) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), block->getID(), block->getDataTag());
}

void World::setBlock(const int x, const int y, const int z, const lce::blocks::Block& block) {
    this->setBlock(x, y, z, block.getID(), block.getDataTag());
}

void World::setBlock(const Pos3D &pos, const lce::blocks::Block& block) {
    this->setBlock(pos.getX(), pos.getY(), pos.getZ(), block.getID(), block.getDataTag());
}

bool World::isAirBlock(const int x, const int y, const int z) {
    if (const ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->isAirBlock(x & 15, y, z & 15);
    }
    return false;
}

bool World::isAirBlock(const Pos3D &pos) {
    return this->isAirBlock(pos.getX(), pos.getY(), pos.getZ());
}

int World::getHeight(const int x, const int z) {
    const Pos2D chunkPos = {x >> 4, z >> 4};
    const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getHeight(x & 15, z & 15);
    }
    return 0;
}

Pos3D World::getHeight(const Pos3D &pos) {
    return {pos.getX(), this->getHeight(pos.getX(), pos.getZ()), pos.getZ()};
}

int World::getTopSolidOrLiquidBlock(const int x, const int z) {
    const Pos2D chunkPos = {x >> 4, z >> 4};
    const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
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

bool World::canBlockFreeze(const Pos3D &pos, const bool noWaterAdj) {
    if (Biome::getBiomeForId(g->getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F)
        return false;

    if (pos.getY() >= 0 && pos.getY() < 256) {
        const Pos2D chunkPos = {pos.getX() >> 4, pos.getZ() >> 4};
        const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
        if (chunk) {
            const bool valid = chunk->canBlockFreeze(pos.convertToChunkCords());
            if (!noWaterAdj) return valid;

            if (!valid) return false;
            for (const auto faces : FACING_HORIZONTAL) {
                if (chunk->canBlockFreeze(pos.offset(faces).convertToChunkCords())) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

ND bool World::canBlockFreezeWater(const Pos3D &pos) {
    return this->canBlockFreeze(pos, false);
}

bool World::canSnowAt(const Pos3D &pos, const bool checkLight) {
    if (Biome::getBiomeForId(g->getBiomeAt(1, pos.getX(), pos.getZ()))->getFloatTemperature(pos) >= 0.15F) {
        return false;
    }

    if (!checkLight) { return true; }

    // TODO: needs to check block light later on to replacate a perfect chunk
    if (pos.getY() >= 0 && pos.getY() < 256) {
        const Pos2D chunkPos = {pos.getX() >> 4, pos.getZ() >> 4};
        const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
        if (chunk) {
            return chunk->canSnowAt(pos.convertToChunkCords());
        }
    }
    return false;
}


void World::generateMineshafts() {
    auto mineshaft_locations = Placement::Mineshaft::getAllPositions(*g);

    for (auto& pos : mineshaft_locations) {
        auto mineshaft_gen = gen::Mineshaft();
        mineshaft_gen.generate(g->getConsole(), g->getWorldSeed(), pos.toChunkPos());
        mineshafts.push_back(mineshaft_gen);
    }
}


void World::generateVillages() {
    auto village_locations = Placement::Village<false>::getAllPositions(g);

    for (auto& village_pos : village_locations) {
        auto village_gen = gen::Village(g);
        village_gen.generate(village_pos.toChunkPos());
        villages.emplace_back(village_gen);
    }
}


void World::generateStrongholds() {
    Pos2D strongholdPos = Placement::Stronghold::getWorldPosition(*g).toChunkPos();
    strongholds.emplace_back();
    strongholds[0].generate(g->getWorldSeed(), strongholdPos);
    std::cout << "Stronghold piece count: " << strongholds[0].getPieceCount() << std::endl;
}

















