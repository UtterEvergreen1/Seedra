// ReSharper disable CppDFAUnreachableCode
#include "common/range.hpp"
#include "structures/placement/stronghold.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/carve/ChunkGenerator.hpp"

int main() {
    constexpr int64_t FLOOR_HEIGHT = 63;
    constexpr int64_t CACTUS_HEIGHT = FLOOR_HEIGHT + 1;
    constexpr int64_t MAX_SISTER_SEED = 0x7FFF000000000000;
    std::string inputFile = "xaktoos_population_seeds" + std::to_string(CACTUS_HEIGHT) + ".txt";
    std::vector<std::tuple<uint64_t, Pos2D, Pos2D>> populationSeeds;
    {
        std::ifstream in(inputFile);
        uint64_t s;
        int x, z, offX, offZ;
        while (in >> s >> x >> z >> offX >> offZ) {
            populationSeeds.emplace_back(s, Pos2D(x, z), Pos2D(offX, offZ));
        }
        in.close();
    }
    Biome::registerBiomes();

    Generator g(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    for (auto& [seed, originalChunkPos, offsetPos] : populationSeeds) {
        // if (!(originalChunkPos > 0)) continue;
        if (originalChunkPos.x < -6 || originalChunkPos.x > 6 || originalChunkPos.z < -6 || originalChunkPos.z > 6) continue;
        // if (seed == 81991907540167) continue;

        std::unique_ptr<ChunkPrimer> cactusChunk = std::make_unique<ChunkPrimer>();
        g.applyWorldSeed(seed);

        ChunkGeneratorOverWorld chunk(g);
        Pos2D chunkPos = (originalChunkPos.toBlockPos() + offsetPos).toChunkPos();
        offsetPos = {offsetPos.x & 15, offsetPos.z & 15};
        int biomeLen = g.getMinCacheSize(4, 10, 10);
        chunk.biomesForGeneration = static_cast<biome_t *>(calloc(biomeLen, sizeof(int)));
        std::fill_n(chunk.biomesForGeneration, biomeLen, desert);
        chunk.setBlocksInChunk(chunkPos.x, chunkPos.z, cactusChunk.get());

        // get offset pos height
        int height = cactusChunk->getHeight(offsetPos.x, offsetPos.z);

        // ReSharper disable once CppDFAConstantConditions
        if (height != CACTUS_HEIGHT) {
            continue;
        }

        // CLion thinks beyond here is unreachable, but it is reachable

        // check surrounding blocks for floor height
        if (offsetPos.z > 0) {
            int heightN = cactusChunk->getHeight(offsetPos.x, offsetPos.z - 1);
            if (heightN > CACTUS_HEIGHT) {
                continue;
            }
        }
        if (offsetPos.z < 15) {
            int heightS = cactusChunk->getHeight(offsetPos.x, offsetPos.z + 1);
            if (heightS > CACTUS_HEIGHT) {
                continue;
            }
        }
        if (offsetPos.x > 0) {
            int heightE = cactusChunk->getHeight(offsetPos.x - 1, offsetPos.z);
            if (heightE > CACTUS_HEIGHT) {
                continue;
            }
        }
        if (offsetPos.x < 15) {
            int heightW = cactusChunk->getHeight(offsetPos.x + 1, offsetPos.z);
            if (heightW > CACTUS_HEIGHT) {
                continue;
            }
        }

        // Generate the actual chunk to check for tall cactus
        free(chunk.biomesForGeneration);
        biomeLen = g.getMinCacheSize(1, 16, 16);
        chunk.biomesForGeneration = static_cast<biome_t*>(calloc(biomeLen, sizeof(int)));
        std::fill_n(chunk.biomesForGeneration, biomeLen, desert);
        chunk.replaceBiomeBlocks(chunkPos.x, chunkPos.z, cactusChunk.get());

        World world(&g);
        world.addChunk(chunkPos, cactusChunk.get());
        cactusChunk->stage = Stage::STAGE_DECORATE;
        cactusChunk->decorateRng = RNG::getPopulationSeed(g.getWorldSeed(), chunkPos.x, chunkPos.z);
        Chunk::populateDecorations(world, chunkPos);

        // Check if the cactus is tall enough
        bool isValidCactus = true;
        for (int y = CACTUS_HEIGHT; y < CACTUS_HEIGHT + 13 && isValidCactus; ++y) {
            if (cactusChunk->getBlockId(offsetPos.x, y, offsetPos.z) != lce::blocks::BlockID::CACTUS_ID) {
                isValidCactus = false;
            }
        }

        if (!isValidCactus) continue;

        // std::cout << "Checking structure seed: " << seed << " at: " << (chunkPos.toBlockPos() + offsetPos) << std::endl;
        for (int64_t sisterSeed = seed | 0x8000000000000000LL; sisterSeed < MAX_SISTER_SEED; sisterSeed += 0x1000000000000) {
            if (sisterSeed < 0) continue;
            g.applyWorldSeed(sisterSeed);
            Pos2D biomeCheck1 = (originalChunkPos + 1).toBlockPos();
            if (g.getBiomeIdAt(1, biomeCheck1.x, biomeCheck1.z) != desert) {
                continue; // biome check failed
            }

            Pos2D biomeCheck4Min = originalChunkPos * 4 - 2;
            Pos2D biomeCheck4Max = biomeCheck4Min + 10;
            bool validBiomes = true;
            for (Pos2D pos = biomeCheck4Min; pos.x <= biomeCheck4Max.x && validBiomes; ++pos.x) {
                for (pos.z = biomeCheck4Min.z; pos.z <= biomeCheck4Max.z && validBiomes; ++pos.z) {
                    if (g.getBiomeIdAt(4, pos.x, pos.z) != desert) {
                        validBiomes = false;
                    }
                }
            }

            if (!validBiomes) continue;

            std::cout << "Valid seed: " << sisterSeed << " at: " << (chunkPos.toBlockPos() + offsetPos) << std::endl;
            break; // only output the first valid sister seed
        }
    }

    /*Biome::registerBiomes();
    Generator g(lce::CONSOLE::XBOX1, LCEVERSION::ELYTRA, 2, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    World world(&g);
    Pos2D chunkPos = {-14, -18};
    ChunkPrimer* chunkPrimer = Chunk::provideNewChunk(g, chunkPos);
    world.addChunk(chunkPos, chunkPrimer);
    chunkPrimer->stage = Stage::STAGE_DECORATE;
    chunkPrimer->decorateRng = RNG::getPopulationSeed(g.getWorldSeed(), chunkPos.x, chunkPos.z);
    Chunk::populateDecorations(world, chunkPos);*/



    /*Generator g(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 5023952512342200539 , lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::LARGE);
    // g.generateCache(4);
    Pos2D strongholdPos = Placement::Stronghold::getWorldPosition(g);
    std::cout << "Stronghold Position: " << strongholdPos << std::endl;*/
}