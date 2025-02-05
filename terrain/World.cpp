#include "World.hpp"

#include "Chunk.hpp"
#include "structures/gen/village/village.hpp"
#include "structures/placement/StaticStructures.hpp"
#include "structures/placement/mineshaft.hpp"
#include "structures/placement/stronghold.hpp"
#include "terrain/biomes/biome.hpp"

#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/carve/ChunkGenerator.hpp"
#include "terrain/carve/RavineGenerator.hpp"
#include "terrain/carve/WaterCaveGenerator.hpp"
#include "terrain/carve/WaterRavineGenerator.hpp"

World::World(Generator *g) : g(g) {
    int worldSize = lce::getChunkWorldBounds(g->getWorldSize());
    this->worldBounds = BoundingBox(-worldSize, 0, -worldSize, worldSize, 256, worldSize);
}

World::~World() {
    this->deleteWorld();
}

void World::deleteWorld() {
    for (auto &chunk : chunks) {
        delete chunk.second;
    }
    delete biomes;
    chunks.clear();
    villages.clear();
    strongholds.clear();
    mineshafts.clear();
}

void World::addChunk(const Pos2D &pos, ChunkPrimer *chunk) {
    std::lock_guard lock(chunkMutex);
    chunks[pos] = chunk;
}

ChunkPrimer *World::getChunk(const Pos2D &pos) {
    if (lastChunkCoords == pos) {
        if (lastChunk != nullptr) {
            return lastChunk;
        }
    }

    if (pos.x < worldBounds.minX || pos.x > worldBounds.maxX || pos.z < worldBounds.minZ ||
        pos.z > worldBounds.maxZ) {
        return nullptr;
    }

    lastChunkCoords = pos;
    std::lock_guard lock(chunkMutex);
    const auto it = chunks.find(pos);
    if (it != chunks.end()) {
        lastChunk = it->second;
        return it->second;
    }

    return nullptr;
}

ChunkPrimer *World::getOrCreateChunk(const Pos2D &chunkPos) {
    ChunkPrimer* chunk = getChunk(chunkPos);
    if (chunk != nullptr) {
        return chunk;
    }

    if (chunkPos.x < worldBounds.minX || chunkPos.x > worldBounds.maxX || chunkPos.z < worldBounds.minZ ||
        chunkPos.z > worldBounds.maxZ) {
        return nullptr;
    }

    chunk = Chunk::provideChunk(*this->g, chunkPos);
    lastChunk = chunk;
    addChunk(chunkPos, chunk);
    return chunk;
}


void World::createChunks(const Pos2D &pos, c_int radius) {
    std::cout << "Creating chunks around " << pos << " with radius " << radius << std::endl;
    for (int dx = radius; dx >= -radius; --dx) {
        for (int dz = radius; dz >= -radius; --dz) {
            Pos2D chunkPos = pos + Pos2D(dx, dz);
            this->getOrCreateChunk(chunkPos);
        }
    }
}


void World::decorateCaves(const Pos2D &theStartPosition, c_int radius, bool hasWaterCaves) {
    std::cout << "Carving chunks around " << theStartPosition << " with radius " << radius << std::endl;

    Pos2D lower = theStartPosition - radius;
    Pos2D upper = theStartPosition + radius;

    BoundingBox bounds = BoundingBox::makeChunkBox(lower.x + 1, lower.z + 1);
    bounds.encompass(BoundingBox::makeChunkBox(upper.x - 1, upper.z - 1));


    // Water Caves
    if (hasWaterCaves) {
        const Pos2DVec_t waterCavePositions = WaterCaveGenerator::getStartingChunks(this->g, lower, upper);
        const Pos2D seedMultiplierWaterCaves = WaterCaveGenerator::getSeedMultiplier(g);
        for (c_auto &waterCavePos: waterCavePositions) {
            WaterCaveGenerator waterCaveGen(*this);
            waterCaveGen.setupRNG(seedMultiplierWaterCaves, waterCavePos);
            waterCaveGen.setGenBounds(bounds);
            waterCaveGen.addFeature(*this, waterCavePos, true);
        }
    }

    // Water Ravines
    if (hasWaterCaves) {
        const Pos2DVec_t waterRavinePositions = WaterRavineGenerator::getStartingChunks(this->g, lower, upper);
        const Pos2D seedMultiplierWaterRavine = WaterRavineGenerator::getSeedMultiplier(g);
        for (c_auto &waterRavinePos: waterRavinePositions) {
            WaterRavineGenerator waterRavineGen(*this);
            waterRavineGen.setupRNG(seedMultiplierWaterRavine, waterRavinePos);
            waterRavineGen.setGenBounds(bounds);
            waterRavineGen.addFeature(*this, waterRavinePos, true);
        }
    }

    // Caves
    {
        const Pos2DVec_t cavePositions = CaveGenerator::getStartingChunks(this->g, lower, upper);
        const Pos2DTemplate<i64> seedMultiplierCave = CaveGenerator::getSeedMultiplier(g);
        for (c_auto &cavePos: cavePositions) {
            CaveGenerator caveGen(*this);
            caveGen.setupRNG(seedMultiplierCave, cavePos);
            caveGen.setGenBounds(bounds);
            caveGen.addFeature(*this, cavePos, true);
        }
    }

    // Ravines
    {
        const Pos2DVec_t ravinePositions = RavineGenerator::getStartingChunks(this->g, lower, upper);
        const Pos2DTemplate<i64> seedMultiplierRavine = RavineGenerator::getSeedMultiplier(g);
        for (c_auto &ravinePos: ravinePositions) {
            RavineGenerator ravineGen(*this);
            ravineGen.setupRNG(seedMultiplierRavine, ravinePos);
            ravineGen.setGenBounds(bounds);
            ravineGen.addFeature(*this, ravinePos, true);
        }
    }

    for (auto [pos, chunkPrimer] : chunks) {
        if (chunkPrimer != nullptr) {
            chunkPrimer->stage = Stage::STAGE_STRUCTURE;
        }
    }
}


void World::decorateChunks(const Pos2D &pos, c_int radius) {
    std::cout << "Decorating chunks around " << pos << " with radius " << radius << std::endl;
    generateWorldBiomes();
    for (int dx = radius; dx >= -radius; --dx) {
        for (int dz = radius; dz >= -radius; --dz) {
            Pos2D chunkPos = pos + Pos2D(dx, dz);
            if (this->getOrCreateChunk(chunkPos))
                Chunk::populateChunk(*this, chunkPos);
        }
    }
}

void World::generateWorldBiomes() {
    // Generate biomes for the whole world
    delete this->biomes;
    this->biomes = g->getBiomeRange(1, worldBounds.minX << 4, worldBounds.minZ << 4, worldBounds.maxX << 5, worldBounds.maxZ << 5);
}

int* World::getChunkBiomes(const Pos2D& pos) {
    if (biomes == nullptr) { return nullptr; }

    BoundingBox bounds = worldBounds << 4;
    Pos2D chunkBlockPos = pos.toBlockPos();
    return biomes + (chunkBlockPos.z + bounds.maxZ) * (bounds.maxX * 2) + (chunkBlockPos.x + bounds.maxX);
}

Biome* World::getBiomeAt(int x, int z) const {
    return Biome::getBiomeForId(this->getBiomeIdAt(x, z));
}

int World::getBiomeIdAt(int x, int z) const {
    BoundingBox bounds = worldBounds << 4;
    if (!bounds.isVecInside({x, 0, z})) {
        return 0;
    }
    return this->biomes[(z + bounds.maxZ) * (bounds.maxX * 2) + (x + bounds.maxX)];
}

int World::getBlockId(c_int x, c_int y, c_int z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlockId(x & 15, y, z & 15);
    }
    return 0;
}

int World::getBlockId(const Pos3D &pos) {
    return this->getBlockId(pos.x, pos.y, pos.z);
}


lce::BlockState World::getBlock(c_int x, c_int y, c_int z) {
    if (const ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->getBlock(x & 15, y, z & 15);
    }
    return lce::BlocksInit::AIR.getState();
}

lce::BlockState World::getBlock(const Pos3D &pos) {
    return this->getBlock(pos.x, pos.y, pos.z);
}

void World::notifyNeighbors(c_int x, c_int y, c_int z) {
    //mimic notify neighbors to load chunks
    for (const auto faces: FACING_HORIZONTAL) {
        (void) this->getBlock(Pos3D(x, y, z).offset(faces));
    }
}

void World::setBlock(c_int x, c_int y, c_int z, c_int blockId) {
    ChunkPrimer *chunk = this->getOrCreateChunk({x >> 4, z >> 4});
    if (chunk == nullptr) return;
    chunk->setBlockId(x & 15, y, z & 15, blockId);
    notifyNeighbors(x, y, z);
}

void World::setBlockId(const Pos3D &pos, int blockId) {
    this->setBlock(pos.x, pos.y, pos.z, blockId);
}


// TODO: remove
void World::setBlock(c_int x, c_int y, c_int z, c_int blockId, c_int meta) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    if (chunk == nullptr) return;
    chunk->setBlockAndData(x & 15, y, z & 15, blockId, meta);
    notifyNeighbors(x, y, z);
}

void World::setBlock(const Pos3D &pos, c_int blockId, c_int meta) {
    this->setBlock(pos.x, pos.y, pos.z, blockId, meta);
}

void World::setBlock(c_int x, c_int y, c_int z, const lce::BlockState blockstate) {
    this->setBlock(x, y, z, blockstate.getID(), blockstate.getDataTag());
}

void World::setBlock(const Pos3D &pos, const lce::BlockState blockstate) {
    this->setBlock(pos.x, pos.y, pos.z, blockstate.getID(), blockstate.getDataTag());
}


bool World::isAirBlock(c_int x, c_int y, c_int z) {
    if (const ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4})) {
        return chunk->isAirBlock(x & 15, y, z & 15);
    }
    return false;
}

bool World::isAirBlock(const Pos3D &pos) {
    return this->isAirBlock(pos.x, pos.y, pos.z);
}

int World::getHeight(c_int x, c_int z) {
    const Pos2D chunkPos = {x >> 4, z >> 4};
    const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getHeight(x & 15, z & 15);
    }
    return 0;
}

Pos3D World::getHeight(const Pos3D &pos) {
    return {pos.x, this->getHeight(pos.x, pos.z), pos.z};
}

int World::getTopSolidOrLiquidBlock(c_int x, c_int z) {
    const Pos2D chunkPos = {x >> 4, z >> 4};
    const ChunkPrimer *chunk = getOrCreateChunk(chunkPos);
    if (chunk) {
        return chunk->getTopSolidOrLiquidBlock(x & 15, z & 15);
    }
    return 0;
}

Pos3D World::getTopSolidOrLiquidBlock(const Pos3D &pos) {
    return {pos.x, this->getTopSolidOrLiquidBlock(pos.x, pos.z), pos.z};
}

bool World::isSnowyAt(int x, int z) const {
    return this->getBiomeAt(x, z)->isSnowyBiome();
}

bool World::hasIdealTemperature(const Pos3D &pos) const {
    return this->getBiomeAt(pos.getX(), pos.getZ())->hasIdealTemperature(pos);
}

int World::getPrecipitationHeight(c_int x, c_int z) {
    ChunkPrimer *chunk = getOrCreateChunk({x >> 4, z >> 4});
    if (chunk) {
        return chunk->getPrecipitationHeight(x & 15, z & 15);
    }
    return 0;
}

bool World::canBlockFreeze(const Pos3D &pos, const bool noWaterAdj) {
    if (!this->getBiomeAt(pos.getX(), pos.getZ())->hasIdealTemperature(pos))
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

ND bool World::canBlockFreezeWater(const Pos3D &pos) {
    return this->canBlockFreeze(pos, false);
}

bool World::canSnowAt(const Pos3D &pos, const bool checkLight) {
    if (!this->getBiomeAt(pos.getX(), pos.getZ())->hasIdealTemperature(pos))
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
