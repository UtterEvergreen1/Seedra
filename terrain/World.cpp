#include "World.hpp"

#include "Chunk.hpp"
#include "structures/gen/village/village.hpp"
#include "structures/placement/StaticStructures.hpp"
#include "structures/placement/mineshaft.hpp"
#include "structures/placement/stronghold.hpp"
#include "terrain/biomes/biome.hpp"

#include "Seedra/structures/gen2/woodland_mansion/WoodlandMansionPieces.hpp"
#include "Seedra/structures/gen2/ocean_monument/OceanMonumentPieces.hpp"
#include "common/AreaRange.hpp"
#include "common/timer.hpp"
#include "structures/gen/ScatteredFeature.hpp"
#include "structures/gen2/TemplateManager.hpp"
#include "structures/placement/DynamicStructures.hpp"
#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/carve/ChunkGenerator.hpp"
#include "terrain/carve/RavineGenerator.hpp"
#include "terrain/carve/WaterCaveGenerator.hpp"
#include "terrain/carve/WaterRavineGenerator.hpp"
#include <algorithm>


World::World(Generator *_g) : chunkPool(ChunkPrimer::getFootprintSize()), g(_g) {
    int worldSize = lce::getChunkWorldBounds(g->getWorldSize());
    this->g->setupNoiseStack(); // setup noise for the world to avoid future data races
    this->worldBounds = BoundingBox(
        static_cast<bbType_t>(-worldSize),
        0,
        static_cast<bbType_t>(-worldSize),
        static_cast<bbType_t>(worldSize),
        256,
        static_cast<bbType_t>(worldSize)
    );
}

World::~World() {
    this->deleteWorld();
}

void World::deleteWorld() {
    // Free or pool all live chunks
    for (auto &entry : chunks) {
        ChunkPrimer *chunk = entry.second;
        if (chunk) {
            chunk->reset();
            chunkPool.releaseToPool(chunk);
        }
    }
    chunks.clear();

    villages.clear();
    strongholds.clear();
    mineshafts.clear();
    // scattered features
    for (const auto* feature : scattered_features) {
        delete feature;
        feature = nullptr;
    }
    scattered_features.clear();
    // monuments
    // for (auto* piece : monuments) {
    //     auto* monument = reinterpret_cast<OceanMonumentPieces::MonumentBuilding*>(piece);
    //     delete monument;
    //     piece = nullptr;
    // }
    // monuments.clear();

    // Reset last-chunk caches
    lastChunk.store(nullptr);
    lastChunkCoords.store(Pos2D(-100000, -100000));
}


void World::createChunks(const AreaRange& range) {
    for (Pos2D p : range) {
        getOrCreateChunk(p);
    }
}


void World::decorateCaves(const AreaRange& range, bool hasWaterCaves) {
    Pos2D lower = range.getLower();
    Pos2D upper = range.getUpper();

    BoundingBox bounds = BoundingBox::makeChunkBox(lower.x , lower.z);
    bounds.encompass(BoundingBox::makeChunkBox(upper.x, upper.z));


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

    for (auto chunk : range) {
        if (ChunkPrimer* chunkPrimer = getChunk(chunk); chunkPrimer != nullptr) {
            chunkPrimer->stage = Stage::STAGE_STRUCTURE;
        }
    }
}


void World::decorateChunks(const AreaRange& range) {
    for (Pos2D p : range) {
        if (this->chunkExists(p))
            Chunk::populateChunk(*this, p);
    }
}

Biome* World::getBiomeAt(int x, int z) const {
    return Biome::getBiomeForId(this->getBiomeIdAt(x, z));
}

biome_t World::getBiomeIdAt(int x, int z) const {
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
        mineshafts.emplace_back();
        mineshafts.back().generate(g->getConsole(), g->getWorldSeed(), pos.toChunkPos());
    }
}


void World::generateVillages() {
    const auto village_locations = Placement::Village<false>::getAllPositions(g);

    for (auto& village_pos : village_locations) {
        villages.emplace_back(g);
        gen::Village& back = villages.back();
        back.generate(village_pos.toChunkPos());

        if (!back.hasMoreThanTwoComponents()) {
            villages.pop_back();
        }
    }
}


void World::generateStrongholds() {
    std::vector<Pos2D> strongholdPositions = Placement::Stronghold::getWorldPositions(*g);
    strongholds.reserve(strongholdPositions.size());
    for (auto& strongholdPos : strongholdPositions) {
        strongholds.emplace_back();
        strongholds.back().generate(g->getWorldSeed(), strongholdPos.toChunkPos());
    }
    // std::cout << "Stronghold piece count: " << strongholds[0].getPieceCount() << std::endl;
}



void World::generateMonuments() {
    Placement::Monument::setWorldSize(g->getWorldSize());

    std::vector<Pos2D> monumentBlockPositions =
            Placement::Monument::getAllPositions(g);
    monumentBlockPositions.emplace_back(100, 100);

    monuments.reserve(monumentBlockPositions.size());


    for (const Pos2D& blockPos : monumentBlockPositions) {
        // Vanilla-ish structure seed – adjust constant to match your port
        RNG structureRng(0
                // RNG::getStructureSeed(
                //         g->getWorldSeed(),
                //         chunkPos.x,
                //         chunkPos.z,
                //         10387313ull // vanilla ocean monument salt
                //         )
        );

        // If your ctor takes block coords:
        auto* monument = new OceanMonumentPieces::MonumentBuilding(structureRng, blockPos.x, blockPos.z, EnumFacing::NORTH);
        monuments.emplace_back(reinterpret_cast<void*>(monument));

        // Or if it expects chunk coords:
        // monuments.emplace_back(structureRng, chunkPos.x, chunkPos.z);
    }
}




void World::generateScatteredFeatures() {
    auto features = Placement::Feature::getAllFeaturePositions(g);

    for (auto& feature : features) {
        auto* featureObj = scattered_features::ScatteredFeatureFactory(g, feature);
        if (featureObj != nullptr) {
            std::cout << "Placing Feature at " << feature.m_pos << "\n";
            scattered_features.emplace_back(featureObj);
        }
    }

    // DataFixer dataFixer;
    // TemplateManager templateManager("structures", dataFixer);

    // Timer timer;
    // RNG rng(0);
    // Pos3D origin{60, 110, 252};
    // WoodlandMansionPieces::generateAt(
    //         templateManager,
    //         origin,
    //         Rotation::NONE,
    //         wm_pieces,
    //         rng
    // );
    // auto end = timer.getSeconds();
    // std::cout << "Time to gen mansion: " << end << "\n";

    // for (auto& f : wm_pieces) {
    //     std::cout << "min=(" << f.m_minX << ", " << f.m_minY << ", "<< f.m_minZ << ") " <<
    //                  "max=(" << f.m_maxX << ", " << f.m_maxY << ", "<< f.m_maxZ << ") " <<
    //                   f.getName() << "\n";
    // }

}
