#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"

#include "common/timer.hpp"
#include "structures/gen/village/village.hpp"
#include "structures/placement/StaticStructures.hpp"

namespace fs = std::filesystem;


template<typename T>
void fileWrite(std::ofstream& file, T obj) {
    file.write(reinterpret_cast<const char*>(&obj), sizeof(T));
}


int main() {
    const char* userProfile = std::getenv("USERPROFILE");
    if (!userProfile) {
        std::cerr << "USERPROFILE environment variable not set!\n";
        return 1;
    }
    fs::path dirPath = fs::path(userProfile)
                       / "CLionProjects"
                       / "LegacyChunkViewer";
    std::string dirName = dirPath.string();
    std::cout << "Using \"" + dirName + "\"...\n\n";
    fs::path filePath = dirPath / "build" / "chunks" / "chunkdata.bin";


    // auto* primer = new ChunkPrimer;
    // primer->setBlock(0, 0, 0, &lce::BlocksInit::SPRUCE_WOOD_STAIRS);
    // bool canSnowAt = primer->canSnowAt({0, 1, 0});
    // std::cout << canSnowAt << "\n";



    Biome::registerBiomes();
    // -1204924307554348042;
    c_i64 WORLD_SEED = -1204924307554348042; // 615831156172857837; // 7710511010199114;
    c_auto CONSOLE = lce::CONSOLE::WIIU;
    c_auto VERSION = LCEVERSION::ELYTRA;
    c_auto WORLD_SIZE = lce::WORLDSIZE::CLASSIC;
    c_auto BIOME_SCALE = lce::BIOMESCALE::SMALL;
    // -6651998285536156346
    Generator g(CONSOLE, VERSION, WORLD_SEED, WORLD_SIZE, BIOME_SCALE);

    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int X_WIDTH  = 27; // 10;
    int Z_WIDTH  = 27; // 10;
    int X_CENTER = 0; // 9;
    int Z_CENTER = 0; // 210 / 16;

    Timer start;

    auto world = World(&g);

    world.generateWorldBiomes();
    world.generateMineshafts();
    world.generateVillages();
    world.generateStrongholds();

    world.getOrCreateChunk({X_CENTER, Z_CENTER});

    bool TODO = true;

    world.createChunks({X_CENTER, Z_CENTER}, X_WIDTH);
    world.decorateCaves({X_CENTER, Z_CENTER}, X_WIDTH, TODO);

    world.decorateChunks({X_CENTER, Z_CENTER}, X_WIDTH);

    std::cout << "World Gen Time: " << start.getSeconds() << "\n";


    std::ofstream file(filePath.string(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filePath << std::endl;
        return -1;
    }

    int64_t worldSeed = g.getWorldSeed();

    fileWrite<i64>(file, worldSeed);
    fileWrite<int>(file, X_CENTER);
    fileWrite<int>(file, Z_CENTER);
    fileWrite<int>(file, X_WIDTH);
    fileWrite<int>(file, Z_WIDTH);

    int sx = (X_CENTER - X_WIDTH) * 16;
    int sz = (Z_CENTER - Z_WIDTH) * 16;
    int sw = X_WIDTH * 2 * 16;
    int sh = Z_WIDTH * 2 * 16;

    std::cout << "Generating biomes at "
              << sx << ", "
              << sz << ", "
              << sx + sw <<  ", "
              << sz + sh << std::endl;

    int* biomes = world.getGenerator()
        ->getBiomeRange(1, sx, sz, sw, sh);

    if (biomes == nullptr) {
        std::cerr << "Error loading biome map, exiting..." << std::endl;
        return -1;
    } else {
        std::cout << "Loaded Biome Map, writing chunks to file" << std::endl;
    }

    for (int cx = -(X_WIDTH) + X_CENTER; cx < X_WIDTH + X_CENTER - 1; cx++) {
        for (int cz = -(Z_WIDTH) + Z_CENTER; cz < Z_WIDTH + Z_CENTER - 1; cz++) {
            Pos2D chunkPos(cx, cz);
            auto* chunkPrimer = world.getChunk({cx, cz});

            if (chunkPrimer == nullptr) {
                std::cerr << "Error getting chunk: " << chunkPos << std::endl;
                continue;
            }

            u8 biomeChunk[256];

            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    int bx = (chunkPos.x + X_WIDTH) * 16 + x;
                    int bz = (chunkPos.z + Z_WIDTH) * 16 + z;
                    biomeChunk[z * 16 + x] = (u8)biomes[bz * sw + bx];
                }
            }


            chunkPrimer->highestYBlock = -1;
            chunkPrimer->highestYBlock = std::min(chunkPrimer->getHighestYBlock() + 1, 255);
            i64 sizeToWrite = (i64) sizeof(chunkPrimer->blocks[0]) * 256 * chunkPrimer->highestYBlock;

            fileWrite<int>(file, chunkPos.x);
            fileWrite<int>(file, chunkPos.z);
            fileWrite<int>(file, chunkPrimer->highestYBlock);
            file.write(reinterpret_cast<const char*>(&biomeChunk[0]), 256);
            file.write(reinterpret_cast<const char*>(&chunkPrimer->blocks[0]), sizeToWrite);


            if (!file) { std::cerr << "Error writing to file: " << filePath << std::endl; }
        }
    }

    delete biomes;

    return 0;
}




// replace all stone with air
// for (int i = 0; i < 16 * 16 * 256; i++) {
//     if ((chunk->getBlockAtIndex(i) >> 4) == 1 && (chunk->getBlockAtIndex(i) & 15) == 0) {
//         chunk->blocks[i] = 0;
//     }
// }

// useful for set certain blocks in the world
/*
//set all the blocks to all the blocks available
lce::registry::BlockRegistry blocks;
blocks.setup();
int x = 0;
int y = 0;
int z = 0;
for (auto block : blocks.REGISTRY.getAllValues()) {
    world->setBlock(x, y, z, block->object->getID(), block->object->getDataTag());
    x+=2;
    if (x == 16) {
        x = 0;
        z+=2;
        if (z == 16) {
            z = 0;
            y+=2;
        }
    }
}*/





