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
    for (auto& [pos, chunk] : chunks) {
        if (chunk) {
            reusableChunks.push_back(chunk); // move to pool instead of deleting
        }
    }
    chunks.clear();
    villages.clear();
    strongholds.clear();
    mineshafts.clear();
}

void World::createChunks(const Pos2D &pos, c_int radius) {
    //std::cout << "Creating chunks around " << pos << " with radius " << radius << std::endl;
    for (int dx = radius; dx >= -radius; --dx) {
        for (int dz = radius; dz >= -radius; --dz) {
            Pos2D chunkPos = pos + Pos2D(dx, dz);
            this->getOrCreateChunk(chunkPos);
        }
    }
}


void World::decorateCaves(const Pos2D &theStartPosition, c_int radius, bool hasWaterCaves) {
    //std::cout << "Carving chunks around " << theStartPosition << " with radius " << radius << std::endl;
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
    //std::cout << "Decorating chunks around " << pos << " with radius " << radius << std::endl;
    for (int dx = radius; dx >= -radius; --dx) {
        for (int dz = radius; dz >= -radius; --dz) {
            Pos2D chunkPos = pos + Pos2D(dx, dz);
            if (this->chunkExists(chunkPos))
                Chunk::populateChunk(*this, chunkPos);
        }
    }
}

Biome* World::getBiomeAt(int x, int z) const {
    return Biome::getBiomeForId(this->getBiomeIdAt(x, z));
}

int World::getBiomeIdAt(int x, int z) const {
    return g->getBiomeIdAt(1, x, z);
}

bool World::isSnowyAt(int x, int z) const {
    return this->getBiomeAt(x, z)->isSnowyBiome();
}

bool World::hasIdealTemperature(const Pos3D &pos) const {
    return this->getBiomeAt(pos.getX(), pos.getZ())->hasIdealTemperature(pos);
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
